#pragma once

#include <cmath>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#ifdef CPPJSON_USE_SSE
    #include <emmintrin.h>
#endif

namespace json {

    enum class JsonValueType {
        String,
        Int,
        UInt,
        Real,
        Object,
        Array,
        Boolean,
        Null
    };

    //--------------------------------------------------------------------------------------------------------

    class JsonValue;
    class JsonArray;
    class JsonObject;
    class SimpleReader;
    class ObjectReader;
    class ObjectWriter;

    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------

    class JsonArray :
        public std::vector<JsonValue>
    {
        using base_type = std::vector<JsonValue>;

    public:
        JsonArray() noexcept = default;

        explicit JsonArray(std::initializer_list<JsonValue> values)
          : base_type(values)
        {}

        template <typename Iter>
        JsonArray(Iter first, Iter last)
          : base_type(first, last)
        {}
    };

    //--------------------------------------------------------------------------------------------------------

    class JsonObject :
        public std::map<std::string, JsonValue, std::less<>>
    {
        using base_type = std::map<std::string, JsonValue, std::less<>>;

    public:
        JsonObject() = default;

        explicit JsonObject(std::initializer_list<std::pair<const std::string, JsonValue>> values)
          : base_type(values)
        {}

        template <typename Iter>
        JsonObject(Iter first, Iter last)
          : base_type(first, last)
        {}

        //----------------------------------------------------------------------------------------------------

        inline bool Exists(const std::string_view& name) const;
        inline bool Exists(const std::string_view& name, JsonValueType type) const;
    };
    
    //--------------------------------------------------------------------------------------------------------

    class JsonValue
    {
    public:
        JsonValue() noexcept
          : m_type(JsonValueType::Null)
        {}

        JsonValue(std::nullptr_t) noexcept
          : m_type(JsonValueType::Null)
        {}

        //----------------------------------------------------------------------------------------------------

        JsonValue(std::string value) noexcept
          : m_type(JsonValueType::String)
        {
            new (&m_data.String) std::string(std::move(value));
        }

        JsonValue(const char* value) noexcept
          : JsonValue(std::string(value))
        {}

        JsonValue(const std::string_view& value) noexcept
          : JsonValue(std::string(value))
        {}

        //----------------------------------------------------------------------------------------------------

        JsonValue(int64_t value) noexcept
          : m_type(JsonValueType::Int)
        {
            m_data.Int = value;
        }

        JsonValue(int32_t value) noexcept
          : JsonValue(static_cast<int64_t>(value))
        {}

        //----------------------------------------------------------------------------------------------------

        JsonValue(uint64_t value) noexcept
          : m_type(JsonValueType::UInt)
        {
            m_data.UInt = value;
        }

        JsonValue(uint32_t value) noexcept
          : JsonValue(static_cast<uint64_t>(value))
        {}

        //----------------------------------------------------------------------------------------------------

        JsonValue(double value) noexcept
          : m_type(JsonValueType::Real)
        {
            m_data.Real = value;
        }

        JsonValue(float value) noexcept
          : JsonValue(static_cast<double>(value))
        {}

        //----------------------------------------------------------------------------------------------------

        JsonValue(bool value) noexcept
          : m_type(JsonValueType::Boolean)
        {
            m_data.Bool = value;
        }

        //----------------------------------------------------------------------------------------------------

        JsonValue(JsonArray&& value) noexcept
          : m_type(JsonValueType::Array)
        {
            new (&m_data.Array) JsonArray(std::forward<JsonArray>(value));
        }

        //----------------------------------------------------------------------------------------------------

        JsonValue(JsonObject&& value) noexcept
          : m_type(JsonValueType::Object)
        {
            new (&m_data.Object) JsonObject(std::forward<JsonObject>(value));
        }

        //----------------------------------------------------------------------------------------------------

        JsonValue(JsonValueType type) noexcept
          : m_type(type)
        {
            switch (type) {
            case JsonValueType::Int: m_data.Int = 0; break;
            case JsonValueType::UInt: m_data.UInt = 0; break;
            case JsonValueType::Real: m_data.Real = 0; break;
            case JsonValueType::Boolean: m_data.Bool = false; break;
            case JsonValueType::String: new (&m_data.String) std::string(); break;
            case JsonValueType::Array: new (&m_data.Array) JsonArray(); break;
            case JsonValueType::Object: new (&m_data.Object) JsonObject(); break;

            case JsonValueType::Null:
            default:
                break;
            }
        }

        //----------------------------------------------------------------------------------------------------

        ~JsonValue() noexcept {
            FreeData();
        }

        //----------------------------------------------------------------------------------------------------

