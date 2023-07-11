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

        explicit JsonNode(Any _value);

        JsonNode();

        JsonNode &operator[](const std::string &key);

        JsonNode &operator[](size_t key);

        void append(const Any &obj);

        template<typename ...Args>
        void append(const Any &item, Args... args) {
            this->append(item);
            this->append(args...);
        }

        void append(const std::pair<std::string, Any> &pair);

        void remove(const std::string &key);

        JsonNode &operator=(const Any &any);
    };

    struct JsonGenerator {

        std::string operator()(const Null &null);

        std::string operator()(const Bool &aBool);

        std::string operator()(const Int &anInt);

        std::string operator()(const Float &aFloat);

        std::string operator()(const String &string);

        std::string operator()(const Array &array);

        std::string operator()(const Object &object);

        std::string operator()(const JsonNode &node);

        std::string operator()(const Any &any);
    };

    class JsonParser {
    public:
        explicit JsonParser(const std::string &json);

        JsonNode parse();

    private:
        const std::string &src;
        size_t pos;
        const size_t len;

        String parseString();

        Null parseNull();

        Bool parseBool();

        Any parseNumber();

        Array parseArray();

        Object parseObject();

        Any parseAny();

        void passWhiteSpace();
    };
}

std::ostream &operator<<(std::ostream& out, const json::JsonNode &jsonNode);