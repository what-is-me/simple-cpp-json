#include "cppjson.h"
#include <iostream>
#include <functional>
#include <chrono>

int main() {
    json::JsonNode jsonNode, simpleNode;
    jsonNode["a"]["b"]["c"] = json::Array();
    jsonNode["a"]["c"][1] = 1;
    jsonNode["xx"].append({"yy", "zz"});
    jsonNode["zz"].append(1, 1.1, true, json::Null{}, "aaa");
    std::cout << jsonNode << std::endl;
    simpleNode.append(1, 2, 3, 4, 5);
    std::cout << simpleNode << std::endl;
    simpleNode = 114;
    std::cout << simpleNode << std::endl;
    simpleNode = 114.514;
    std::cout << simpleNode << std::endl;
    simpleNode = "data";
    std::cout << simpleNode << std::endl;

    std::cout << std::endl;

    std::string json = R"(
        [
            13,
            29,
            3.33,
            {
                "one": true,
                "two": {
                    "two-one": 1.3e9,
                    "two-two": null,
                    "two-three": "hello\t\\"cpp\\""
                },
                "three": 999
            },
            true,
            false,
            [1, 2, 3],
            "foo",
            "baz"
        ]
    )";
    jsonNode = json::JsonParser(json).parse();
    jsonNode[6].append("1", "2", "3");
    for (int i = 0; i < 9; i++) {
        std::cout << "$." << i << " = " << jsonNode[i] << std::endl;
    }
    std::cout << "$.3.three.four = " << jsonNode[3]["two"]["four"] << std::endl;
    std::cout << "$.3.two.two-one = " << jsonNode[3]["two"]["two-one"] << std::endl;
    std::cout << "$.100 = " << jsonNode[100] << std::endl;
}