        JsonValue(const JsonValue& other) noexcept
          : m_type(JsonValueType::Null)
        {
            *this = other;
        }

        //----------------------------------------------------------------------------------------------------

        JsonValue(JsonValue&& other) noexcept
          : m_type(JsonValueType::Null)
        {
            *this = std::forward<JsonValue>(other);
        }

        //----------------------------------------------------------------------------------------------------
        
        JsonValue& operator = (const JsonValue& other) noexcept {
            FreeData();

            m_type = other.m_type;
            switch (m_type) {
            case JsonValueType::Int: m_data.Int = other.m_data.Int; break;
            case JsonValueType::UInt: m_data.UInt = other.m_data.UInt; break;
            case JsonValueType::Real: m_data.Real = other.m_data.Real; break;
            case JsonValueType::Boolean: m_data.Bool = other.m_data.Bool; break;
            case JsonValueType::String: new (&m_data.String) std::string(other.m_data.String); break;
            case JsonValueType::Array: new (&m_data.Array) JsonArray(other.m_data.Array); break;
            case JsonValueType::Object: new (&m_data.Object) JsonObject(other.m_data.Object); break;
            case JsonValueType::Null: break;
            }
            return *this;
        }

        //----------------------------------------------------------------------------------------------------

        JsonValue& operator = (JsonValue&& other) noexcept {
            FreeData();

            m_type = other.m_type;
            switch (m_type) {
            case JsonValueType::Int: m_data.Int = other.m_data.Int; break;
            case JsonValueType::UInt: m_data.UInt = other.m_data.UInt; break;
            case JsonValueType::Real: m_data.Real = other.m_data.Real; break;
            case JsonValueType::Boolean: m_data.Bool = other.m_data.Bool; break;
            case JsonValueType::String: new (&m_data.String) std::string(std::move(other.m_data.String)); break;
            case JsonValueType::Array: new (&m_data.Array) JsonArray(std::move(other.m_data.Array)); break;
            case JsonValueType::Object: new (&m_data.Object) JsonObject(std::move(other.m_data.Object)); break;
            case JsonValueType::Null: break;
            }
            other.FreeData();
            other.m_type = JsonValueType::Null;

            return *this;
        }

        //----------------------------------------------------------------------------------------------------

        bool operator == (const JsonValue& other) const {
            if (m_type != other.m_type) {
                return false;
            }

            switch (m_type) {
            case JsonValueType::Null: return true;
            case JsonValueType::Int: return m_data.Int == other.m_data.Int;
            case JsonValueType::UInt: return m_data.UInt == other.m_data.UInt;
            case JsonValueType::Real: return m_data.Real == other.m_data.Real;
            case JsonValueType::Boolean: return m_data.Bool == other.m_data.Bool;
            case JsonValueType::String: return m_data.String == other.m_data.String;
            case JsonValueType::Array: return m_data.Array == other.m_data.Array;
            case JsonValueType::Object: return m_data.Object == other.m_data.Object;
            default:
                return false;
            }
        }

        //----------------------------------------------------------------------------------------------------

        bool operator != (const JsonValue& other) const {
            if (m_type != other.m_type) {
                return true;
            }

            switch (m_type) {
            case JsonValueType::Null: return false;
            case JsonValueType::Int: return m_data.Int != other.m_data.Int;
            case JsonValueType::UInt: return m_data.UInt != other.m_data.UInt;
            case JsonValueType::Real: return m_data.Real != other.m_data.Real;
            case JsonValueType::Boolean: return m_data.Bool != other.m_data.Bool;
            case JsonValueType::String: return m_data.String != other.m_data.String;
            case JsonValueType::Array: return m_data.Array != other.m_data.Array;
            case JsonValueType::Object: return m_data.Object != other.m_data.Object;
            default:
                return false;
            }
        }

        //----------------------------------------------------------------------------------------------------

        JsonValueType Type() const { return m_type; }
        bool IsNull() const { return m_type == JsonValueType::Null; }
        bool IsString() const { return m_type == JsonValueType::String; }
        bool IsInt() const { return m_type == JsonValueType::Int; }
        bool IsUInt() const { return m_type == JsonValueType::UInt; }
        bool IsReal() const { return m_type == JsonValueType::Real; }
        bool IsBool() const { return m_type == JsonValueType::Boolean; }
        bool IsArray() const { return m_type == JsonValueType::Array; }
        bool IsObject() const { return m_type == JsonValueType::Object; }

        bool IsInteger() const { return IsInt() || IsUInt(); }

        //----------------------------------------------------------------------------------------------------

