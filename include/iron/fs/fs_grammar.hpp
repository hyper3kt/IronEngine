#pragma once

#include "iron/result.hpp"

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

        enum GrammarParseStatus {
            IRON_GRAMMAR_STATUS_FAILED,
            IRON_GRAMMAR_STATUS_CONTINUE,
            IRON_GRAMMAR_STATUS_PARSED,
        };

        typedef struct {
            unsigned int type;
            GrammarTypes dataType = IRON_GRAMMAR_STRING;
            bool required = false;
            bool instance = false;
            bool composed = false;
            int minBytes = 0;
            int maxBytes = 0;
            std::vector<ElemExp> composition;
        } ElemExp;

        typedef struct {
            unsigned int binType;
            bool required = false;
            bool instance = false;
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

            protected:

            void SetMagic(unsigned int magic...);
            Result<GrammarParseStatus> Parse(const uchar* buf);
            std::vector<uchar> Serialize(std::vector<Bin> bins);

            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, bool required, bool unique);
            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, int byteCount, bool required, bool unique);
            ElemExp ElementExpectation(unsigned int type, GrammarTypes dataType, int minBytes, int maxBytes, bool required, bool unique);
            ElemExp ElementExpectation(unsigned int type, bool required, bool unique, ElemExp composites...);

            BinExp BinExpectation(unsigned int type, bool required, bool instance);

            void DefineBinElements(unsigned int bin, ElemExp elems...);
            void DefineSubBins(unsigned int bin, BinExp subBins...);

            std::vector<Bin> parsedBins;

        };

    }

}