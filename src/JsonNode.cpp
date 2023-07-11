#include <stdexcept>
#include "cppjson.h"

namespace json {
    JsonNode::JsonNode(Any _value) : value(std::move(_value)) {}

    JsonNode::JsonNode() : value(Null{}) {}

    JsonNode &JsonNode::operator[](const std::string &key) {
        if (std::get_if<Null>(&value)) {
            this->value = Object();
        }
        if (auto object = std::get_if<Object>(&value)) {
            return object->operator[](key);
        }
        throw std::runtime_error("not object");
    }


    JsonNode &JsonNode::operator[](size_t key) {
        if (std::get_if<Null>(&value)) {
            this->value = Array();
        }
        if (auto object = std::get_if<Array>(&value)) {
            if (object->size() <= key)object->resize(key + 1);
            return object->at(key);
        }
        throw std::runtime_error("not array");
    }

    void JsonNode::append(const Any &obj) {
        if (std::get_if<Null>(&value)) {
            this->value = Array();
        }
        if (auto object = std::get_if<Array>(&value)) {
            object->emplace_back(obj);
        } else {
            throw std::runtime_error("not array");
        }
    }

    void JsonNode::append(const std::pair<std::string, Any> &pair) {
        if (std::get_if<Null>(&value)) {
            this->value = Object();
        }
        if (auto object = std::get_if<Object>(&value)) {
            object->insert(pair);
        } else {
            throw std::runtime_error("not object");
        }
    }

    void JsonNode::remove(const std::string &key) {
        if (std::get_if<Null>(&value)) {
            this->value = Object();
        }
        if (auto object = std::get_if<Object>(&value)) {
            object->erase(key);
        } else {
            throw std::runtime_error("not object");
        }
    }

    JsonNode &JsonNode::operator=(const Any &any) {
        this->value = any;
        return *this;
    }
}

std::ostream &operator<<(std::ostream &out, const json::JsonNode &jsonNode) {
    return out << json::JsonGenerator()(jsonNode);
}