        std::string AsString() const {
            switch (m_type) {
            case JsonValueType::String: return m_data.String;
            case JsonValueType::Int: return std::to_string(m_data.Int);
            case JsonValueType::UInt: return std::to_string(m_data.UInt);
            case JsonValueType::Real: return std::to_string(m_data.Real);
            case JsonValueType::Boolean: return m_data.Bool ? std::string("true") : std::string("false");
            case JsonValueType::Null: return std::string("null");

            case JsonValueType::Array:
            case JsonValueType::Object:
                throw std::logic_error("Cannot convert arrays or objects to string");
            }

            throw std::runtime_error("Unknown value type");
        }

        //----------------------------------------------------------------------------------------------------

        std::intmax_t AsInt() const{
            switch (m_type) {
            case JsonValueType::String: return std::stoll(m_data.String);
            case JsonValueType::Int: return m_data.Int;
            case JsonValueType::UInt: return static_cast<std::intmax_t>(m_data.UInt);
            case JsonValueType::Real: return static_cast<std::intmax_t>(m_data.Real);
            case JsonValueType::Boolean: return m_data.Bool ? 1 : 0;

            case JsonValueType::Null:
            case JsonValueType::Array:
            case JsonValueType::Object:
                throw std::logic_error("Cannot convert arrays, objects or null to int");
            }

            throw std::runtime_error("Unknown value type");
        }

        //----------------------------------------------------------------------------------------------------

        std::uintmax_t AsUInt() const {
            switch (m_type) {
            case JsonValueType::String: return std::stoull(m_data.String);
            case JsonValueType::Int: return static_cast<std::uintmax_t>(m_data.Int);
            case JsonValueType::UInt: return m_data.UInt;
            case JsonValueType::Real: return static_cast<std::uintmax_t>(m_data.Real);
            case JsonValueType::Boolean: return m_data.Bool ? 1 : 0;

            case JsonValueType::Null:
            case JsonValueType::Array:
            case JsonValueType::Object:
                throw std::logic_error("Cannot convert arrays, objects or null to int");
            }

            throw std::runtime_error("Unknown value type");
        }

        //----------------------------------------------------------------------------------------------------

        float AsFloat() const {
            switch (m_type) {
            case JsonValueType::String: return std::stof(m_data.String);
            case JsonValueType::Int: return static_cast<float>(m_data.Int);
            case JsonValueType::UInt: return static_cast<float>(m_data.UInt);
            case JsonValueType::Real: return static_cast<float>(m_data.Real);
            case JsonValueType::Boolean: return m_data.Bool ? 1.0f : 0.0f;

            case JsonValueType::Null:
            case JsonValueType::Array:
            case JsonValueType::Object:
                throw std::logic_error("Cannot convert arrays, objects or null to real");
            }

            throw std::runtime_error("Unknown value type");
        }

        //----------------------------------------------------------------------------------------------------

        double AsDouble() const {
            switch (m_type) {
            case JsonValueType::String: return std::stod(m_data.String);
            case JsonValueType::Int: return static_cast<double>(m_data.Int);
            case JsonValueType::UInt: return static_cast<double>(m_data.UInt);
            case JsonValueType::Real: return m_data.Real;
            case JsonValueType::Boolean: return m_data.Bool ? 1.0 : 0.0;

            case JsonValueType::Null:
            case JsonValueType::Array:
            case JsonValueType::Object:
                throw std::logic_error("Cannot convert arrays, objects or null to real");
            }

            throw std::runtime_error("Unknown value type");
        }

        //----------------------------------------------------------------------------------------------------

        bool AsBool() const {
            switch (m_type) {
            case JsonValueType::Int: return (m_data.Int != 0);
            case JsonValueType::UInt: return (m_data.UInt != 0);
            case JsonValueType::Real: return (m_data.Real != 0.0);
            case JsonValueType::Boolean: return m_data.Bool;
            case JsonValueType::String:
                if (m_data.String == "true") { return true; }
                if (m_data.String == "false") { return false; }
                throw std::runtime_error("Failed to convert string to bool");

            case JsonValueType::Null:
            case JsonValueType::Array:
            case JsonValueType::Object:
                throw std::logic_error("Cannot convert arrays, objects, strings or null to bool");
            }

            throw std::runtime_error("Unknown value type");
        }

        //----------------------------------------------------------------------------------------------------

        const JsonArray& AsArray() const { AssertArray(); return m_data.Array; }
              JsonArray& AsArray()       { AssertArray(); return m_data.Array; }

        const JsonObject& AsObject() const { AssertObject(); return m_data.Object; }
              JsonObject& AsObject()       { AssertObject(); return m_data.Object; }

    private:
        JsonValueType m_type;

