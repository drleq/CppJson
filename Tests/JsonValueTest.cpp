#include "Json.hpp"

#include "CppUnitTestFramework.hpp"
#include "ToString.h"
#include <cmath>

using namespace json;

namespace {
    struct JsonValueTest {};
}

namespace json_test {

    TEST_CASE(JsonValueTest, Constructor_Default) {
        JsonValue value;
        CHECK(value.IsNull());
        CHECK_EQUAL(value.Type(), JsonValueType::Null);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Nullptr) {
        JsonValue value(nullptr);
        CHECK(value.IsNull());
        CHECK_EQUAL(value.Type(), JsonValueType::Null);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_String) {
        SECTION("Using std::string") {
            JsonValue value(std::string("String value"));
            CHECK(value.IsString());
            CHECK_EQUAL(value.Type(), JsonValueType::String);
            CHECK_EQUAL(value.AsString(), "String value");
        }

        SECTION("Using const char*") {
            JsonValue value("String value");
            CHECK(value.IsString());
            CHECK_EQUAL(value.Type(), JsonValueType::String);
            CHECK_EQUAL(value.AsString(), "String value");
        }

        SECTION("Using std::string_view") {
            JsonValue value(std::string_view("String value"));
            CHECK(value.IsString());
            CHECK_EQUAL(value.Type(), JsonValueType::String);
            CHECK_EQUAL(value.AsString(), "String value");
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Int) {
        SECTION("Using int32_t") {
            JsonValue zero(0);
            CHECK(zero.IsInt());
            CHECK(zero.IsInteger());
            CHECK_EQUAL(zero.Type(), JsonValueType::Int);
            CHECK_EQUAL(zero.AsInt(), 0);

            JsonValue min(std::numeric_limits<int32_t>::min());
            CHECK(min.IsInt());
            CHECK(min.IsInteger());
            CHECK_EQUAL(min.Type(), JsonValueType::Int);
            CHECK_EQUAL(min.AsInt(), std::numeric_limits<int32_t>::min());

            JsonValue max(std::numeric_limits<int32_t>::max());
            CHECK(max.IsInt());
            CHECK(max.IsInteger());
            CHECK_EQUAL(max.Type(), JsonValueType::Int);
            CHECK_EQUAL(max.AsInt(), std::numeric_limits<int32_t>::max());
        }

        SECTION("Using int64_t") {
            JsonValue zero(0ll);
            CHECK(zero.IsInt());
            CHECK(zero.IsInteger());
            CHECK_EQUAL(zero.Type(), JsonValueType::Int);
            CHECK_EQUAL(zero.AsInt(), 0ll);

            JsonValue min(std::numeric_limits<int64_t>::min());
            CHECK(min.IsInt());
            CHECK(min.IsInteger());
            CHECK_EQUAL(min.Type(), JsonValueType::Int);
            CHECK_EQUAL(min.AsInt(), std::numeric_limits<int64_t>::min());

            JsonValue max(std::numeric_limits<int64_t>::max());
            CHECK(max.IsInt());
            CHECK(max.IsInteger());
            CHECK_EQUAL(max.Type(), JsonValueType::Int);
            CHECK_EQUAL(max.AsInt(), std::numeric_limits<int64_t>::max());
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_UInt) {
        SECTION("Using uint32_t") {
            JsonValue min(std::numeric_limits<uint32_t>::min());
            CHECK(min.IsUInt());
            CHECK(min.IsInteger());
            CHECK_EQUAL(min.Type(), JsonValueType::UInt);
            CHECK_EQUAL(min.AsUInt(), std::numeric_limits<uint32_t>::min());

            JsonValue max(std::numeric_limits<uint32_t>::max());
            CHECK(max.IsUInt());
            CHECK(max.IsInteger());
            CHECK_EQUAL(max.Type(), JsonValueType::UInt);
            CHECK_EQUAL(max.AsUInt(), std::numeric_limits<uint32_t>::max());
        }

        SECTION("Using uint64_t") {
            JsonValue min(std::numeric_limits<uint64_t>::min());
            CHECK(min.IsUInt());
            CHECK(min.IsInteger());
            CHECK_EQUAL(min.Type(), JsonValueType::UInt);
            CHECK_EQUAL(min.AsUInt(), std::numeric_limits<uint64_t>::min());

            JsonValue max(std::numeric_limits<uint64_t>::max());
            CHECK(max.IsUInt());
            CHECK(max.IsInteger());
            CHECK_EQUAL(max.Type(), JsonValueType::UInt);
            CHECK_EQUAL(max.AsUInt(), std::numeric_limits<uint64_t>::max());
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Real) {
        SECTION("Using double") {
            JsonValue zero(0.0);
            CHECK(zero.IsReal());
            CHECK_FALSE(zero.IsInteger());
            CHECK_EQUAL(zero.Type(), JsonValueType::Real);
            CHECK_EQUAL(zero.AsDouble(), 0.0);
            CHECK_EQUAL(zero.AsFloat(), 0.0f);

            JsonValue min(std::numeric_limits<double>::min());
            CHECK(min.IsReal());
            CHECK_FALSE(min.IsInteger());
            CHECK_EQUAL(min.Type(), JsonValueType::Real);
            CHECK_EQUAL(min.AsDouble(), std::numeric_limits<double>::min());

            JsonValue max(std::numeric_limits<double>::max());
            CHECK(max.IsReal());
            CHECK_FALSE(max.IsInteger());
            CHECK_EQUAL(max.Type(), JsonValueType::Real);
            CHECK_EQUAL(max.AsDouble(), std::numeric_limits<double>::max());

            JsonValue nan(std::numeric_limits<double>::quiet_NaN());
            CHECK(nan.IsReal());
            CHECK_FALSE(nan.IsInteger());
            CHECK_EQUAL(nan.Type(), JsonValueType::Real);
            CHECK(std::isnan(nan.AsDouble()));
        }

        SECTION("Using float") {
            JsonValue zero(0.0f);
            CHECK(zero.IsReal());
            CHECK_FALSE(zero.IsInteger());
            CHECK_EQUAL(zero.Type(), JsonValueType::Real);
            CHECK_EQUAL(zero.AsDouble(), 0.0);
            CHECK_EQUAL(zero.AsFloat(), 0.0f);

            JsonValue min(std::numeric_limits<float>::min());
            CHECK(min.IsReal());
            CHECK_FALSE(min.IsInteger());
            CHECK_EQUAL(min.Type(), JsonValueType::Real);
            CHECK_EQUAL(min.AsFloat(), std::numeric_limits<float>::min());

            JsonValue max(std::numeric_limits<float>::max());
            CHECK(max.IsReal());
            CHECK_FALSE(max.IsInteger());
            CHECK_EQUAL(max.Type(), JsonValueType::Real);
            CHECK_EQUAL(max.AsFloat(), std::numeric_limits<float>::max());

            JsonValue nan(std::numeric_limits<float>::quiet_NaN());
            CHECK(nan.IsReal());
            CHECK_FALSE(nan.IsInteger());
            CHECK_EQUAL(nan.Type(), JsonValueType::Real);
            CHECK(std::isnan(nan.AsFloat()));
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Boolean) {
        JsonValue value_true(true);
        CHECK(value_true.IsBool());
        CHECK_EQUAL(value_true.Type(), JsonValueType::Boolean);
        CHECK_EQUAL(value_true.AsBool(), true);

        JsonValue value_false(false);
        CHECK(value_false.IsBool());
        CHECK_EQUAL(value_false.Type(), JsonValueType::Boolean);
        CHECK_EQUAL(value_false.AsBool(), false);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Array) {
        JsonValue empty(JsonArray {});
        CHECK(empty.IsArray());
        CHECK_EQUAL(empty.Type(), JsonValueType::Array);
        CHECK_EQUAL(empty.AsArray(), JsonArray{});

        JsonValue non_empty(JsonArray{ 1234, "String value", nullptr });
        CHECK(non_empty.IsArray());
        CHECK_EQUAL(non_empty.Type(), JsonValueType::Array);
        CHECK_EQUAL(non_empty.AsArray(), JsonArray({ 1234, "String value", nullptr }));
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Object) {
        JsonValue empty(JsonObject {});
        CHECK(empty.IsObject());
        CHECK_EQUAL(empty.Type(), JsonValueType::Object);
        CHECK_EQUAL(empty.AsObject(), JsonObject{});

        JsonValue non_empty(JsonObject {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        });
        CHECK(non_empty.IsObject());
        CHECK_EQUAL(non_empty.Type(), JsonValueType::Object);
        CHECK_EQUAL(
            non_empty.AsObject(),
            JsonObject({
                { "First", 1234 },
                { "Second", "String value" },
                { "Third", nullptr }
            })
        );
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Type) {
        JsonValue value_null(JsonValueType::Null);
        CHECK(value_null.IsNull());
        CHECK_EQUAL(value_null.Type(), JsonValueType::Null);

        JsonValue value_int(JsonValueType::Int);
        CHECK(value_int.IsInt());
        CHECK(value_int.IsInteger());
        CHECK_EQUAL(value_int.Type(), JsonValueType::Int);
        CHECK_EQUAL(value_int.AsInt(), 0);

        JsonValue value_uint(JsonValueType::UInt);
        CHECK(value_uint.IsUInt());
        CHECK(value_uint.IsInteger());
        CHECK_EQUAL(value_uint.Type(), JsonValueType::UInt);
        CHECK_EQUAL(value_uint.AsUInt(), 0);

        JsonValue value_real(JsonValueType::Real);
        CHECK(value_real.IsReal());
        CHECK_FALSE(value_real.IsInteger());
        CHECK_EQUAL(value_real.Type(), JsonValueType::Real);
        CHECK_EQUAL(value_real.AsDouble(), 0.0);
        CHECK_EQUAL(value_real.AsFloat(), 0.0f);

        JsonValue value_bool(JsonValueType::Boolean);
        CHECK(value_bool.IsBool());
        CHECK_EQUAL(value_bool.Type(), JsonValueType::Boolean);
        CHECK_EQUAL(value_bool.AsBool(), false);

        JsonValue value_string(JsonValueType::String);
        CHECK(value_string.IsString());
        CHECK_EQUAL(value_string.Type(), JsonValueType::String);
        CHECK_EQUAL(value_string.AsString(), "");

        JsonValue value_array(JsonValueType::Array);
        CHECK(value_array.IsArray());
        CHECK_EQUAL(value_array.Type(), JsonValueType::Array);
        CHECK_EQUAL(value_array.AsArray(), JsonArray{});

        JsonValue value_object(JsonValueType::Object);
        CHECK(value_object.IsObject());
        CHECK_EQUAL(value_object.Type(), JsonValueType::Object);
        CHECK_EQUAL(value_object.AsObject(), JsonObject{});
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Copy) {
        JsonValue value(JsonObject {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        });

        JsonValue copy(value);
        CHECK_EQUAL(copy.Type(), JsonValueType::Object);
        CHECK_EQUAL(copy.AsObject(), value.AsObject());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Constructor_Move) {
        JsonValue value(JsonObject {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        });

        JsonValue moved(std::move(value));
        CHECK_EQUAL(moved.Type(), JsonValueType::Object);
        CHECK_EQUAL(
            moved.AsObject(),
            JsonObject({
                { "First", 1234 },
                { "Second", "String value" },
                { "Third", nullptr }
            })
        );
        CHECK_EQUAL(value.Type(), JsonValueType::Null);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Assignment_Copy) {
        JsonValue value(JsonObject {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        });

        JsonValue copy;
        copy = value;
        CHECK_EQUAL(copy.Type(), JsonValueType::Object);
        CHECK_EQUAL(copy.AsObject(), value.AsObject());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Assignment_Move) {
        JsonValue value(JsonObject {
            { "First", 1234 },
            { "Second", "String value" },
            { "Third", nullptr }
        });

        JsonValue moved;
        moved = std::move(value);
        CHECK_EQUAL(moved.Type(), JsonValueType::Object);
        CHECK_EQUAL(
            moved.AsObject(),
            JsonObject({
                { "First", 1234 },
                { "Second", "String value" },
                { "Third", nullptr }
            })
        );
        CHECK_EQUAL(value.Type(), JsonValueType::Null);
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, Equality) {
        std::vector<JsonValue> values = {
            nullptr,
            1234,
            -1234,
            1234.0,
            true,
            "String value",
            JsonArray{ 1234, "Second", nullptr },
            JsonObject{ 
                { "First", 1234 },
                { "Second", "String value" },
                { "Third", nullptr }
            }
        };

        for (size_t i = 0; i < values.size(); ++i) {
            for (size_t j = 0; j < values.size(); ++j) {
                bool same_object = (i == j);

                if (same_object) {
                    CHECK_TRUE(values[i] == values[j]);
                    CHECK_FALSE(values[i] != values[j]);
                } else {
                    CHECK_FALSE(values[i] == values[j]);
                    CHECK_TRUE(values[i] != values[j]);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsString) {
        CHECK_EQUAL(JsonValue("String value").AsString(), "String value");
        CHECK_EQUAL(JsonValue().AsString(), "null");
        CHECK_EQUAL(JsonValue(nullptr).AsString(), "null");
        CHECK_EQUAL(JsonValue(-1234).AsString(), "-1234");
        CHECK_EQUAL(JsonValue(1234u).AsString(), "1234");
        CHECK_EQUAL(JsonValue(1234.0).AsString(), "1234.000000");
        CHECK_EQUAL(JsonValue(1234.0f).AsString(), "1234.000000");
        CHECK_EQUAL(JsonValue(true).AsString(), "true");
        CHECK_EQUAL(JsonValue(false).AsString(), "false");

        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsString());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsString());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsInt) {
        CHECK_EQUAL(JsonValue(-1234).AsInt(), -1234);
        CHECK_EQUAL(JsonValue(1234u).AsInt(), 1234);
        CHECK_EQUAL(JsonValue(-1234.5).AsInt(), -1234);
        CHECK_EQUAL(JsonValue(-1234.5f).AsInt(), -1234);
        CHECK_EQUAL(JsonValue(true).AsInt(), 1);
        CHECK_EQUAL(JsonValue(false).AsInt(), 0);
        CHECK_EQUAL(JsonValue("-1234").AsInt(), -1234);

        CHECK_THROW(std::logic_error, JsonValue().AsInt());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsInt());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsInt());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsInt());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsUInt) {
        CHECK_EQUAL(JsonValue(1234u).AsUInt(), 1234);
        CHECK_EQUAL(JsonValue(1234).AsUInt(), 1234);
        CHECK_EQUAL(JsonValue(1234.5).AsUInt(), 1234);
        CHECK_EQUAL(JsonValue(1234.5f).AsUInt(), 1234);
        CHECK_EQUAL(JsonValue(true).AsUInt(), 1);
        CHECK_EQUAL(JsonValue(false).AsUInt(), 0);
        CHECK_EQUAL(JsonValue("1234").AsUInt(), 1234);

        CHECK_THROW(std::logic_error, JsonValue().AsUInt());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsUInt());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsUInt());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsUInt());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsDouble) {
        CHECK_EQUAL(JsonValue(-1234.5).AsDouble(), -1234.5);
        CHECK_EQUAL(JsonValue(-1234.5f).AsDouble(), -1234.5);
        CHECK_EQUAL(JsonValue(-1234).AsDouble(), -1234.0);
        CHECK_EQUAL(JsonValue(1234u).AsDouble(), 1234.0);
        CHECK_EQUAL(JsonValue(true).AsDouble(), 1.0);
        CHECK_EQUAL(JsonValue(false).AsDouble(), 0.0);
        CHECK_EQUAL(JsonValue("-1234.5").AsDouble(), -1234.5);

        CHECK_THROW(std::logic_error, JsonValue().AsDouble());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsDouble());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsDouble());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsDouble());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsFloat) {
        CHECK_EQUAL(JsonValue(-1234.5f).AsFloat(), -1234.5f);
        CHECK_EQUAL(JsonValue(-1234.5).AsFloat(), -1234.5f);
        CHECK_EQUAL(JsonValue(-1234).AsFloat(), -1234.0f);
        CHECK_EQUAL(JsonValue(1234u).AsFloat(), 1234.0f);
        CHECK_EQUAL(JsonValue(true).AsFloat(), 1.0f);
        CHECK_EQUAL(JsonValue(false).AsFloat(), 0.0f);
        CHECK_EQUAL(JsonValue("-1234.5").AsFloat(), -1234.5f);

        CHECK_THROW(std::logic_error, JsonValue().AsFloat());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsFloat());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsFloat());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsFloat());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsBool) {
        CHECK_EQUAL(JsonValue(true).AsBool(), true);
        CHECK_EQUAL(JsonValue(false).AsBool(), false);

        CHECK_EQUAL(JsonValue(-1).AsBool(), true);
        CHECK_EQUAL(JsonValue(0).AsBool(), false);
        CHECK_EQUAL(JsonValue(1u).AsBool(), true);
        CHECK_EQUAL(JsonValue(0u).AsBool(), false);

        CHECK_EQUAL(JsonValue(-1.0).AsBool(), true);
        CHECK_EQUAL(JsonValue(0.0).AsBool(), false);
        CHECK_EQUAL(JsonValue(-1.0f).AsBool(), true);
        CHECK_EQUAL(JsonValue(0.0f).AsBool(), false);

        CHECK_EQUAL(JsonValue("true").AsBool(), true);
        CHECK_EQUAL(JsonValue("false").AsBool(), false);
        CHECK_THROW(std::runtime_error, JsonValue("Different").AsBool());

        CHECK_THROW(std::logic_error, JsonValue().AsBool());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsBool());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsBool());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsBool());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsArray) {
        CHECK_EQUAL(
            JsonValue(JsonValueType::Array).AsArray(),
            JsonArray()
        );
        CHECK_EQUAL(
            JsonValue(JsonArray({ 1234, "Second", nullptr })).AsArray(),
            JsonArray({ 1234, "Second", nullptr })
        );

        CHECK_THROW(std::logic_error, JsonValue().AsArray());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsArray());
        CHECK_THROW(std::logic_error, JsonValue(1).AsArray());
        CHECK_THROW(std::logic_error, JsonValue(1u).AsArray());
        CHECK_THROW(std::logic_error, JsonValue(1.0).AsArray());
        CHECK_THROW(std::logic_error, JsonValue(1.0f).AsArray());
        CHECK_THROW(std::logic_error, JsonValue(true).AsArray());
        CHECK_THROW(std::logic_error, JsonValue(false).AsArray());
        CHECK_THROW(std::logic_error, JsonValue("String value").AsArray());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Object).AsArray());
    }

    //--------------------------------------------------------------------------------------------------------

    TEST_CASE(JsonValueTest, AsObject) {
        CHECK_EQUAL(
            JsonValue(JsonValueType::Object).AsObject(),
            JsonObject()
        );
        CHECK_EQUAL(
            JsonValue(JsonObject({
                { "First", 1234 },
                { "Second", "String value" },
                { "Third", nullptr }
            })).AsObject(),
            JsonObject({
                { "First", 1234 },
                { "Second", "String value" },
                { "Third", nullptr }
            })
        );

        CHECK_THROW(std::logic_error, JsonValue().AsObject());
        CHECK_THROW(std::logic_error, JsonValue(nullptr).AsObject());
        CHECK_THROW(std::logic_error, JsonValue(1).AsObject());
        CHECK_THROW(std::logic_error, JsonValue(1u).AsObject());
        CHECK_THROW(std::logic_error, JsonValue(1.0).AsObject());
        CHECK_THROW(std::logic_error, JsonValue(1.0f).AsObject());
        CHECK_THROW(std::logic_error, JsonValue(true).AsObject());
        CHECK_THROW(std::logic_error, JsonValue(false).AsObject());
        CHECK_THROW(std::logic_error, JsonValue("String value").AsObject());
        CHECK_THROW(std::logic_error, JsonValue(JsonValueType::Array).AsObject());
    }

}