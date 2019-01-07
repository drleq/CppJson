#include "Json.hpp"

#include "CppUnitTestFramework.hpp"
#include "ToString.h"

using namespace json;

namespace {
    struct ObjectWriterTest {};
}

namespace json_test {

    TEST_CASE(ObjectWriterTest, Write_Empty) {
        JsonValue empty;

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(empty, true), "null");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(empty, false), "null");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_Null) {
        JsonValue null(nullptr);

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(null, true), "null");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(null, false), "null");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_Boolean) {
        JsonValue value_true(true);
        JsonValue value_false(false);

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(value_true, true), "true");
            CHECK_EQUAL(ObjectWriter::Write(value_false, true), "false");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(value_true, false), "true");
            CHECK_EQUAL(ObjectWriter::Write(value_false, false), "false");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_Int) {
        JsonValue value_zero(0);
        JsonValue value_min(std::numeric_limits<int64_t>::min());
        JsonValue value_max(std::numeric_limits<int64_t>::max());

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(value_zero, true), "0");
            CHECK_EQUAL(ObjectWriter::Write(value_min, true), "-9223372036854775808");
            CHECK_EQUAL(ObjectWriter::Write(value_max, true), "9223372036854775807");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(value_zero, false), "0");
            CHECK_EQUAL(ObjectWriter::Write(value_min, false), "-9223372036854775808");
            CHECK_EQUAL(ObjectWriter::Write(value_max, false), "9223372036854775807");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_UInt) {
        JsonValue value_min(std::numeric_limits<uint64_t>::min());
        JsonValue value_max(std::numeric_limits<uint64_t>::max());

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(value_min, true), "0");
            CHECK_EQUAL(ObjectWriter::Write(value_max, true), "18446744073709551615");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(value_min, false), "0");
            CHECK_EQUAL(ObjectWriter::Write(value_max, false), "18446744073709551615");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_Real) {
        JsonValue value_small(std::numeric_limits<double>::min());
        JsonValue value_min(std::numeric_limits<double>::lowest());
        JsonValue value_max(std::numeric_limits<double>::max());
        JsonValue value_nan(std::numeric_limits<double>::quiet_NaN());
        JsonValue value_inf(std::numeric_limits<double>::infinity());
        JsonValue value_neg_inf(-std::numeric_limits<double>::infinity());

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(value_small, true), "2.22507e-308");
            CHECK_EQUAL(ObjectWriter::Write(value_min, true), "-1.79769e+308");
            CHECK_EQUAL(ObjectWriter::Write(value_max, true), "1.79769e+308");
            CHECK_EQUAL(ObjectWriter::Write(value_nan, true), "\"nan\"");
            CHECK_EQUAL(ObjectWriter::Write(value_inf, true), "\"inf\"");
            CHECK_EQUAL(ObjectWriter::Write(value_neg_inf, true), "\"-inf\"");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(value_small, false), "2.22507e-308");
            CHECK_EQUAL(ObjectWriter::Write(value_min, false), "-1.79769e+308");
            CHECK_EQUAL(ObjectWriter::Write(value_max, false), "1.79769e+308");
            CHECK_EQUAL(ObjectWriter::Write(value_nan, false), "\"nan\"");
            CHECK_EQUAL(ObjectWriter::Write(value_inf, false), "\"inf\"");
            CHECK_EQUAL(ObjectWriter::Write(value_neg_inf, false), "\"-inf\"");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_String) {
        JsonValue empty("");
        JsonValue non_empty("String value");
        JsonValue fancy(u8"ハローワールド");

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(empty, true), "\"\"");
            CHECK_EQUAL(ObjectWriter::Write(non_empty, true), "\"String value\"");
            CHECK_EQUAL(ObjectWriter::Write(fancy, true), u8"\"ハローワールド\"");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(empty, false), "\"\"");
            CHECK_EQUAL(ObjectWriter::Write(non_empty, false), "\"String value\"");
            CHECK_EQUAL(ObjectWriter::Write(fancy, false), u8"\"ハローワールド\"");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_Array) {
        JsonArray empty;
        JsonArray three_element({ 1234, "Value", nullptr });
        JsonArray four_element({ 1234, "Value", nullptr, true });
        JsonArray nested{
            "Outer",
            JsonArray{ 1234 }
        };

        SECTION("Styled") {
            CHECK_EQUAL(ObjectWriter::Write(empty, true), "[]");
            CHECK_EQUAL(ObjectWriter::Write(three_element, true), "[1234, \"Value\", null]");
            CHECK_EQUAL(ObjectWriter::Write(four_element, true), "[\n    1234,\n    \"Value\",\n    null,\n    true\n]");
            CHECK_EQUAL(ObjectWriter::Write(nested, true), "[\n    \"Outer\",\n    [1234]\n]");
        }
        SECTION("Compact") {
            CHECK_EQUAL(ObjectWriter::Write(empty, false), "[]");
            CHECK_EQUAL(ObjectWriter::Write(three_element, false), "[1234,\"Value\",null]");
            CHECK_EQUAL(ObjectWriter::Write(four_element, false), "[1234,\"Value\",null,true]");
            CHECK_EQUAL(ObjectWriter::Write(nested, false), "[\"Outer\",[1234]]");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(ObjectWriterTest, Write_Object) {
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