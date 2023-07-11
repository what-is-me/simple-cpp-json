#include "cppjson.h"

namespace json {

    std::string JsonGenerator::operator()(const Null &null) {
        return "null";
    }

    std::string JsonGenerator::operator()(const Bool &aBool) {
        return aBool ? "true" : "false";
    }

    std::string JsonGenerator::operator()(const Int &anInt) {
        return std::to_string(anInt);
    }

    std::string JsonGenerator::operator()(const Float &aFloat) {
        return std::to_string(aFloat);
    }

    std::string JsonGenerator::operator()(const String &string) {
        std::string ret = "\"";
        for (char ch: string) {
            switch (ch) {
                case '\n':
                    ret.append("\\n");
                    break;
                case '\t':
                    ret.append("\\t");
                    break;
                case '\r':
                    ret.append("\\r");
                    break;
                case '\\':
                    ret.append("\\\\");
                    break;
                case '\'':
                    ret.append("\\\'");
                    break;
                case '\"':
                    ret.append("\\\"");
                    break;
                default:
                    ret.push_back(ch);
            }
        }
        return ret + "\"";
    }

    std::string JsonGenerator::operator()(const Array &array) {
        if (array.empty())return "[]";
        std::string ret = "[";
        for (const auto &item: array) {
            ret.append(this->operator()(item));
            ret.push_back(',');
        }
        ret.back() = ']';
        return ret;
    }

    std::string JsonGenerator::operator()(const Object &object) {
        if (object.empty())return "{}";
        std::string ret = "{";
        for (const auto &[key, value]: object) {
            ret.append(this->operator()(key) + ":" + this->operator()(value));
            ret.push_back(',');
        }
        ret.back() = '}';
        return ret;
    }

    std::string JsonGenerator::operator()(const JsonNode &node) {
        return this->operator()(node.value);
    }

    std::string JsonGenerator::operator()(const Any &any) {
        return std::visit(JsonGenerator(), any);
    }
}
