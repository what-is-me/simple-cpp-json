#include <set>
#include <stdexcept>
#include "cppjson.h"

namespace json {
    JsonNode JsonParser::parse() {
        pos = 0;
        if (len == 0) {
            throw std::runtime_error("empty string");
        }
        passWhiteSpace();
        return JsonNode(parseAny());
    }

    String JsonParser::parseString() {
        std::string str;
        pos++;
        for (; src[pos] != '"' && pos < len; pos++) {
            if (src[pos] == '\\') {
                pos++;
                switch (src[pos]) {
                    case 'n':
                        str.push_back('\n');
                        break;
                    case 't':
                        str.push_back('\t');
                        break;
                    case 'r':
                        str.push_back('\r');
                        break;
                    case '\\':
                    case '\'':
                    case '\"':
                    case '\?':
                        str.push_back(src[++pos]);
                        break;
                    default:
                        throw std::runtime_error("wrong escape character");
                }
            } else {
                str.push_back(src[pos]);
            }
        }
        pos++;
        passWhiteSpace();
        return str;
    }

    Null JsonParser::parseNull() {
        if (src.substr(pos, 4) == "null") {
            pos += 4;
            passWhiteSpace();
            return Null{};
        }
        throw std::runtime_error("illegal word: " + src.substr(pos, 4));
    }

    Bool JsonParser::parseBool() {
        if (src.substr(pos, 4) == "true") {
            pos += 4;
            passWhiteSpace();
            return true;
        }
        if (src.substr(pos, 5) == "false") {
            pos += 5;
            passWhiteSpace();
            return false;
        }
        throw std::runtime_error("illegal word: " + src.substr(pos, 4));
    }

    Any JsonParser::parseNumber() {
        size_t endpos = pos;
        bool isFloat = false;
        for (; endpos < len; endpos++) {
            if (src[endpos] == 'e' ||
                src[endpos] == '.') {
                isFloat = true;
                continue;
            }
            if (!std::isdigit(src[endpos])) {
                break;
            }
        }
        std::string number = std::string{src.substr(pos, endpos - pos)};
        pos = endpos;
        passWhiteSpace();
        try {
            if (isFloat) {
                return std::stod(number);
            } else {
                return std::stoi(number);
            }
        }
        catch (...) {
            throw std::runtime_error("cannot cast \"" + number + "\" to number");
        }
    }


    Array JsonParser::parseArray() {
        pos++;
        Array ret;
        while (src[pos] != ']') {
            ret.emplace_back(parseAny());
            if (src[pos] == ',') {
                pos++;
            } else if (src[pos] == ']') {
                break;
            } else {
                throw std::runtime_error(std::string("illegal word: \'") + src[pos] + "\'");
            }
        }
        pos++;
        passWhiteSpace();
        return ret;
    }

    Object JsonParser::parseObject() {
        Object obj;
        pos++;
        while (src[pos] != '}') {
            passWhiteSpace();
            std::string key = parseString();
            if (src[pos] == ':') {
                pos++;
            } else {
                throw std::runtime_error(std::string("illegal word: \'") + src[pos] + "\'");
            }
            obj.emplace(key, parseAny());
            if (src[pos] == ',') {
                pos++;
            } else if (src[pos] == '}') {
                break;
            } else {
                throw std::runtime_error(std::string("illegal word: \'") + src[pos] + "\'");
            }
        }
        pos++;
        passWhiteSpace();
        return obj;
    }

    void JsonParser::passWhiteSpace() {
        while (pos < len && std::isspace(src[pos])) {
            ++pos;
        }
    }

    JsonParser::JsonParser(const std::string &json) : src(json), pos(0), len(json.size()) {}

    Any JsonParser::parseAny() {
        passWhiteSpace();
        switch (src[pos]) {
            case 'n':
                return parseNull();
            case 't':
            case 'f':
                return parseBool();
            case '"':
                return parseString();
            case '[':
                return parseArray();
            case '{':
                return parseObject();
            default:
                return parseNumber();
        }
    }

}