# Simple Cpp Json

A Very Simple C++ JSON library.

## About

This is a very simple c++ json library using **c++20** standard.
In this project, we use `std::variant` for mutable types.
This is a header-only library, just include the `cppjson.hpp` and it can be used anywhere.
It only relies on standard library.

## Platforms

c++ compiler must support `c++20` standard.

## Apis and Example

### Basic Types

```cpp
using Null = std::monostate;                      // null
using Bool = bool;                                // bool: true/false
using Int = int64_t;                              // integer
using Float = double;                             // float
using String = std::string;                       // string
using Array = std::vector<JsonNode>;              // array
using Object = std::map<std::string, JsonNode>;   // object
using Any = std::variant<Null, Bool, Int, Float, String, Array, Object>;
class JsonNode{
    Any value;
};
```

### Methods of JsonNode

| method                                                 | description                                                                                                                                                                                                                                           |
|:-------------------------------------------------------|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `JsonNode()`                                           | this->value = null                                                                                                                                                                                                                                    |
| `JsonNode(value)`                                      | this->value = value                                                                                                                                                                                                                                   |
| `JsonNode &operator[](const std::string &key)`         | if type of value is object, get the reference of value by key. if type of value is null, it will be altered to object. and after that, get the reference of value by key.                                                                             |
| `JsonNode &operator[](size_t key)`                     | if type of value is array, get the reference of value by index(key). if type of value is null, it will be altered to array. and after that, get the reference of value by key. if the size of array is smaller than key, the array will be augmented. |
| `void append(const std::pair<std::string, Any> &pair)` | if type of value is object, pair[key, value] will be pushed into it.                                                                                                                                                                                  |
| `void remove(const std::string &key)`                  | if type of value is object, remove key-value pair by key.                                                                                                                                                                                             |
| `void append(const Any &obj)`                          | if type of value is array, obj will be pushed into back of it. arr.append(a) is ok, arr.append(a,b,c,d) is ok, too.                                                                                                                                   |
| `JsonNode &operator=(const Any &any)`                  | for easy assigning value. this->value = any                                                                                                                                                                                                           |

#### Example

```cpp
int main(){
    json::JsonNode jsonNode, simpleNode;
    //now jsonNode=null, simpleNode=null
    jsonNode["a"]["b"]["c"] = json::Array();
    //now jsonNode={"a":{"b":{"c":[]}}}
    jsonNode["a"]["c"][1] = 1;
    //now jsonNode={"a":{"b":{"c":[]},"c":[null,1]}}
    jsonNode["xx"].append({"yy", "zz"});
    //now jsonNode={"a":{"b":{"c":[]},"c":[null,1]},"xx":{"yy":"zz"}}
    jsonNode["zz"].append(1, 1.1, true, json::Null{}, "aaa");
    //now jsonNode={"a":{"b":{"c":[]},"c":[null,1]},"xx":{"yy":"zz"},"zz":[1,1.100000,true,null,"aaa"]}
    simpleNode.append(1, 2, 3, 4, 5);
    //now simpleNode=[1,2,3,4,5]
    simpleNode = 114;
    //now simpleNode=114
    simpleNode = 114.514;
    //now simpleNode=114.514
    simpleNode = "data";
    //now simpleNode="data"
}
```

### Methods of JsonGenerator

`JsonGenerator` generate a JsonNode into json string.

```cpp
JsonNode jsonNode;
// do sth to jsonNode, like append or modify.
json::JsonGenerator() generator;
std::string json_str = generator(jsonNode);
std::cout << json_str << std::endl;
//also, you can write as follows, we override ostream<<JsonNode:
std::cout << jsonNode << std::endl;
```

### Methods of JsonPhaser

just use `JsonNode jsonNode = json::JsonParser(json_str).parse();` to parse string json_str into JsonNode.

#### example

```cpp
int main(){
    std::string json = R"(
        [
            13,
            29,
            3.33,
            {
                "one": true,
                "two": {
                    "two-one": -1.3e9,
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
```

Results:

```txt
$.0 = 13      
$.1 = 29      
$.2 = 3.330000
$.3 = {"one":true,"three":999,"two":{"two-one":-1300000000.000000,"two-three":"hello\t\"cpp\"","two-two":null}}
$.4 = true
$.5 = false
$.6 = [1,2,3,"1","2","3"]
$.7 = "foo"
$.8 = "baz"
$.3.three.four = null
$.3.two.two-one = -1300000000.000000
$.100 = null
```