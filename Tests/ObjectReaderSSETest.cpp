#define CPPJSON_USE_SSE
#include "Json.hpp"

#include "ToString.h"
#include "CppUnitTestFramework.hpp"

using namespace json;

namespace {
    struct ObjectReaderSSETest {};
}

namespace json_test {

    TEST_CASE(ObjectReaderSSETest, Parse_Null) {
        CHECK_EQUAL(ObjectReader::Parse("null"), nullptr);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_Boolean) {
        CHECK_EQUAL(ObjectReader::Parse("true"), true);
        CHECK_EQUAL(ObjectReader::Parse("false"), false);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_Int) {
        CHECK_EQUAL(ObjectReader::Parse("0"), 0u);
        CHECK_EQUAL(ObjectReader::Parse("-9223372036854775808"), std::numeric_limits<int64_t>::min());
        CHECK_EQUAL(ObjectReader::Parse("9223372036854775807"), static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_UInt) {
        CHECK_EQUAL(ObjectReader::Parse("0"), std::numeric_limits<uint64_t>::min());
        CHECK_EQUAL(ObjectReader::Parse("18446744073709551615"), std::numeric_limits<uint64_t>::max());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_Real) {
        CHECK_CLOSE(
            ObjectReader::Parse("2.22507e-308")->AsDouble(),
            std::numeric_limits<double>::min(),
            1e-5
        );
        CHECK_CLOSE(
            ObjectReader::Parse("-1.79769e+308")->AsDouble(),
            std::numeric_limits<double>::lowest(),
            1e-5
        );
        CHECK_CLOSE(
            ObjectReader::Parse("1.79769e+308")->AsDouble(),
            std::numeric_limits<double>::max(),
            1e-5
        );
        CHECK_EQUAL(ObjectReader::Parse("nan"), std::nullopt);
        CHECK_EQUAL(ObjectReader::Parse("inf"), std::nullopt);
        CHECK_EQUAL(ObjectReader::Parse("-inf"), std::nullopt);
        CHECK_EQUAL(ObjectReader::Parse("\"nan\""), "nan");
        CHECK_EQUAL(ObjectReader::Parse("\"inf\""), "inf");
        CHECK_EQUAL(ObjectReader::Parse("\"-inf\""), "-inf");
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_String) {
        CHECK_EQUAL(ObjectReader::Parse("\"\""), "");
        CHECK_EQUAL(ObjectReader::Parse("\"String value\""), "String value");
        CHECK_EQUAL(ObjectReader::Parse(u8"\"ハローワールド\""), u8"ハローワールド");
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_Array) {
        SECTION("Styled") {
            CHECK_EQUAL(
                ObjectReader::Parse("[]")->AsArray(),
                JsonArray{}
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[1234, \"Value\", null]")->AsArray(),
                JsonArray({ 1234u, "Value", nullptr })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[\n    1234,\n    \"Value\",\n    null,\n    true\n]")->AsArray(),
                JsonArray({ 1234u, "Value", nullptr, true })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[\n    \"Outer\",\n    [1234]\n]")->AsArray(),
                JsonArray({ "Outer", JsonArray{ 1234u } })
            );
        }

        SECTION("Compact") {
            CHECK_EQUAL(
                ObjectReader::Parse("[]")->AsArray(),
                JsonArray()
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[1234,\"Value\",null]")->AsArray(),
                JsonArray({1234u, "Value", nullptr })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[1234,\"Value\",null,true]")->AsArray(),
                JsonArray({ 1234u, "Value", nullptr, true })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[\"Outer\",[1234]]")->AsArray(),
                JsonArray({ "Outer", JsonArray{ 1234u } })
            );
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderSSETest, Parse_Object) {
        JsonObject empty;
        JsonObject non_empty{
            { "First", 1234 },
            { "Second", "Value" },
            { "Third", nullptr }
        };

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(empty, true), "{}");
            CHECK_EQUAL(ObjectWriter::Write(non_empty, true), "{\n    \"First\" : 1234,\n    \"Second\" : \"Value\",\n    \"Third\" : null\n}");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(empty, false), "{}");
            CHECK_EQUAL(ObjectWriter::Write(non_empty, false), "{\"First\":1234,\"Second\":\"Value\",\"Third\":null}");
        }
    }

}