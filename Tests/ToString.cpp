#include "ToString.h"

#include <algorithm>
#include <sstream>

using namespace json;

namespace CppUnitTestFramework::Ext {

    std::string ToString(const JsonValue& value) {
        switch(value.Type()) {
        case JsonValueType::Null: return "nullptr";
        case JsonValueType::Boolean: return ToString(value.AsBool());
        case JsonValueType::Int: return ToString(value.AsInt());
        case JsonValueType::UInt: return ToString(value.AsUInt());
        case JsonValueType::Real: return ToString(value.AsDouble());
        case JsonValueType::String: return value.AsString();
        case JsonValueType::Array: return ToString(value.AsArray());
        case JsonValueType::Object: return ToString(value.AsObject());
        default:
            return "<invalid>";
        }
    }

    //--------------------------------------------------------------------------------------------------------

    std::string ToString(const JsonArray& value) {
        std::ostringstream ss;
        ss << "JsonArray[";

        size_t count = std::min<size_t>(value.size(), 3);
        for (size_t i = 0; i < count; ++i) {
            if (i != 0) {
                ss << ", ";
            }
            ss << ToString(value[i]);
        }
        if (count < value.size()) {
            ss << ", ...";
        }

        ss << ']';
        return ss.str();
    }

    //--------------------------------------------------------------------------------------------------------

    std::string ToString(const JsonObject& value) {
        std::ostringstream ss;
        ss << "JsonObject{";

        size_t count = std::min<size_t>(value.size(), 3);
        size_t i = 0;
        for (auto& kvp : value) {
            if (i >= count) {
                break;
            }

            if (i != 0) {
                ss << ", ";
            }
            ss << kvp.first << " : " << ToString(kvp.second);
        }
        if (count < value.size()) {
            ss << ", ...";
        }

        ss << '}';
        return ss.str();
    }

    //--------------------------------------------------------------------------------------------------------

    std::string ToString(std::nullopt_t) {
        return "?";
    }

}