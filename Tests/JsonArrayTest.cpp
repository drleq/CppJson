#include "Json.hpp"

#include "ToString.h"
#include "CppUnitTestFramework.hpp"

using namespace json;

namespace {
    struct JsonArrayTest {};
}

namespace json_test {

    TEST_CASE(JsonArrayTest, IsVector) {
        // Check this code compiles correctly.
        JsonArray arr;
        [[maybe_unused]] std::vector<JsonValue>& vec = arr;
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonArrayTest, Constructor_Default) {
        JsonArray arr;
        CHECK(arr.empty());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonArrayTest, Constructor_InitializerList) {
        JsonArray empty({});
        CHECK(empty.empty());

        JsonArray non_empty { 1234, "String value", nullptr };
        CHECK_EQUAL(non_empty.size(), 3u);
        CHECK_EQUAL(non_empty[0], 1234);
        CHECK_EQUAL(non_empty[1], "String value");
        CHECK_EQUAL(non_empty[2], nullptr);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonArrayTest, Constructor_Iterators) {
        JsonArray arr{ 1234, "String value", nullptr };

        JsonArray copy(arr.begin(), arr.end());
        CHECK_EQUAL(copy.size(), 3u);
        CHECK_EQUAL(copy[0], 1234);
        CHECK_EQUAL(copy[1], "String value");
        CHECK_EQUAL(copy[2], nullptr);

        JsonArray moved(
            std::make_move_iterator(arr.begin()),
            std::make_move_iterator(arr.end())
        );
        CHECK_EQUAL(moved.size(), 3u);
        CHECK_EQUAL(moved[0], 1234);
        CHECK_EQUAL(moved[1], "String value");
        CHECK_EQUAL(moved[2], nullptr);
        CHECK_EQUAL(arr.size(), 3u);
        CHECK_EQUAL(arr[0], nullptr);
        CHECK_EQUAL(arr[1], nullptr);
        CHECK_EQUAL(arr[2], nullptr);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonArrayTest, NestedDeclaration) {
        JsonArray outer {
            1234,
            JsonArray{ "Inner", "Values" }
        };
        CHECK_EQUAL(outer.size(), 2u);
        CHECK_EQUAL(outer[0], 1234);
        CHECK_EQUAL(outer[1], JsonValue(JsonArray{ "Inner", "Values" }));
    }

}