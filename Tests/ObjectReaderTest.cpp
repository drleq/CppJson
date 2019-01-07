#include "Json.hpp"

#include "CppUnitTestFramework.hpp"
#include "ToString.h"

using namespace json;

namespace {
    struct ObjectReaderTest {};
}

namespace json_test {

    TEST_CASE(ObjectReaderTest, Parse_Null) {
        CHECK_EQUAL(ObjectReader::Parse("null"), nullptr);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Parse_Boolean) {
        CHECK_EQUAL(ObjectReader::Parse("true"), true);
        CHECK_EQUAL(ObjectReader::Parse("false"), false);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Parse_Int) {
        CHECK_EQUAL(ObjectReader::Parse("0"), 0);
        CHECK_EQUAL(ObjectReader::Parse("-9223372036854775808"), std::numeric_limits<int64_t>::min());
        CHECK_EQUAL(ObjectReader::Parse("9223372036854775807"), std::numeric_limits<int64_t>::max());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Parse_UInt) {
        CHECK_EQUAL(ObjectReader::Parse("0"), std::numeric_limits<uint64_t>::min());
        CHECK_EQUAL(ObjectReader::Parse("18446744073709551615"), std::numeric_limits<uint64_t>::max());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Parse_Real) {
        CHECK_EQUAL(ObjectReader::Parse("2.22507e-308"), std::numeric_limits<double>::min());
        CHECK_EQUAL(ObjectReader::Parse("-1.79769e+308"), std::numeric_limits<double>::lowest());
        CHECK_EQUAL(ObjectReader::Parse("1.79769e+308"), std::numeric_limits<double>::max());
        CHECK_EQUAL(ObjectReader::Parse("nan"), std::nullopt);
        CHECK_EQUAL(ObjectReader::Parse("inf"), std::nullopt);
        CHECK_EQUAL(ObjectReader::Parse("-inf"), std::nullopt);
        CHECK_EQUAL(ObjectReader::Parse("\"nan\""), "nan");
        CHECK_EQUAL(ObjectReader::Parse("\"inf\""), "inf");
        CHECK_EQUAL(ObjectReader::Parse("\"-inf\""), "-inf");
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Parse_String) {
        CHECK_EQUAL(ObjectReader::Parse("\"\""), "");
        CHECK_EQUAL(ObjectReader::Parse("\"String value\""), "String value");
        CHECK_EQUAL(ObjectReader::Parse(u8"\"ハローワールド\""), u8"ハローワールド");
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Parse_Array) {
        SECTION("Styled") {
            CHECK_EQUAL(
                ObjectReader::Parse("[]")->AsArray(),
                JsonArray{}
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[1234, \"Value\", null]")->AsArray(),
                JsonArray({1234, "Value", nullptr })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[\n    1234,\n    \"Value\",\n    null,\n    true\n]")->AsArray(),
                JsonArray({ 1234, "Value", nullptr, true })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[\n    \"Outer\",\n    [1234]\n]")->AsArray(),
                JsonArray({ "Outer", JsonArray{ 1234 } })
            );
        }

        SECTION("Compact") {
            CHECK_EQUAL(
                ObjectReader::Parse("[]")->AsArray(),
                JsonArray()
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[1234,\"Value\",null]")->AsArray(),
                JsonArray({1234, "Value", nullptr })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[1234,\"Value\",null,true]")->AsArray(),
                JsonArray({ 1234, "Value", nullptr, true })
            );
            CHECK_EQUAL(
                ObjectReader::Parse("[\"Outer\",[1234]]")->AsArray(),
                JsonArray({ "Outer", JsonArray{ 1234 } })
            );
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectReaderTest, Write_Object) {
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