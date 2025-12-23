#pragma once

#include "iron/result.hpp"
#include "iron/fs/fs_map.hpp"

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

        typedef struct {
            unsigned int type;
            GrammarTypes dataType = IRON_GRAMMAR_STRING;
            bool required = false;
            bool unique = false;
            bool composed = false;
            int minBytes = 0;
            int maxBytes = 0;
            std::vector<ElemExp> composition;
        } ElemExp;

        typedef struct {
            unsigned int binType;
            bool required = false;
            bool unique = false;
        } BinExp;

        typedef struct {
            unsigned int type;
            std::vector<uchar> data;
            std::vector<Element> composites;
        } Element;

        typedef struct {
            unsigned int type;
            std::vector<Element> elements;
            std::vector<Bin> bins;
        } Bin;

        class Grammar {

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
            std::vector<int> magic;
            Map* map;

            int GetSequentialId(unsigned int bin, bool createIfNotFound);
            int GetElementId(unsigned int elem, bool createIfNotFound);

            Result<Bin> ReadBin();
            Result<Element> ReadElement(int bin);
            Result<Element> ReadCompositeElement(int elem, int bin);
            Result<std::vector<uchar>> ReadBytes(int num);
            ApparentID PeekType(unsigned int bin);

            protected:

            void SetMagic(unsigned int magic...);
            Result<EngineResult> Parse(Map* map);
            std::vector<uchar> Serialize(std::vector<Bin> bins);

            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, bool required, bool unique);
            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, int byteCount, bool required, bool unique);
            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, int minBytes, int maxBytes, bool required, bool unique);
            ElemExp ElementExpectation(unsigned int type, bool required, bool unique, int numComposites, ElemExp composites[]);

            BinExp BinExpectation(unsigned int type, bool required, bool unique);

            void DefineBinElements(unsigned int bin, int numElems, ElemExp elems[]);
            void DefineSubBins(unsigned int bin, int numBins, BinExp subBins[]);

            std::vector<Bin> parsedBins;

        };

    }

}