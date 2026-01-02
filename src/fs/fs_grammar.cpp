#include "iron/fs/fs_grammar.hpp"
#include "iron/result.hpp"

using namespace Iron;
using namespace FileSystem;

#define GRAMMAR_NO_ID -1
#define READ_STRING 0

int Grammar::GetSequentialId(unsigned int bin, bool createIfNotFound) {
    for(int i = 0; i < internalBins.size(); i++) {
        if(internalBins.at(i).type == bin) {
            return i;
        }
    }

    if(createIfNotFound) {
        InternalBin ib;
        ib.type = bin;
        internalBins.push_back(ib);

        return internalBins.size() - 1;
    }

    return GRAMMAR_NO_ID;
}

int Grammar::GetElementId(unsigned int elem, bool createIfNotFound) {
    for(int i = 0; i < internalElems.size(); i++) {
        if(internalElems.at(i).type == elem) {
            return i;
        }
    }

    if(createIfNotFound) {
        InternalElem ie;
        ie.type = elem;
        internalElems.push_back(ie);

        return internalElems.size() - 1;
    }

    return GRAMMAR_NO_ID;
}

void Grammar::SetMagic(uchar magic...) {
    this->magic.clear();
    this->magic.insert(this->magic.begin(), magic);
}

Result<EngineResult> Grammar::Parse(Map* map) {
    if(internalBins.size() == 0) {
        return Failure(IRON_RESULT_FAILED);
    }

    this->map = map;

    if(map->Size() < magic.size()) {
        return Failure(IRON_RESULT_FAILED);
    }

    for(int i = 0; i < magic.size(); i++) {
        auto c = map->Consume();

        if(!c.Success()) {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        if(c.Value() != magic.at(i)) {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }
    }

    parsedBins.clear();

    while(map->GetCharsConsumed() < map->Size()) {
        auto getBin = ReadBin();
        
        if(!getBin.Success()) {
            return getBin.Fail();
        }

        parsedBins.push_back(getBin.Value());
    }

    return IRON_RESULT_LOADED;
}

Result<Bin> Grammar::ReadBin() {
    auto getBinType = map->Consume();
    
    if(!getBinType.Success()) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    auto binType = getBinType.Value();
    int id = GetSequentialId(binType, false);

    if(id == GRAMMAR_NO_ID) {
        std::string err = "Got undefined id: ";
        err += id;
        return Failure(IRON_RESULT_IMPROPER_FORMAT, err);
    }

    Bin bin;
    bin.type = binType;

    while(map->Peek().Success() && map->Peek().Value() != binType) {
        auto peekType = PeekType(id);

        if(peekType == GRAMMAR_INV) {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        else if(peekType == GRAMMAR_BIN) {
            auto getBin = ReadBin();

            if(!getBin.Success()) {
                return getBin.Fail();
            }

            bin.bins.push_back(getBin.Value());
        }

        else if(peekType == GRAMMAR_ELEM) {
            auto getElement = ReadElement(id);

            if(!getElement.Success()) {
                return getElement.Fail();
            }

            bin.elements.push_back(getElement.Value());
        }
    }

    return bin;
}

Result<Element> Grammar::ReadElement(int bin) {
    auto getChar = map->Consume();

    if(!getChar.Success()) {
        return Failure(IRON_RESULT_FAILED);
    }

    auto elemType = getChar.Value();

    if(GetElementId(elemType, false) == GRAMMAR_NO_ID) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    auto binElems = internalBins.at(bin).elems;
    ElemExp rule;
    bool foundRule = false;

    for(int i = 0; i < binElems.size(); i++) {
        if(binElems.at(i).type == elemType) {
            rule = binElems.at(i);
            foundRule = true;
        }
    }

    if(!foundRule) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    return ReadElement(rule);
}

Result<Element> Grammar::ReadElement(ElemExp elem) {
    Element element;
    element.type = elem.type;
    element.internalType = elem.dataType;

    if(elem.composed == true) {
        for(int i = 0; i < elem.composition.size(); i++) {
            auto getElement = ReadElement(elem);

            if(!getElement.Success()) {
                return getElement.Fail();
            }

            element.composites.push_back(getElement.Value());
        }

        return element;
    }

    unsigned int numBytes = READ_STRING;

    if(elem.dataType == IRON_GRAMMAR_VAR_SIZE) {
        auto getNumBytes = map->Consume();

        if(!getNumBytes.Success()) {
            return Failure(IRON_RESULT_IMPROPER_FORMAT);
        }

        numBytes = getNumBytes.Value();
    }

    else if(elem.dataType == IRON_GRAMMAR_BYTE) {
        numBytes = 1;
    }

    else if(elem.dataType == IRON_GRAMMAR_BYTES) {
        numBytes = elem.minBytes;
    }

    auto getBytes = ReadBytes(numBytes);

    if(!getBytes.Success()) {
        return getBytes.Fail();
    }

    element.data = getBytes.Value();

    return element;
}

Result<std::vector<uchar>> Grammar::ReadBytes(int num) {
    if(num == 0) {
        return Failure(IRON_RESULT_IMPROPER_FORMAT);
    }

    std::vector<uchar> bytes;

    if(num > 0) {
        for(int i = 0; i < num; i++) {
            auto c = map->Consume();

            if(!c.Success()) {
                return Failure(IRON_RESULT_FAILED);
            }

            bytes.push_back(c.Value());
        }

        return bytes;
    }

    while(map->Peek().Success() && map->Peek().Value() != 0x00) {
        auto c = map->Consume();
        
        if(!c.Success()) {
            // shouldn't ever get here tbhon
            return Failure(IRON_RESULT_FAILED);
        }

        bytes.push_back(c.Value());
    }

    return bytes;
}

Grammar::ApparentID Grammar::PeekType(unsigned int bin) {
    auto getChar = map->Peek();

    if(!getChar.Success()) {
        return GRAMMAR_INV;
    }

    auto c = getChar.Value();
    int id = GetSequentialId(c, false);

    if(id != GRAMMAR_NO_ID) {
        auto bins = internalBins.at(id).bins;

        for(int i = 0; i < bins.size(); i++) {
            if(bins.at(i).binType == c) {
                return GRAMMAR_BIN;
            }
        }

        return GRAMMAR_INV;
    }

    id = GetElementId(c, false);

    if(id != GRAMMAR_NO_ID) {
        return GRAMMAR_ELEM;
    }

    return GRAMMAR_INV;
}

std::vector<uchar> Grammar::SerializeElement(Element element) {
    std::vector<uchar> out;
    out.push_back(element.type);

    if(element.internalType == IRON_GRAMMAR_COMPOSED) {
        for(int i = 0; i < element.composites.size(); i++) {
            auto chars = SerializeElement(element.composites.at(i));
            out.insert(out.end(), chars.begin(), chars.end());
        }

        return out;
    }

    if(element.internalType == IRON_GRAMMAR_VAR_SIZE) {
        out.push_back(element.data.size());
    }

    out.insert(out.end(), element.data.begin(), element.data.end());
    
    return out;
}

std::vector<uchar> Grammar::Serialize(std::vector<Bin> bins) {
    std::vector<uchar> out;
    out.insert(out.begin(), magic.begin(), magic.end());

    for(int i = 0; i < bins.size(); i++) {
        Bin bin = bins.at(i);
        auto elements = bin.elements;

        out.push_back(bin.type);

        for(int j = 0; j < elements.size(); j++) {
            auto element = elements.at(i);
            auto chars = SerializeElement(element);

            out.insert(out.end(), chars.begin(), chars.end());
        }

        if(!bin.bins.size()) {
            auto subBins = Serialize(bin.bins);
            out.insert(out.end(), subBins.begin(), subBins.end());
        }

        out.insert(out.end(), bin.type);
    }
    
    return out;
}

ElemExp Grammar::ElementExpectation(unsigned int type, GrammarTypes dataType) {
    ElemExp exp;
    exp.type = type;
    exp.dataType = dataType;
    return exp;
}

ElemExp Grammar::ElementExpectation(unsigned int type, GrammarTypes dataType, int byteCount) {
    auto exp = ElementExpectation(type, dataType);
    exp.minBytes = byteCount;
    exp.maxBytes = byteCount;
    return exp;
}

ElemExp Grammar::ElementExpectation(unsigned int type, GrammarTypes dataType, int minBytes, int maxBytes) {
    auto exp = ElementExpectation(type, dataType, minBytes);
    exp.maxBytes = maxBytes;
    return exp;
}

ElemExp Grammar::ElementExpectation(unsigned int type, int numComposites, ElemExp composites[]) {
    auto exp = ElementExpectation(type, IRON_GRAMMAR_COMPOSED);
    exp.composed = true;
    exp.composition.insert(exp.composition.begin(), composites, composites + numComposites);
    return exp;
}

BinExp Grammar::BinExpectation(unsigned int type) {
    BinExp exp = {type};
    return exp;
}

void Grammar::DefineBinElements(unsigned int bin, int numElems, ElemExp elems[]) {
    int id = GetSequentialId(bin, true);
    auto ptr = internalBins.at(id).elems.end();
    internalBins.at(id).elems.insert(ptr, elems, elems + numElems);

    for(int i = 0; i < numElems; i++) {
        int elemId = GetElementId(elems[i].type, true);
        internalElems.at(elemId).binsIn.push_back(bin);
    }
}

void Grammar::DefineSubBins(unsigned int bin, int numSubBins, BinExp subBins[]) {
    int id = GetSequentialId(bin, true);
    auto ptr = internalBins.at(id).bins.end();
    internalBins.at(id).bins.insert(ptr, subBins, subBins + numSubBins);
}

void Grammar::ResetGrammar() {
    internalBins.clear();
    internalElems.clear();
}