#include "config_parser.hpp"

using namespace Iron;
using namespace ConfigParser;

std::vector<Token> ConfigParser::GetTokens(std::string raw) {
    std::vector<Token> out;
    int lines = 0;
    bool readValue = false;
    bool readEntry = false;
    std::string value = "";

    for(int i = 0; i < raw.size(); i++) {
        char c = raw.at(i);

        if(c == '\n') {
            if(readEntry) {
                out.push_back(Token{CC_TOK_ID, value, lines, i});
                readEntry = false;
                value = "";
            }

            lines++;
        }

        if(readValue) {
            if(c == '"') {
                out.push_back(Token{CC_TOK_VALUE, value, lines, i});
                readValue = false;
                value = "";
            }

            value += c;
        }

        else if(c == '"') {
            readValue = true;
        }

        else if(c == ':' || c == '.') {
            if(readEntry) {
                out.push_back(Token{CC_TOK_ID, value, lines, i});
                readEntry = false;
                value = "";
            }

            if(c == ':') {
                out.push_back(Token{CC_TOK_SET, ":", lines, i});
                continue;
            }

            out.push_back(Token{CC_TOK_SUB, ".", lines, i});
        }

        else if(c == '>') {
            out.push_back(Token{CC_TOK_LIST, ">", lines, i});
        }

        else {
            readEntry = true;
            value += c;
        }
    }

    return out;
}

Result<Token> Parser::Peek() {
    if(consumedTokens >= tokens.size()) {
        return Failure(IRON_RESULT_TOO_BIG);
    }

    Token token = tokens.at(consumedTokens);
    return token;
}

Result<Token> Parser::Consume() {
    auto token = Peek();

    if(token.Success()) {
        consumedTokens++;
    }

    return token;
}

void Parser::Rewind() {
    if(consumedTokens >= 0) {
        return;
    }

    consumedTokens--;
}

std::string utilTokenName(TokenType type) {
    switch(type) {
        case CC_TOK_ID:
        return "identifier";
        case CC_TOK_LIST:
        return ">";
        case CC_TOK_SET:
        return ":";
        case CC_TOK_SUB:
        return ".";
        case CC_TOK_VALUE:
        return "value";
        default:
        return "unknown token";
    }
}

std::string utilExpectationFailed(std::string what, std::string got = "", int line = -1, int column = -1) {
    std::string err = "Expected " + what;
    
    if(column != -1 && line != -1) {
        err += " at line ";
        err += line;
        err += " on column ";
        err += column;
    }

    if(got != "") {
        err += ", got " + got;
    }

    return err;
}

Result<std::vector<Token>> Parser::ExpectEntry() {
    auto getId = Peek();
    auto getDelim = Peek();

    std::vector<Token> idList;

    while(getId.Success() && getDelim.Success()) {
        getId = Consume();
        getDelim = Consume();

        if(!getId.Success()) {
            return Failure(utilExpectationFailed(utilTokenName(CC_TOK_ID), "eof"), IRON_RESULT_UNINITIALIZED);
        }

        if(!getDelim.Success()) {
            return Failure(utilExpectationFailed(
                utilTokenName(CC_TOK_LIST) + " or " + utilTokenName(CC_TOK_SET), "eof"), 
                IRON_RESULT_UNINITIALIZED
            );
        }

        auto id = getId.GetValue();

        if(id.type != CC_TOK_ID) {
            return Failure(
                utilExpectationFailed(utilTokenName(CC_TOK_ID), utilTokenName(id.type), id.line, id.column),
                IRON_RESULT_IMPROPER_FORMAT
            );
        }

        idList.push_back(id);

        auto delim = getDelim.GetValue();

        if(delim.type == CC_TOK_SET) {
            return idList;
        }

        else if(delim.type == CC_TOK_SUB) {
            continue;
        }

        return Failure(utilExpectationFailed(
            utilTokenName(CC_TOK_LIST) + " or " + utilTokenName(CC_TOK_SET), utilTokenName(delim.type)), 
            IRON_RESULT_IMPROPER_FORMAT
        );
    }
}

Result<std::vector<std::string>> Parser::ExpectValues() {
    std::vector<std::string> values;
    auto check = Peek();

    if(!check.Success()) {
        return Failure(
            utilExpectationFailed(
                utilTokenName(CC_TOK_ID) + " or " + utilTokenName(CC_TOK_LIST), "eof"), 
                IRON_RESULT_IMPROPER_FORMAT
            );
    }

    auto checkToken = check.GetValue();

    if(checkToken.type == CC_TOK_VALUE) {
        Consume();
        values.push_back(checkToken.value);
        return values;
    }

    else if(checkToken.type != CC_TOK_LIST) {
        return Failure(
            utilExpectationFailed(
                utilTokenName(CC_TOK_LIST), utilTokenName(checkToken.type), checkToken.line, checkToken.column), 
                IRON_RESULT_IMPROPER_FORMAT
            );
    }

    auto getListItem = Peek();
    auto getValue = Peek();

    while(getListItem.Success() && getValue.Success()) {
        getListItem = Consume();
        getValue = Consume();

        if(!getListItem.Success()) {
            return Failure(utilExpectationFailed(utilTokenName(CC_TOK_LIST), "eof"), IRON_RESULT_UNINITIALIZED);
        }

        if(!getValue.Success()) {
            return Failure(utilExpectationFailed(utilTokenName(CC_TOK_LIST), "eof"), IRON_RESULT_UNINITIALIZED);
        }

        auto listItem = getListItem.GetValue();

        if(listItem.type != CC_TOK_LIST) {
            if(listItem.type == CC_TOK_ID) {
                Rewind(); // So that Parser::ExpectEntry() does not fail.
                return values;
            }

            return Failure(
                utilExpectationFailed(
                    utilTokenName(CC_TOK_ID), utilTokenName(listItem.type), listItem.line, listItem.column),
                    IRON_RESULT_IMPROPER_FORMAT
                );
        }

        auto value = getValue.GetValue();

        if(value.type != CC_TOK_VALUE) {
            return Failure(
                utilExpectationFailed(
                    utilTokenName(CC_TOK_VALUE), value.value, value.line, value.column),
                    IRON_RESULT_IMPROPER_FORMAT
                );
        }

        values.push_back(value.value);
    }
}

Result<boost::unordered_map<std::string, Entry>> Parser::GetMap(std::vector<Token> tokens) {
    this->tokens = tokens;

    while(consumedTokens < tokens.size()) {
        auto getEntry = ExpectEntry();

        if(!getEntry.Success()) {
            return getEntry.GetFailure();
        }

        auto getValues = ExpectValues();

        if(!getValues.Success()) {
            return getValues.GetFailure();
        }

        auto nameList = getEntry.GetValue();
        Token root = nameList.at(0);

        if(nameList.size() == 1) {
            entries[root.value] = Entry(root.value, getValues.GetValue());
            continue;
        }

        entries[root.value] = Entry(root.value, "");

        Entry* current = &entries[root.value];
        Token sub;

        for(int i = 1; i < nameList.size(); i++) {
            sub = nameList.at(i);

            if(!current->HasSubEntry(sub.value)) {
                current->AddSubEntry(new Entry(sub.value, ""));
            }

            auto getNext = current->SubEntry(sub.value);
            current = getNext.GetValue();
        }

        current = new Entry(sub.value, getValues.GetValue());
    }

    return entries;
}