# CppJson
A single header C++17 [JSON](https://en.wikipedia.org/wiki/JSON) library with a focus a simple declarative syntax.  It should compile with any C++17 compliant compiler without any additional source or binary dependencies.  SSE instructions can be optionally used to accelerate decoding.

# Basic usage
All JSON values are encapsulated in individual ```JsonValue``` objects.  The mapping of C++ types to JSON value types is intended to provide a balance of ease-of-use and functionality:

| JSON type | C++ types |
|-----------|-----------|
| `null`    | `nullptr` |
| `Boolean` | `bool`    |
| `Number`  | `int32_t`, `int64_t`, `uint32_t`, `uint64_t`, `float` and `double` |
| `String`  | UTF-8 encoded `std::string` |
| `Array`   | `JsonArray` (that extends `std::vector<JsonValue>`) |
| `Object`  | `JsonObject` (that extends `std::map<std::string,  JsonValue>`) |

These C++ types can be combined using a declarative initializer-list syntax, or can be built using the more traditional functionality of `std::vector` and `std::map`.

```cpp
// Declarative syntax
JsonObject dog {
    { "Name", "Woofles" },
    { "Age", 4 },
    { "Toys", JsonArray{ "Ball", "Squeeky Bone" } }
};

// std::map syntax
JsonObject dog;
dog["Name"] = "Woofles";
dog["Age"] = 4;
dog["Toys"] = JsonArray{ "Ball", "Squeeky Bone" };
```

JSON values can be serialized and de-serialized using the `ObjectWriter::Write()` and `ObjectReader::Parse()` functions respectively.

# Declarative syntax and efficiency
While the declarative syntax is easier to read (and mirrors similar JavaScript code) it is unable to use move-semantics due to the [use of initializer-lists](https://akrzemi1.wordpress.com/2016/07/07/the-cost-of-stdinitializer_list/).  If the use of move-semantics is important to your application then you should use the `std::vector` and `std::map` syntax instead.

For the majority of applications the performance impact would be negligable.

# Serialization
Support for serialization is currently limited to contiguous `std::string_view` objects.  It is not possible to stream data when either reading or writing.  This was a conscious decision to keep the API as simple as possible while still supporting the needs of dependant projects.

Serialization is performed by the static `ObjectWriter::Write()` function.  It takes a single `JsonValue`, `JsonArray` or `JsonObject` and serializes it to a UTF-8 encoded `std::string`.  By default the serialization is styled to be human readable using four spaces for indentation.  Passing `false` as the second parameter will disable this styling and enables compact serialization.  For example:

```cpp
JsonObject dog {
    { "Name", "Woofles" },
    { "Age", 4 },
    { "Toys", JsonArray{ "Ball", "Squeeky Bone" } }
};

auto styled_json = ObjectWriter::Write(dog);
// {
//     "Name" : "Woofles",
//     "Age" : 4,
//     "Toys" : ["Ball", "Squeeky Bone"]
// }

auto compact_json = ObjectWriter::Write(dog, false);
// {"Name":"Woofles","Age":4,"Toys":["Ball","Squeeky Bone"]}
```

De-serialization is performed by the static `ObjectReader::Parse()` function that returns a `std::optional<JsonValue>`.  If the de-serialization succeeds then the root `JsonValue` object is returned, otherwise `std::nullopt` is returned instead.  There is no log or exception to identify the point of failure.

```cpp
std::string_view compact_json = ...;
// {"Name":"Woofles","Age":4,"Toys":["Ball","Squeeky Bone"]}

auto value00 = ObjectReader::Parse(compact_json);
if (!value00) {
    // Handle error
}
```

De-serialization can optionally tolerate and ignore C-style comments in the JSON stream, sometimes referred to as "JSON with comments" or `jsonc`.  By default, this feature is disabled and any comments found by the parser will result in a parsing failure.  To enable this feature simply set the `ignore_comments` flag to `true`.

```cpp
std::string_view json = ...;
// {
//     // Woofles is the good boy of the house
//     /* This comment is also allowed even though
//        it spans multiple lines */
//     "Name" : "Woofles",
//     "Age" : 4,
//     "Toys" : ["Ball", "Squeeky Bone"]
// }

auto value00 = ObjectReader::Parse(json);  // Fails as comments are not allowed
auto value00 = ObjectReader::Parse(json, ignore_comments = true);  // Succeeds
```

## "SAX" de-serialization
Internally, the `ObjectReader` class implements the `ISimpleReaderHooks` interface and uses the `SimpleReader::Parse()` function to tokenize the JSON string.  The `SimpleReader` class acts as a "SAX" parser that invokes callbacks for each value it encounters.  If your application wants to filter out unwanted values without allocating memory for them then you should use this approach.

## SSE de-serialization
De-serialization can optionally use [SSE](https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions) CPU features to accelerate the handling of whitespace and strings.  While it is disabled by default to improve portability if your application is intended to run on an `x86` based CPU made after 2000 then it will provide a benefit.

SSE support can be enabled by defining `CPPJSON_USE_SSE` before including the header.
```cpp
#define CPPJSON_USE_SSE
#include "Json.hpp"
```