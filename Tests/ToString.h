#include "Json.hpp"

namespace CppUnitTestFramework::Ext {

    std::string ToString(const json::JsonValue& value);
    std::string ToString(const json::JsonArray& value);
    std::string ToString(const json::JsonObject& value);

}