        union Data {
            std::intmax_t Int;
            std::uintmax_t UInt;
            double Real;
            bool Bool;
			std::string String;
			JsonArray Array;
			JsonObject Object;

			Data() : Int(0) {}
			~Data() {}
		};

		Data m_data;

        void FreeData() {
            switch (m_type) {
            case JsonValueType::String:
                m_data.String.~basic_string();
                break;

            case JsonValueType::Array:
                m_data.Array.~JsonArray();
                break;

            case JsonValueType::Object:
                m_data.Object.~JsonObject();
                break;

            default:
                break;
            }

            m_type = JsonValueType::Null;
        }

        inline void AssertArray() const {
            if (m_type != JsonValueType::Array) {
                throw std::logic_error("Value is not an array");
            }
        }
        inline void AssertObject() const {
            if (m_type != JsonValueType::Object) {
                throw std::logic_error("Value is not an object");
            }
        }

        friend class ObjectWriter;
    };

    //--------------------------------------------------------------------------------------------------------

    inline bool JsonObject::Exists(const std::string_view& name) const {
        return find(name) != end();
    }

    inline bool JsonObject::Exists(const std::string_view& name, JsonValueType type) const {
        auto it = find(name);
        if (it == end()) { return false; }
        return it->second.Type() == type;
    }

    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    
    class ISimpleReaderHooks {
    public:
        virtual ~ISimpleReaderHooks() = default;

        virtual bool OnNull() = 0;
        virtual bool OnBool(bool value) = 0;
        virtual bool OnInt(int64_t value) = 0;
        virtual bool OnUInt(uint64_t value) = 0;
        virtual bool OnReal(double value) = 0;
        virtual bool OnString(std::string_view value) = 0;

        virtual bool OnArrayStart() = 0;
        virtual bool OnArrayEnd() = 0;

        virtual bool OnObjectStart() = 0;
        virtual bool OnObjectKey(std::string_view value) = 0;
        virtual bool OnObjectEnd() = 0;
    };

    //--------------------------------------------------------------------------------------------------------

    class SimpleReader {
    public:
        static bool Parse(std::string_view data, ISimpleReaderHooks*const hooks) {
            State state{ data.data(), data.data() + data.size(), hooks };
            return ParseValue(state);
        }

    private:
        struct State {
            const char* NextChar;
            const char*const End;
            ISimpleReaderHooks*const Hooks;

            bool Eof() const noexcept { return NextChar >= End; }
            size_t BytesRemaining() const noexcept { return std::distance(NextChar, End); }
        };

        //----------------------------------------------------------------------------------------------------

        static bool ParseValue(State& state) {
            SkipWhitespace(state);
            if (state.Eof()) { return false; }

            switch (*state.NextChar) {
            case 't': return ParseTrue(state);
            case 'f': return ParseFalse(state);
            case 'n': return ParseNull(state);
            case '"': return ParseString(state);
            case '[': return ParseArray(state);
            case '{': return ParseObject(state);
            default: return ParseNumber(state);
            }
        }

        //----------------------------------------------------------------------------------------------------

        static void SkipWhitespace(State& state) {
            // Fast check for no whitespace.
            if (state.Eof()) { return; }

#ifdef CPPJSON_USE_SSE
            const __m128i SpaceChar128 = _mm_set1_epi8(' ');
            const __m128i ReturnChar128 = _mm_set1_epi8('\r');
            const __m128i LineFeedChar128 = _mm_set1_epi8('\n');
            const __m128i TabChar128 = _mm_set1_epi8('\t');

            // SSE2 main loop.
            size_t loop_count = state.BytesRemaining() / sizeof(__m128i);
            while (loop_count != 0) {
                __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(state.NextChar));

                __m128i mask = _mm_cmpeq_epi8(chars, SpaceChar128);
                mask = _mm_or_si128(mask, _mm_cmpeq_epi8(chars, ReturnChar128));
                mask = _mm_or_si128(mask, _mm_cmpeq_epi8(chars, LineFeedChar128));
                mask = _mm_or_si128(mask, _mm_cmpeq_epi8(chars, TabChar128));

                uint16_t bitmask = static_cast<uint16_t>(~_mm_movemask_epi8(mask));
                if (bitmask != 0) {
#ifdef __GNUG__
                    int32_t offset = __builtin_ffs(static_cast<int32_t>(bitmask)) - 1;
#endif
#ifdef _MSC_VER
                    unsigned long offset;
                    _BitScanForward(&offset, static_cast<uint32_t>(bitmask));
#endif
                    state.NextChar += offset;
                    return;
                }

                state.NextChar += sizeof(__m128i);
                loop_count--;
            }

