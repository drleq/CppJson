#include "Json.hpp"

#include "CppUnitTestFramework.hpp"

namespace CppUnitTestFramework::Ext {

    std::string ToString(const json::JsonValue& value);
    std::string ToString(const json::JsonArray& value);
    std::string ToString(const json::JsonObject& value);

    //--------------------------------------------------------------------------------------------------------

    std::string ToString(std::nullopt_t);

    template <typename T>
    std::string ToString(const std::optional<T>& value) {
        if (!value.has_value()) {
            return ToString(std::nullopt);
        }
        return ToString(value.value());
    }

}