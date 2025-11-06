#include "iron/config.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <string>

using namespace Iron;

enum ConfigTokenType {
	TOK_TEXT,
	TOK_COLON,
	TOK_LIST_ITEM,
	TOK_STRING,
	TOK_END,
	TOK_EMPTY,
};

struct ConfigToken {
	std::string data = "";
	ConfigTokenType type = TOK_EMPTY;
	int atColumn = -1;
	int onLine = -1;

	ConfigToken(int atColumn, int onLine) {
		this->atColumn = atColumn;
		this->onLine = onLine;
	}
	ConfigToken() {}
};

std::vector<ConfigToken> LexString(std::string line, int lineNumber) {
	std::vector<ConfigToken> tokens;
	ConfigToken token = ConfigToken(0, lineNumber);

	bool stringConsumer = false;

	for(int i = 0; i < line.length(); i++) {
		char currentChar = line.at(i);

		if(currentChar == '#' && !stringConsumer) {
			tokens.push_back(token);
			return tokens;
		}

		if(stringConsumer) {
			if(currentChar == '"') {
				stringConsumer = false;
				tokens.push_back(token);
				token = ConfigToken(i, lineNumber);
			} else token.data += currentChar;

			continue;
		}

		if(currentChar == '"') {
			stringConsumer = true;
			token.type = TOK_STRING;

			continue;
		}

		bool isColon = currentChar == ':';
		bool isGTSymbol = currentChar == '>';

		if(isColon || isGTSymbol) {
			tokens.push_back(token);
			token = ConfigToken(i, lineNumber);
			token.type = isColon ? TOK_COLON : TOK_LIST_ITEM;
			tokens.push_back(token);

			continue;
		}

		if(currentChar == ' ' && token.type != TOK_TEXT) {
			continue;
		}

		if(token.type == TOK_EMPTY) {
			token.type = TOK_TEXT;
		}

		token.data += currentChar;
	}

	return tokens;
}

enum CTNodeType {
	TREE_ORIGIN,
	TREE_DEFINITION,
	TREE_ITEM,
};

struct CTParserNode {
	CTNodeType type;
	std::string data;
	std::vector<CTParserNode> children;

	CTParserNode(CTNodeType type, std::string data) {
		this->type = type;
		this->data = data;
	}
};

enum CTParserFailure {
	PARSER_EXPECTED_DEFINITION,
	PARSER_EXPECTED_LIST_ITEM,
	PARSER_FAILED,
};

class CTParser {

	std::vector<ConfigToken> tokens;
	CTParserNode origin = CTParserNode(TREE_ORIGIN, "Origin");
	int numTokensConsumed = 0;

	std::string RepresentTokenPosition(ConfigToken token) {
		std::string expected = "line " + token.onLine;
		expected += " at column " + token.atColumn;
		return expected;
	}

	ConfigToken ConsumeToken() {
		ConfigToken token = tokens.at(numTokensConsumed);
		numTokensConsumed++;
		return token;
	}

	ConfigToken PeekToken() {
		return tokens.at(numTokensConsumed);
	}

	ConfigToken PeekToken(int offset) {
		return tokens.at(numTokensConsumed + offset);
	}

	bool IsTextual(ConfigTokenType type) {
		return type == TOK_STRING || type == TOK_TEXT;
	}

	Result<ConfigToken> ExpectDefinition() {
		ConfigToken name = ConsumeToken();
		ConfigToken colon = ConsumeToken();

		if(name.type == TOK_TEXT && colon.type == TOK_COLON) {
			return Result<ConfigToken>(name);
		} else return Result<ConfigToken>(Failure("Expected definition on " + RepresentTokenPosition(name), PARSER_EXPECTED_DEFINITION));
	}

	bool ExpectListItem() {
		ConfigToken listItem = ConsumeToken();
		return listItem.type == TOK_LIST_ITEM;
	}

