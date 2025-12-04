#pragma once

#include "iron/result.hpp"
#include "iron/base/entry.hpp"

#include <boost/unordered_map.hpp>

#include <iostream>
#include <vector>

namespace Iron {

    namespace ConfigParser {

        enum TokenType {
            CC_TOK_ID,
            CC_TOK_SET,
            CC_TOK_VALUE,
            CC_TOK_LIST,
            CC_TOK_SUB,
        };

        struct Token {
            TokenType type;
            std::string value = "";
            int line = 0;
            int column = 0;
        };

        std::vector<Token> GetTokens(std::string raw);

        class Parser {

            boost::unordered_map<std::string, Entry> entries;
            std::vector<Token> tokens;
            int consumedTokens = 0;

            Result<Token> Consume();
            Result<Token> Peek();
            void Rewind();
            Result<std::vector<Token>> ExpectEntry();
            Result<std::vector<std::string>> ExpectValues();

            public:

            Result<boost::unordered_map<std::string, Entry>> GetMap(std::vector<Token> tokens);

        };

    }

}