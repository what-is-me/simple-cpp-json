#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <iostream>

namespace json {
    struct JsonNode;
    using Null = std::monostate;
    using Bool = bool;
    using Int = int64_t;
    using Float = double;
    using String = std::string;
    using Array = std::vector<JsonNode>;
    using Object = std::map<std::string, JsonNode>;
    using Any = std::variant<Null, Bool, Int, Float, String, Array, Object>;

    struct JsonNode {
        Any value;

        explicit JsonNode(Any _value) : value(std::move(_value)) {}

        JsonNode() : value(Null{}) {}

        JsonNode &operator[](const std::string &key) {
            if (std::get_if<Null>(&value)) {
                this->value = Object();
            }
            if (auto object = std::get_if<Object>(&value)) {
                return object->operator[](key);
            }
            throw std::runtime_error("not object");
        }


        JsonNode &operator[](size_t key) {
            if (std::get_if<Null>(&value)) {
                this->value = Array();
            }
            if (auto object = std::get_if<Array>(&value)) {
                if (object->size() <= key)object->resize(key + 1);
                return object->at(key);
            }
            throw std::runtime_error("not array");
        }

        void append(const Any &obj) {
            if (std::get_if<Null>(&value)) {
                this->value = Array();
            }
            if (auto object = std::get_if<Array>(&value)) {
                object->emplace_back(obj);
            } else {
                throw std::runtime_error("not array");
            }
        }

        void append(const std::pair<std::string, Any> &pair) {
            if (std::get_if<Null>(&value)) {
                this->value = Object();
            }
            if (auto object = std::get_if<Object>(&value)) {
                object->insert(pair);
            } else {
                throw std::runtime_error("not object");
            }
        }

        template<typename ...Args>
        void append(const Any &item, Args... args) {
            this->append(item);
            this->append(args...);
        }

        void remove(const std::string &key) {
            if (std::get_if<Null>(&value)) {
                this->value = Object();
            }
            if (auto object = std::get_if<Object>(&value)) {
                object->erase(key);
            } else {
                throw std::runtime_error("not object");
            }
        }

        JsonNode &operator=(const Any &any) {
            this->value = any;
            return *this;
        }
    };

    struct JsonGenerator {

        std::string operator()(const Null &null) {
            return "null";
        }

        std::string operator()(const Bool &aBool) {
            return aBool ? "true" : "false";
        }

        std::string operator()(const Int &anInt) {
            return std::to_string(anInt);
        }

        std::string operator()(const Float &aFloat) {
            return std::to_string(aFloat);
        }

        std::string operator()(const String &string) {
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

        std::string operator()(const Array &array) {
            if (array.empty())return "[]";
            std::string ret = "[";
            for (const auto &item: array) {
                ret.append(this->operator()(item));
                ret.push_back(',');
            }
            ret.back() = ']';
            return ret;
        }

        std::string operator()(const Object &object) {
            if (object.empty())return "{}";
            std::string ret = "{";
            for (const auto &[key, value]: object) {
                ret.append(this->operator()(key) + ":" + this->operator()(value));
                ret.push_back(',');
            }
            ret.back() = '}';
            return ret;
        }

        std::string operator()(const JsonNode &node) {
            return this->operator()(node.value);
        }

        std::string operator()(const Any &any) {
            return std::visit(JsonGenerator(), any);
        }
    };

    class JsonParser {
    public:
        explicit JsonParser(const std::string &json) : src(json), pos(0), len(json.size()) {}

        JsonNode parse() {
            pos = 0;
            if (len == 0) {
                throw std::runtime_error("empty string");
            }
            passWhiteSpace();
            return JsonNode(parseAny());
        }

    private:
        std::string_view src;
        size_t pos;
        const size_t len;


        String parseString() {
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

        Null parseNull() {
            if (src.substr(pos, 4) == "null") {
                pos += 4;
                passWhiteSpace();
                return Null{};
            }
            throw std::runtime_error("illegal word");
        }

        Bool parseBool() {
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
            throw std::runtime_error("illegal word");
        }

        Any parseNumber() {
            size_t endPos = pos;
            bool isFloat = false;
            for (; endPos < len; endPos++) {
                if (src[endPos] == 'e' ||
                    src[endPos] == '.') {
                    isFloat = true;
                    continue;
                }
                if (!std::isdigit(src[endPos]) && src[endPos] != '-') {
                    break;
                }
            }
            std::string number = std::string{src.substr(pos, endPos - pos)};
            pos = endPos;
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


        Array parseArray() {
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

        Object parseObject() {
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

        void passWhiteSpace() {
            while (pos < len && std::isspace(src[pos])) {
                ++pos;
            }
        }


        Any parseAny() {
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
    };
}

inline std::ostream &operator<<(std::ostream &out, const json::JsonNode &jsonNode) {
    return out << json::JsonGenerator()(jsonNode);
}