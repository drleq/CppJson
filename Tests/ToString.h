#pragma once

#include "Json.hpp"
#include <optional>

namespace CppUnitTestFramework::Ext {

    std::string ToString(const json::JsonValue& value);
    std::string ToString(const json::JsonArray& value);
    std::string ToString(const json::JsonObject& value);

    //--------------------------------------------------------------------------------------------------------

    std::string ToString(const std::nullopt_t&);

    template <typename T>
    std::string ToString(const std::optional<T>& value) {
        if (!value.has_value()) {
            return "?";
        }
        return ToString(value.value());
    }

}