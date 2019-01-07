#include "Json.hpp"

#include "CppUnitTestFramework.hpp"
#include "ToString.h"

using namespace json;

namespace {
    struct JsonObjectTest {};
}

namespace json_test {

    TEST_CASE(JsonObjectTest, IsMap) {
        // Check this code compiles correctly.
        JsonObject obj;
        [[maybe_unused]] std::map<std::string, JsonValue, std::less<>>& map = obj;
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonObjectTest, Constructor_Default) {
        JsonObject obj;
        CHECK(obj.empty());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonObjectTest, Constructor_InitializerList) {
        JsonObject empty({});
        CHECK(empty.empty());

        JsonObject non_empty {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        };
        CHECK_EQUAL(non_empty.size(), 3);
        CHECK_EQUAL(non_empty["First"], 1234);
        CHECK_EQUAL(non_empty["Second"], "String value");
        CHECK_EQUAL(non_empty["Third"], nullptr);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonObjectTest, Constructor_Iterators) {
        JsonObject obj {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        };

        JsonObject copy(obj.begin(), obj.end());
        CHECK_EQUAL(copy.size(), 3);
        CHECK_EQUAL(copy["First"], 1234);
        CHECK_EQUAL(copy["Second"], "String value");
        CHECK_EQUAL(copy["Third"], nullptr);

        JsonObject moved(
            std::make_move_iterator(obj.begin()),
            std::make_move_iterator(obj.end())
        );
        CHECK_EQUAL(moved.size(), 3);
        CHECK_EQUAL(moved["First"], 1234);
        CHECK_EQUAL(moved["Second"], "String value");
        CHECK_EQUAL(moved["Third"], nullptr);
        CHECK_EQUAL(obj.size(), 3);
        CHECK_EQUAL(obj["First"], nullptr);
        CHECK_EQUAL(obj["Second"], nullptr);
        CHECK_EQUAL(obj["Third"], nullptr);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonObjectTest, NestedDeclaration) {
        JsonObject outer {
            { "First", 1234 },
            {
                "Second", JsonObject{
                    { "Inner", "Values" }
                }
            }
        };
        CHECK_EQUAL(outer.size(), 2);
        CHECK_EQUAL(outer["First"], 1234);
        CHECK_EQUAL(outer["Second"], JsonValue(JsonObject{ { "Inner", "Values" } }));
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonObjectTest, Exists) {
        JsonObject obj {
            { "First", 1234 },
            { "Second", "String value" }
        };

        CHECK_TRUE(obj.Exists("First"));
        CHECK_TRUE(obj.Exists("First", JsonValueType::Int));
        CHECK_FALSE(obj.Exists("First", JsonValueType::String));

        CHECK_TRUE(obj.Exists("Second"));
        CHECK_TRUE(obj.Exists("Second", JsonValueType::String));
        CHECK_FALSE(obj.Exists("Second", JsonValueType::Int));

        CHECK_FALSE(obj.Exists(""));
        CHECK_FALSE(obj.Exists("Third"));
    }

}