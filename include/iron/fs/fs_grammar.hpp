#pragma once

#include "iron/result.hpp"
#include "iron/fs/fs_map.hpp"
#include "iron/macros.hpp"

#include <iostream>
#include <vector>

typedef unsigned char uchar;

namespace Iron {

    namespace FileSystem {

        enum GrammarTypes {
            IRON_GRAMMAR_STRING,
            IRON_GRAMMAR_BYTES,
            IRON_GRAMMAR_BYTE,
            IRON_GRAMMAR_VAR_SIZE,
            IRON_GRAMMAR_COMPOSED,
        };

        typedef struct IronDLL _ElemExp {
            unsigned int type;
            GrammarTypes dataType = IRON_GRAMMAR_STRING;
            bool composed = false;
            int minBytes = 0;
            int maxBytes = 0;
            std::vector<struct _ElemExp> composition;
        } ElemExp;

        typedef struct IronDLL _BinExp{
            unsigned int binType;
        } BinExp;

        typedef struct IronDLL _Element {
            unsigned int type;
            GrammarTypes internalType;
            std::vector<uchar> data;
            std::vector<struct _Element> composites;
        } Element;

        typedef struct IronDLL _Bin {
            unsigned int type;
            std::vector<struct _Element> elements;
            std::vector<struct _Bin> bins;
        } Bin;

        class IronDLL Grammar {

            typedef struct {
                unsigned int type;
                std::vector<ElemExp> elems;
                std::vector<BinExp> bins;
            } InternalBin;

            typedef struct {
                unsigned int type;
                std::vector<unsigned int> binsIn;
            } InternalElem;

            enum ApparentID {
                GRAMMAR_BIN,
                GRAMMAR_ELEM,
                GRAMMAR_INV,
            };

            std::vector<InternalBin> internalBins;
            std::vector<InternalElem> internalElems;
            std::vector<uchar> magic;
            Map* map;

            int GetSequentialId(unsigned int bin, bool createIfNotFound);
            int GetElementId(unsigned int elem, bool createIfNotFound);

            Result<Bin> ReadBin();
            Result<Element> ReadElement(int bin);
            Result<Element> ReadElement(ElemExp elem);
            Result<std::vector<uchar>> ReadBytes(int num);
            ApparentID PeekType(unsigned int bin);

            std::vector<uchar> SerializeElement(Element element);

            protected:

            void SetMagic(uchar magic...);
            Result<EngineResult> Parse(Map* map);
            std::vector<uchar> Serialize(std::vector<Bin> bins);

            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType);
            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, int byteCount);
            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, int minBytes, int maxBytes);
            ElemExp ElementExpectation(unsigned int type, int numComposites, ElemExp composites[]);

            BinExp BinExpectation(unsigned int type);

            void DefineBinElements(unsigned int bin, int numElems, ElemExp elems[]);
            void DefineSubBins(unsigned int bin, int numBins, BinExp subBins[]);
            void ResetGrammar();

            std::vector<Bin> parsedBins;

        };

    }

}