            // Plain loop for the <16 remaining chars.
#endif

            while (!state.Eof()) {
                char c = *state.NextChar;
                bool is_space = (c == ' ') || (c == '\r') || (c == '\n') || (c == '\t');
                if (!is_space) { return; }
                ++state.NextChar;               
            }
        }

        //----------------------------------------------------------------------------------------------------

        static void ScanString(State& state) {
            // Fast check for empty string.
            if (*state.NextChar == '"') {
                return;
            }

#ifdef CPPJSON_USE_SSE
            const __m128i QuoteChar128 = _mm_set1_epi8('"');
            const __m128i EscapeChar128 = _mm_set1_epi8('\\');

            // SSE2 main loop.
            size_t loop_count = state.BytesRemaining() / sizeof(__m128i);
            if (loop_count != 0) {
                uint16_t bitmask_escape = 0;
                while (loop_count != 0) {
                    __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(state.NextChar));

                    __m128i mask_quote = _mm_cmpeq_epi8(chars, QuoteChar128);
                    __m128i mask_escape = _mm_cmpeq_epi8(chars, EscapeChar128);
                    uint16_t bitmask_quote = static_cast<uint16_t>(_mm_movemask_epi8(mask_quote));
                    bitmask_escape |= static_cast<uint16_t>(_mm_movemask_epi8(mask_escape)) << 1;

                    uint16_t bitmask_unescaped_quote = bitmask_quote & ~bitmask_escape;
                    if (bitmask_unescaped_quote != 0) {
#ifdef __GNUG__
                        int32_t offset = __builtin_ffs(static_cast<int32_t>(bitmask_unescaped_quote)) - 1;
#endif
#ifdef _MSC_VER
                        unsigned long offset;
                        _BitScanForward(&offset, static_cast<uint32_t>(bitmask_unescaped_quote));
#endif
                        state.NextChar += offset;
                        return;
                    }

                    --loop_count;
                    state.NextChar += sizeof(__m128i);
                    bitmask_escape = static_cast<uint16_t>(_mm_movemask_epi8(mask_escape)) >> (sizeof(__m128i) - 1);
                }
            }

            // Plain loop for the <16 remaining chars.
#endif