	Result<std::vector<ConfigToken>> ExpectList() {
		std::vector<ConfigToken> tokens;

		int numListItem = 0;
		int numOthers = 0;
		bool expectListItem = true;

		while(numListItem > numOthers) {
			if(expectListItem) {
				if(!ExpectListItem()) break;

				numListItem++;
				expectListItem = false;
				continue;
			}

			ConfigToken token = ConsumeToken();

			if(!IsTextual(token.type)) {
				return Result<std::vector<ConfigToken>>(Failure("Expected list item char (>) on " + RepresentTokenPosition(token), PARSER_EXPECTED_LIST_ITEM));
			}

			tokens.push_back(token);
			numOthers++;
			expectListItem = true;
		}

		return Result<std::vector<ConfigToken>>(tokens);
	}

	public:

	CTParser(std::vector<ConfigToken> tokens) {
		this->tokens = tokens;
	}

	Result<CTParserNode> ParseTokens() {
		while(numTokensConsumed < tokens.size()) {
			Result<ConfigToken> tokenResult = ExpectDefinition();

			if(!tokenResult.Success()) {
				return Result<CTParserNode>(tokenResult.GetFailure());
			}

			CTParserNode definition = CTParserNode(TREE_DEFINITION, tokenResult.GetValue().data);

			ConfigToken token = PeekToken();

			if(IsTextual(token.type)) {
				ConsumeToken();
				definition.children.push_back(CTParserNode(TREE_ITEM, token.data));
				origin.children.push_back(definition);
				continue;
			} else if(token.type != TOK_LIST_ITEM) return Result<CTParserNode>(
					Failure(
						"Expected list item char (>) on " + RepresentTokenPosition(token), 
						PARSER_EXPECTED_LIST_ITEM
					)
				);

			Result<std::vector<ConfigToken>> listResult = ExpectList();

			if(!listResult.Success()) {
				return Result<CTParserNode>(listResult.GetFailure());
			}

			std::vector<ConfigToken> list = listResult.GetValue();

			for(int i = 0; i < list.size(); i++) {
				definition.children.push_back(CTParserNode(TREE_ITEM, list.at(i).data));
			}
		}

		return Result<CTParserNode>(origin);
	}
};

boost::unordered_map<std::string, ConfigEntry> CompileTree(CTParserNode tree) {
	boost::unordered_map<std::string, ConfigEntry> map;

	for(int i = 0; i < tree.children.size(); i++) {
		CTParserNode definition = tree.children.at(i);
		std::vector<CTParserNode> items = definition.children;

		if(items.size() == 1) {
			map[definition.data] = ConfigEntry(items.at(0).data, IRON_ENTRY_STRING, definition.data);
			continue;
		}

		std::vector<std::string> list;

		for(int i = 0; i < items.size(); i++) {
			list.push_back(items.at(i).data);
		}

		map[definition.data] = ConfigEntry(list, IRON_ENTRY_STRING_LIST, definition.data);
	}

	return map;
}

Config::Config(std::string path) {
	this->path = path;
}

Result<ConfigLoadStatus> Config::Load() {
	boost::filesystem::path configPath(path);

	if(!boost::filesystem::exists(configPath)) {
		return Result<ConfigLoadStatus>(Failure("The path: " + path + " was not found on the system", IRON_CONFIG_FILE_NOT_FOUND));
	}

	boost::filesystem::ifstream config(configPath);
	std::string line;
	std::vector<ConfigToken> tokenList;
	int numLines = 0;

	while(std::getline(config, line)) {
		std::vector<ConfigToken> newTokens = LexString(line, numLines);
		tokenList.insert(tokenList.end(), newTokens.begin(), newTokens.end());
		numLines++;
	}

	config.close();

	CTParser parser = CTParser(tokenList);

	Result<CTParserNode> tree = parser.ParseTokens();

	if(!tree.Success()) {
		return Result<ConfigLoadStatus>(Failure(tree.GetFailure().GetFailureReason(), IRON_CONFIG_PARSER_FAILED));
	}

	map = CompileTree(tree.GetValue());

	return Result<ConfigLoadStatus>(IRON_CONFIG_OKAY);
}

Result<ConfigEntry> Config::GetEntry(std::string name) {
	if(!HasEntry(name)) {
		return Result<ConfigEntry>(Failure("Entry " + name + " doesn't exist in config file: " + path, 0));
	}

	return Result<ConfigEntry>(map[name]);
}

bool Config::HasEntry(std::string name) {
	return map.contains(name);
}