            for (; !state.Eof(); ++state.NextChar) {
                if (*state.NextChar == '\\') {
                    ++state.NextChar;
                    continue;
                }
                if (*state.NextChar == '"') {
                    return;
                }
            }
        }

        //----------------------------------------------------------------------------------------------------

        static bool ParseTrue(State& state) {
            if (state.BytesRemaining() < 4) { return false; }

            uint32_t tmp = *reinterpret_cast<const uint32_t*>(state.NextChar + 1);
            tmp &= 0xFFFFFF;
            if (tmp == 0x657572) {
                state.NextChar += 4;
                return state.Hooks->OnBool(true);
            }
            return false;
        }

        static bool ParseFalse(State& state) {
            if (state.BytesRemaining() < 5) { return false; }
            
            uint32_t tmp = *reinterpret_cast<const uint32_t*>(state.NextChar + 1);
            if (tmp == 0x65736C61) {
                state.NextChar += 5;
                return state.Hooks->OnBool(false);
            }
            return false;
        }

        static bool ParseNull(State& state) {
            if (state.BytesRemaining() < 4) { return false; }

            uint32_t tmp = *reinterpret_cast<const uint32_t*>(state.NextChar + 1);
            tmp &= 0xFFFFFF;
            if (tmp == 0x6C6C75) {
                state.NextChar += 4;
                return state.Hooks->OnNull();
            }
            return false;
        }

        //----------------------------------------------------------------------------------------------------

        static bool ParseString(State& state) {
            ++state.NextChar;
            if (state.Eof()) { return false; }

            const char* string_start = state.NextChar;
            ScanString(state);
            if (state.Eof()) { return false; }

            size_t length = std::distance(string_start, state.NextChar);
            ++state.NextChar;
            return state.Hooks->OnString(std::string_view(string_start, length));
        }

        static bool ParseObjectKey(State& state) {
            ++state.NextChar;
            if (state.Eof()) { return false; }

            const char* string_start = state.NextChar;
            ScanString(state);
            if (state.Eof()) { return false; }

            size_t length = std::distance(string_start, state.NextChar);
            ++state.NextChar;
            return state.Hooks->OnObjectKey(std::string_view(string_start, length));
        }

        //----------------------------------------------------------------------------------------------------

        static bool ParseArray(State& state) {
            ++state.NextChar;

            SkipWhitespace(state);
            if (state.Eof()) { return false; }

            if (!state.Hooks->OnArrayStart()) { return false; }

            if (*state.NextChar == ']') {
                ++state.NextChar;
                return state.Hooks->OnArrayEnd();
            }

            for (;;) {
                // Read value.
                if (!ParseValue(state)) { return false; }

                SkipWhitespace(state);
                if (state.Eof()) { return false; }

                if (*state.NextChar == ']') {
                    ++state.NextChar;
                    return state.Hooks->OnArrayEnd();
                }

                // Read separator.
                if (*state.NextChar != ',') { return false; }
                ++state.NextChar;
            }
        }

        //----------------------------------------------------------------------------------------------------

        static bool ParseObject(State& state) {
            ++state.NextChar;

            SkipWhitespace(state);
            if (state.Eof()) { return false; }

            if (!state.Hooks->OnObjectStart()) { return false; }

            if (*state.NextChar == '}') {
                ++state.NextChar;
                return state.Hooks->OnObjectEnd();
            }

            for (;;) {
                // Read key.
                if (!ParseObjectKey(state)) { return false; }

                SkipWhitespace(state);
                if (state.Eof()) { return false; }

                // Read separator.
                if (*state.NextChar != ':') { return false; }
                ++state.NextChar;

                SkipWhitespace(state);
                if (state.Eof()) { return false; }

                // Read value.
                if (!ParseValue(state)) { return false; }

                SkipWhitespace(state);
                if (state.Eof()) { return false; }

                if (*state.NextChar == '}') {
                    ++state.NextChar;
                    return state.Hooks->OnObjectEnd();
                }

                if (*state.NextChar != ',') { return false; }
                ++state.NextChar;

                SkipWhitespace(state);
                if (state.Eof()) { return false; }
            }
        }

        //----------------------------------------------------------------------------------------------------

        static bool ParseNumber(State& state) {
            bool is_negative = *state.NextChar == '-';
            if (is_negative) { ++state.NextChar; }

            if (state.Eof()) { return false; }

            auto start = state.NextChar;
            bool is_double = false;
            while (!state.Eof()) {
                auto c = *state.NextChar;
                if (c >= '0' && c <= '9') {
                    ++state.NextChar;
                } else if (c == '-' || c == '+') {
                    ++state.NextChar;
                } else if (c == '.' || c == 'e' || c == 'E') {
                    is_double = true;
                    ++state.NextChar;
                } else {
                    break;
                }
            }

            auto end = const_cast<char*>(state.NextChar);
            if (is_double) {
                double value = std::strtod(start, &end);
                if (start == end) { return false; }
                if (is_negative) { value = -value; }
                return state.Hooks->OnReal(value);
                
            } else {
                uint64_t value = std::strtoull(start, &end, 10);
                if (start == end) { return false; }
                if (is_negative) {
                    auto limit = static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1;
                    if (value > limit) {
                        // Underflow.
                        return false;
                    }
                    return state.Hooks->OnInt(-static_cast<int64_t>(value));
                } else {
                    return state.Hooks->OnUInt(value);
                }
            }
        }
    };

    //--------------------------------------------------------------------------------------------------------

    class ObjectReader :
        private ISimpleReaderHooks
    {
    public:
        static std::optional<JsonValue> Parse(std::string_view data) {
            ObjectReader parser;
            if (!SimpleReader::Parse(data, &parser)) {
                return std::nullopt;
            }
            
            if (parser.m_stack_top != -1) { return false; }
            return std::make_optional(std::move(parser.m_stack[0].Value));
        }

    private:
        ObjectReader() {
            m_stack.resize(16);
        }

        struct Node {
            JsonValue Value;
            std::string ObjectKey;
        };

        bool m_has_top_value = false;
        int m_stack_top = -1;
        std::vector<Node> m_stack;

        //----------------------------------------------------------------------------------------------------

        Node& AllocateNode() {
            if (m_stack_top == static_cast<int>(m_stack.size())) {
                m_stack.resize(m_stack.size() + 16);
            }
            ++m_stack_top;
            return m_stack[m_stack_top];
        }

        bool SetValue(JsonValue&& value) {
            if (m_stack_top == -1) {
                if (m_has_top_value) {
                    return false;
                }

                m_stack[0].Value = std::forward<JsonValue>(value);
                m_has_top_value = true;
                return true;
            }

            return SetValue(m_stack[m_stack_top], std::forward<JsonValue>(value));
        }

        bool SetValue(Node& parent, JsonValue&& value) {
            auto& parent_value = parent.Value;
            if (parent_value.Type() == JsonValueType::Array) {
                auto& json_array = parent_value.AsArray();
                json_array.emplace_back(std::forward<JsonValue>(value));
                return true;
            }

            if (parent_value.Type() == JsonValueType::Object) {
                auto& json_object = parent_value.AsObject();
                auto& object_key = parent.ObjectKey;
                json_object.emplace(std::move(object_key), std::forward<JsonValue>(value));
                return true;
            }

            return false;
        }

        //----------------------------------------------------------------------------------------------------

        bool OnNull() override {
            return SetValue(JsonValue());
        }
        bool OnBool(bool value) override {
            return SetValue(JsonValue(value));
        }
        bool OnInt(intmax_t value) override {
            return SetValue(JsonValue(value));
        }
        bool OnUInt(uintmax_t value) override {
            return SetValue(JsonValue(value));
        }
        bool OnReal(double value) override {
            return SetValue(JsonValue(value));
        }
        bool OnString(std::string_view value) override {
            return SetValue(JsonValue(value));
        }

        //----------------------------------------------------------------------------------------------------

        bool OnArrayStart() override {
            if (m_stack_top == -1) {
                if (m_has_top_value) {
                    return false;
                }
                m_has_top_value = true;
            }

            auto& node = AllocateNode();
            node.Value = JsonArray();
            node.Value.AsArray().reserve(16);
            return true;
        }
        bool OnArrayEnd() override {
            if (m_stack_top < 0) { return false; }
            if (m_stack_top == 0) {
                --m_stack_top;
                return true;
            }

            auto& array_node = m_stack[m_stack_top];
            auto& parent_node = m_stack[m_stack_top - 1];
            if (!SetValue(parent_node, std::move(array_node.Value))) { return false; }
            --m_stack_top;
            return true;
        }

        //----------------------------------------------------------------------------------------------------

        bool OnObjectStart() override {
            if (m_stack_top == -1) {
                if (m_has_top_value) {
                    return false;
                }
                m_has_top_value = true;
            }

            auto& node = AllocateNode();
            node.Value = JsonObject();
            return true;
        }
        bool OnObjectKey(std::string_view value) override {
            auto& object_node = m_stack[m_stack_top];
            object_node.ObjectKey = value;
            return true;
        }
        bool OnObjectEnd() override {
            if (m_stack_top < 0) { return false; }
            if (m_stack_top == 0) {
                --m_stack_top;
                return true;
            }

            auto& object_node = m_stack[m_stack_top];
            auto& parent_node = m_stack[m_stack_top - 1];
            if (!SetValue(parent_node, std::move(object_node.Value))) { return false; }
            --m_stack_top;
            return true;
        }
    };

    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------

    class ObjectWriter {
    public:
        static std::string Write(const JsonValue& root, bool styled = true) {
            std::string buf;
            buf.reserve(1024);
            Write(buf, root, styled);
            buf.shrink_to_fit();
            return buf;
        }

        //----------------------------------------------------------------------------------------------------

        static std::string Write(const JsonArray& root, bool styled = true) {
            std::string buf;
            buf.reserve(1024);
            if (styled) {
                WriteArrayStyled(buf, root, 0);
            } else {
                WriteArrayCompact(buf, root);
            }
            buf.shrink_to_fit();
            return buf;
        }

        //----------------------------------------------------------------------------------------------------

        static std::string Write(const JsonObject& root, bool styled = true) {
            std::string buf;
            buf.reserve(1024);
            if (styled) {
                WriteObjectStyled(buf, root, 0);
            } else {
                WriteObjectCompact(buf, root);
            }
            buf.shrink_to_fit();
            return buf;
        }

        //----------------------------------------------------------------------------------------------------

        static void Write(
            std::string& buf,
            const JsonValue& root,
            bool styled = true,
            size_t indentation = 0
        ) {
            switch (root.Type()) {
            case JsonValueType::String:  WriteString(buf, root.m_data.String); break;
            case JsonValueType::Int:     WriteInt(buf, root.m_data.Int); break;
            case JsonValueType::UInt:    WriteUInt(buf, root.m_data.UInt); break;
            case JsonValueType::Real:    WriteDouble(buf, root.m_data.Real); break;

            case JsonValueType::Array:
                if (styled) {
                    WriteArrayStyled(buf, root.m_data.Array, indentation);
                } else {
                    WriteArrayCompact(buf, root.m_data.Array);
                }
                break;

            case JsonValueType::Object:
                if (styled) {
                    WriteObjectStyled(buf, root.m_data.Object, indentation);
                } else {
                    WriteObjectCompact(buf, root.m_data.Object);
                }
                break;

            case JsonValueType::Boolean:
                if (root.m_data.Bool) {
                    buf.append("true", 4);
                } else {
                    buf.append("false", 5);
                }
                break;

            case JsonValueType::Null:
                buf.append("null", 4);
                break;
            }
        }

    private:
        static void WriteString(std::string& buf, const std::string& value) {
            buf += '"';

            auto pos = value.find_first_of("\"/\\\b\f\n\r\t", 0, 8);
            if (pos == std::string::npos) {
                buf += value;
            } else {
                for (char c : value) {
                    switch (c) {
                    case '"':  buf.append("\\\"", 2); break;
                    case '/':  buf.append("\\/", 2); break;
                    case '\b': buf.append("\\b", 2); break;
                    case '\f': buf.append("\\f", 2); break;
                    case '\n': buf.append("\\n", 2); break;
                    case '\r': buf.append("\\r", 2); break;
                    case '\t': buf.append("\\t", 2); break;
                    case '\\': buf.append("\\\\", 2); break;
                    default:   buf += c; break;
                    }
                }
            }

            buf += '"';
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteUInt(std::string& buf, uintmax_t value) {
            char tmp[32];
            char* tmp_ptr = std::end(tmp);

            do {
                *--tmp_ptr = '0' + static_cast<char>(value % 10);
                value /= 10;
            } while (value != 0);

            buf.append(tmp_ptr, std::end(tmp));
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteInt(std::string& buf, intmax_t value) {
            if (value < 0) {
                buf += '-';
                value = -value;
            }

            WriteUInt(buf, static_cast<uintmax_t>(value));
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteDouble(std::string& buf, double value) {
            if (std::isnan(value)) {
                buf.append("\"nan\"", 5);

            } else if (std::isinf(value)) {
                if (value >= 0) {
                    buf.append("\"inf\"", 5);
                } else {
                    buf.append("\"-inf\"", 6);
                }

            } else {
                char tmp[32];
                std::snprintf(tmp, 32, "%g", value);
                buf.append(tmp);
            }
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteArrayCompact(std::string& buf, const JsonArray& value) {
            buf += '[';
            
            size_t last_index = value.size() - 1;
            size_t i = 0;
            for (auto& sub_value : value) {
                Write(buf, sub_value, false);
                if (i++ != last_index) {
                    buf += ',';
                }
            }
            
            buf += ']';
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteObjectCompact(std::string& buf, const JsonObject& value) {
            buf += '{';
            
            size_t last_index = value.size() - 1;
            size_t i = 0;
            for (auto& kvp : value) {
                WriteString(buf, kvp.first);
                buf += ':';
                Write(buf, kvp.second, false);
                if (i++ != last_index) {
                    buf += ',';
                }
            }
            
            buf += '}';
        }

        //----------------------------------------------------------------------------------------------------

        static bool IsMultilineArray(const JsonArray& value) {
            if (value.size() > 3) { return true; }

            for (auto& sub_value : value) {
                if (sub_value.Type() == JsonValueType::Array && !sub_value.AsArray().empty()) {
                    return true;
                }
                if (sub_value.Type() == JsonValueType::Object && !sub_value.AsObject().empty()) {
                    return true;
                }
            }

            return false;
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteArrayStyled(std::string& buf, const JsonArray& value, size_t indentation) {
            if (value.empty()) {
                buf.append("[]", 2);
                return;
            }

            size_t last_index = value.size() - 1;
            size_t i = 0;

            if (IsMultilineArray(value)) {
                buf.append("[\n", 2);

                for (auto& sub_value : value) {
                    buf.append(indentation + 4, ' ');
                    Write(buf, sub_value, true, indentation + 4);

                    if (i++ != last_index) {
                        buf.append(",\n", 2);
                    } else {
                        buf.append(1, '\n');
                    }
                }

                buf.append(indentation, ' ');
                buf += ']';
            } else {
                buf += '[';

                for (auto& sub_value : value) {
                    Write(buf, sub_value, true, indentation + 4);
                    if (i++ != last_index) {
                        buf.append(", ", 2);
                    }
                }

                buf += ']';
            }
        }

        //----------------------------------------------------------------------------------------------------

        static void WriteObjectStyled(std::string& buf, const JsonObject& value, size_t indentation) {
            if (value.empty()) {
                buf.append("{}", 2);
                return;
            }

            buf.append("{\n", 2);

            size_t last_index = value.size() - 1;
            size_t i = 0;
            for (auto kvp : value) {
                buf.append(indentation + 4, ' ');
                WriteString(buf, kvp.first);
                buf.append(" : ", 3);
                Write(buf, kvp.second, true, indentation + 4);
                if (i++ != last_index) {
                    buf.append(",\n", 2);
                } else {
                    buf.append(1, '\n');
                }
            }

            buf.append(indentation, ' ');
            buf += '}';
        }
    };

}