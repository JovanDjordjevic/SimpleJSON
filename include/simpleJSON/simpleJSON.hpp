#ifndef SIMPLE_JSON_HPP
#define SIMPLE_JSON_HPP

#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace internal {
    template <typename T, typename U>
    struct is_not_same : std::true_type {};

    template <typename T>
    struct is_not_same<T, T> : std::false_type {};

    template<typename T>
    using remove_cvref_t = typename std::remove_cv_t<typename std::remove_reference_t<T>>;

    template <typename T, typename U>
    inline constexpr bool is_same_or_constructible = std::disjunction_v<
        typename std::is_same<T, U>::type,
        typename std::is_same<T, remove_cvref_t<U>>::type,
        typename std::is_constructible<T, U>::type
    >;
} // namespace internal

// ============================================================================================================================================
// =================================================================== API ====================================================================
// ============================================================================================================================================

/// @brief Namespace containing all relevant classes and functions
namespace simpleJSON {
    class JSONString;
    class JSONNumber;
    class JSONBool;
    class JSONNull;
    class JSONArray;
    class JSONMap;
    class JSONObject;

    /// @brief Type used to represent floating point numbers.
    /// @details If you wish to change the underlying type, don't forget to also change how it is parsed (see internal::strToJSONFloating) 
    using JSONFloating = long double;
    /// @brief Type used to represent both positive and negative whole numbers.
    /// @details If you wish to change the underlying type, don't forget to also change how it is parsed (see internal::strToJSONInteger) 
    using JSONInteger = long long int;

    /// @brief Default string used for indentation
    /// @details This string will be appended once per indent level.
    static const char* defaultIndentString = "\t";

    /// @brief Function to parse a json file
    /// @details Throws JSONException if anything goes wrong
    /// @return A JSONObject representing data contained in the file
    inline JSONObject parseFromFile(const std::filesystem::path& fileName);

    /// @brief Function to parse a string containing json data
    /// @details Throws JSONException if anything goes wrong
    /// @return A JSONObject representing data contained in the string
    inline JSONObject parseFromString(const std::string& jsonString);

    /// @brief Convenience function to represent a json object as a non-formatted string
    /// @details Throws JSONException if anything goes wrong
    /// @return String representation of json object
    inline std::string dumpToString(const JSONObject& obj);

    /// @brief Convenience function to represent a json object as an indented string
    /// @details Throws JSONException if anything goes wrong
    /// @param[in] obj Object to turn into a string
    /// @param[in] indentString What indentation to use (defaults to one tab per indent level)
    /// @return String representation of json object
    inline std::string dumpToPrettyString(const JSONObject& obj, const std::string& indentString = defaultIndentString);

    /// @brief Class to represent various exceptions thrown by library functions
    class JSONException : public std::runtime_error {
        public:
            JSONException(const std::string& msg);
            /// @brief Get message contained within the exception
            [[nodiscard]] const char* what() const noexcept override;
    };

    /// @brief Class to represent json strings
    /// @details Internally holds a std::string
    class JSONString {
        public:
            /// @brief Default constructor creates an empty string
            JSONString();

            /// @brief JSONString can be constructed from every type that std::string can be constructed from
            template <typename T, typename = std::enable_if_t<std::is_constructible_v<std::string, T>>>
            JSONString(const T& val);

            friend bool operator==(const JSONString& lhs, const JSONString& rhs);
            friend bool operator!=(const JSONString& lhs, const JSONString& rhs);
            friend bool operator<(const JSONString& lhs, const JSONString& rhs);
            friend bool operator<=(const JSONString& lhs, const JSONString& rhs);
            friend bool operator>(const JSONString& lhs, const JSONString& rhs);
            friend bool operator>=(const JSONString& lhs, const JSONString& rhs);

            /// @brief Get a copy of the underlying std::string
            [[nodiscard]] std::string getString() const;

            /// @brief Get string representation as specified in the json standard
            [[nodiscard]] std::string toString() const;

            /// @brief Get indented string representation string as specified in the json standard
            [[nodiscard]] std::string toIndentedString(const std::string&, const std::string&) const;

        private:
            std::string mValue;
    };

    /// @brief Class to represent json numbers
    /// @details Internally, holds either a floating or integer value
    ///          represented by JSONFloating or JSONInteger respectively
    class JSONNumber {
        public:
            /// @brief Default constructor creates integer with value 0
            constexpr JSONNumber();

            /// @brief JSONNumber can be constructed from any arithmetic type except bool
            template <typename T, typename = std::enable_if_t<std::conjunction_v<
                typename internal::is_not_same<T, bool>::type,
                typename std::is_arithmetic<T>::type>>>
            constexpr JSONNumber(const T v);

            friend bool constexpr operator==(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool constexpr operator!=(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool constexpr operator<(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool constexpr operator<=(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool constexpr operator>(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool constexpr operator>=(const JSONNumber& lhs, const JSONNumber& rhs);

            /// @brief Get a copy of the underlying number
            template <typename T, typename = std::enable_if_t<std::disjunction_v<
                typename std::is_same<T, JSONInteger>::type,
                typename std::is_same<T, JSONFloating>::type>>>
            [[nodiscard]] constexpr auto getValue() const;

            /// @brief Get string representation as specified in the json standard
            [[nodiscard]] std::string toString() const;

            /// @brief Get indented string representation string as specified in the json standard
            [[nodiscard]] std::string toIndentedString(const std::string&, const std::string&) const;

        private:
            template <typename T>
            constexpr auto resolveJsonNumberInitialValue(const T val) const;

        private:
            std::variant<JSONFloating, JSONInteger> mValue;
    };

    /// @brief Class to represent json booleans
    /// @details Internally, holds a bool
    class JSONBool {
        public:
            /// @brief Default constructor creates boolean that holds false
            constexpr JSONBool();

            template <typename T, typename = std::enable_if_t<std::is_same_v<T, bool>>>
            constexpr JSONBool(const T val);

            friend bool constexpr operator==(const JSONBool& lhs, const JSONBool& rhs);
            friend bool constexpr operator!=(const JSONBool& lhs, const JSONBool& rhs);

            /// @brief Get a copy of the underlying boolean
            [[nodiscard]] bool getBoolean() const;

            /// @brief Get string representation as specified in the json standard
            [[nodiscard]] std::string toString() const;

            /// @brief Get indented string representation string as specified in the json standard
            [[nodiscard]] std::string toIndentedString(const std::string&, const std::string&) const;

        private:
            bool mValue;
    };

    /// @brief Class to represent json nulls
    /// @details Internally, this is just an empty object
    class JSONNull {
        public:
            constexpr JSONNull();
            constexpr JSONNull(const std::nullptr_t);

            friend bool constexpr operator==(const JSONNull&, const JSONNull&);
            friend bool constexpr operator!=(const JSONNull&, const JSONNull&);

            /// @brief Get string representation as specified in the json standard
            [[nodiscard]] std::string toString() const;

            /// @brief Get indented string representation string as specified in the json standard
            [[nodiscard]] std::string toIndentedString(const std::string&, const std::string&) const;
    };

    /// @brief Class to represent json arrays
    /// @details Internally, holds a std::vector of JSONObject-s
    class JSONArray {
        public:
            /// @brief Default constructor creates an empty array
            JSONArray();

            /// @brief Array can be contructed from an initializer list
            JSONArray(const std::initializer_list<JSONObject> list);

            /// @brief Append anything that can be deduced as JSONObject to this array
            template <typename T>
            void append(T&& arg);

            /// @brief Remove element from the back of the array
            void pop();

            /// @brief Get number of elements in the array
            [[nodiscard]] size_t size() const;

            /// @brief Remove all elements from the array
            void clear();

            /// @brief Access the element at specified index
            /// @details Throws JSONException if index is greater than size
            JSONObject& operator[](const size_t index);

            /// @brief Constant acces to element at specified index
            /// @details Throws JSONException if index is greater than size
            const JSONObject& operator[](const size_t index) const;

            friend bool operator==(const JSONArray& lhs, const JSONArray& rhs);
            friend bool operator!=(const JSONArray& lhs, const JSONArray& rhs);

            /// @brief Get string representation as specified in the json standard
            [[nodiscard]] std::string toString() const;

            /// @brief Get indented string representation string as specified in the json standard
            [[nodiscard]] std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

        private:
            std::vector<JSONObject> mValue;
    };

    /// @brief Class to represent maps of key-value pairs
    /// @details Internally, holds a std::map<JSONString, JSONObject>
    class JSONMap {
        public:
            /// @brief Default constructor creates an map
            JSONMap();

            /// @brief Array can be contructed from an initializer list
            JSONMap(const std::initializer_list<std::pair<const JSONString, JSONObject>> list);

            /// @brief Get number of elements in the map
            [[nodiscard]] size_t size() const;

            /// @brief Remove all elements from the map
            void clear();

            /// @brief Removes the item with the specified key
            void removeField(const JSONString& key);

            /// @brief Access map item with specified key
            JSONObject& operator[](const JSONString& key);

            /// @brief Constant acces map item with specified key
            const JSONObject& operator[](const JSONString& key) const;

            friend bool operator==(const JSONMap& lhs, const JSONMap& rhs);
            friend bool operator!=(const JSONMap& lhs, const JSONMap& rhs);

            /// @brief Get string representation as specified in the json standard
            [[nodiscard]] std::string toString() const;

            /// @brief Get indented string representation string as specified in the json standard
            [[nodiscard]] std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

        private:
            std::map<JSONString, JSONObject> mValue;
    };

    /// @brief Class to represent json objects
    /// @details This is the main class the user should interact with
    ///          Internally it holds one of the json standard types:
    ///          string, number, boolean, null, array or map of key-value pairs.
    ///          Because JSONObject may contain different types with different methods
    ///          all those methods must be implemented in this class as well
    class JSONObject {
        public:
            /// @brief Default constructor creates an empty map
            JSONObject();

            template <typename T>
            JSONObject(const T& val);

            /// @brief Append anything that can be deduced as JSONObject to array contained in this object 
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            template <typename T>
            void append(T&& arg);

            /// @brief Remove element from the back of the array contained in this object
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            void pop();

            /// @brief Remove specific entry from map conained in this object
            /// @details Only valid if this JSONObject holds a map, otherwise throws a JSONException
            void removeField(const JSONString& key);

            /// @brief Get number of elements in the array or map contained in this object
            /// @details Only valid if this JSONObject holds an array or map, otherwise throws a JSONException
            [[nodiscard]] size_t size() const;

            /// @brief Remove all elements from the array or map contained in this object
            /// @details Only valid if this JSONObject holds an array or map, otherwise throws a JSONException
            void clear();

            /// @brief Access the element at specified index in array conained in this object
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            JSONObject& operator[](const size_t index);

            /// @brief Constant access the element at specified index in array conained in this object
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            const JSONObject& operator[](const size_t index) const;

            /// @brief Access the value at specified key in map conained in this object
            /// @details Only valid if this JSONObject holds a map, otherwise throws a JSONException
            JSONObject& operator[](const JSONString& key);

            /// @brief Constant access the value at specified key in map conained in this object
            /// @details Only valid if this JSONObject holds a map, otherwise throws a JSONException
            const JSONObject& operator[](const JSONString& key) const;

            friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator<(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator<=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator>(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator>=(const JSONObject& lhs, const JSONObject& rhs);

            /// @brief Convert to string as specified in the json standard
            /// @details Format of string depends on the type contained within this object
            [[nodiscard]] std::string toString() const;

            /// @brief Convert to indented string as specified in the json standard
            /// @details Format of string depends on the type contained within this object
            [[nodiscard]] std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

        private:
            template <typename T>
            constexpr auto resolveJsonObjectInitialValue(const T& val) const;

        private:
            std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONMap> mValue;
    };
}   // namespace simpleJSON 

// ============================================================================================================================================
// ============================================================== IMPLEMENTATION ==============================================================
// ============================================================================================================================================

// namespace for helper functions, not indended for end-users
namespace internal {
    enum class NextJsonType {
        JSON_STRING,
        JSON_NUMBER,
        JSON_BOOL,
        JSON_NULL,
        JSON_ARRAY,
        JSON_OBJECT,
        JSON_END_OF_STREAM,
        JSON_ERROR
    };

    inline simpleJSON::JSONObject beginParseFromStream(std::istream& stream);

    inline NextJsonType detectNextType(const char nextCharInStream);

    inline constexpr bool isValidEscapedCharacter(const char c);
    inline constexpr bool isValidWhitespace(const char c);
    inline char peekNextNonSpaceCharacter(std::istream& stream);

    inline simpleJSON::JSONFloating strToJSONFloating(const std::string& str);
    inline simpleJSON::JSONInteger strToJSONInteger(const std::string& str);

    inline simpleJSON::JSONString parseString(std::istream& stream);
    inline simpleJSON::JSONNumber parseNumber(std::istream& stream);
    inline simpleJSON::JSONBool parseBool(std::istream& stream);
    inline simpleJSON::JSONNull parseNull(std::istream& stream);
    inline simpleJSON::JSONArray parseArray(std::istream& stream);
    inline simpleJSON::JSONObject parseObject(std::istream& stream);
} // namespace internal

namespace simpleJSON {
    inline JSONObject parseFromFile(const std::filesystem::path& fileName) {
        std::ifstream stream(fileName);
        return internal::beginParseFromStream(stream);
    }

    inline JSONObject parseFromString(const std::string& jsonString) {
        std::stringstream stream(jsonString);
        return internal::beginParseFromStream(stream);
    }

    // void dumpToFile(const char* fileName);

    inline std::string dumpToString(const JSONObject& obj) {
        return obj.toString();
    }

    inline std::string dumpToPrettyString(const JSONObject& obj, const std::string& indentString) {
        std::string currentIndentation = "";
        return obj.toIndentedString(currentIndentation, indentString);
    }

    // JSONexception

    inline JSONException::JSONException(const std::string& msg) : std::runtime_error(msg) {}

    inline const char* JSONException::what() const noexcept {
        return std::runtime_error::what();
    }

    // JSONString

    inline JSONString::JSONString() : mValue{std::string{}} {}

    template <typename T, typename>
    inline JSONString::JSONString(const T& val) : mValue{val} {}

    inline bool operator==(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.mValue == rhs.mValue;
    }

    inline bool operator!=(const JSONString& lhs, const JSONString& rhs) { 
        return !(lhs == rhs);
    }

    inline bool operator<(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.mValue < rhs.mValue;
    }

    inline bool operator<=(const JSONString& lhs, const JSONString& rhs) { 
        return (lhs < rhs) || (lhs == rhs);
    }

    inline bool operator>(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.mValue > rhs.mValue;
    }

    inline bool operator>=(const JSONString& lhs, const JSONString& rhs) { 
        return (lhs > rhs) || (lhs == rhs);
    }

    inline std::string JSONString::getString() const {
        return mValue;
    }

    inline std::string JSONString::toString() const {
        return "\"" + mValue + "\"";
    }

    inline std::string JSONString::toIndentedString(const std::string&, const std::string&) const {
        return toString();
    }

    // JSONNumber

    inline constexpr JSONNumber::JSONNumber() : mValue{JSONInteger{0}} {}

    template <typename T, typename>
    constexpr JSONNumber::JSONNumber(const T v) : mValue{resolveJsonNumberInitialValue(v)} {}

    inline bool constexpr operator==(const JSONNumber& lhs, const JSONNumber& rhs) {
        return lhs.mValue == rhs.mValue;
    }

    inline bool constexpr operator!=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return !(lhs == rhs);
    }

    inline bool constexpr operator<(const JSONNumber& lhs, const JSONNumber& rhs) {
        // FIXME: find a better way to implement this method!!!
        // std::variant::operator< and operator > do not behave the way we need
        const auto& lhsValue = lhs.mValue;
        const auto& rhsValue = rhs.mValue;

        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) < std::get<JSONFloating>(rhsValue);
        }

        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONInteger>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) < static_cast<JSONFloating>(std::get<JSONInteger>(rhsValue));
        }

        if (std::holds_alternative<JSONInteger>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return static_cast<JSONFloating>(std::get<JSONInteger>(lhsValue)) < std::get<JSONFloating>(rhsValue);
        }

        if (std::holds_alternative<JSONInteger>(lhsValue) && std::holds_alternative<JSONInteger>(rhsValue)) { 
            return std::get<JSONInteger>(lhsValue) < std::get<JSONInteger>(rhsValue);
        }

        // should not get here
        throw JSONException("Unexpected comparison on JSONNumber");
    }

    inline bool constexpr operator<=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    inline bool constexpr operator>(const JSONNumber& lhs, const JSONNumber& rhs) {
        return !(lhs <= rhs);
    }

    inline bool constexpr operator>=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }

    template <typename T, typename>
    inline constexpr auto JSONNumber::getValue() const {
        return std::get<T>(mValue);
    }

    inline std::string JSONNumber::toString() const {
        return std::visit(
            [&](const auto& arg) {
                return std::to_string(arg);
            }, mValue);
    }

    inline std::string JSONNumber::toIndentedString(const std::string&, const std::string&) const {
        return toString();
    }

    template <typename T>
    inline constexpr auto JSONNumber::resolveJsonNumberInitialValue(const T val) const {
        if constexpr (std::is_floating_point_v<T>) {
            return JSONFloating(val);
        }
        else if constexpr (std::is_integral_v<T>) {
            return JSONInteger(val);
        }
        else {
            throw simpleJSON::JSONException("JSONNumber must be created with floating point or integer value");
        }
    }

    // JSONBool

    inline constexpr JSONBool::JSONBool() : mValue{false} {}

    template <typename T, typename>
    inline constexpr JSONBool::JSONBool(const T val) : mValue{val} {}

    inline bool constexpr operator==(const JSONBool& lhs, const JSONBool& rhs) {
        return lhs.mValue == rhs.mValue;
    }

    inline bool constexpr operator!=(const JSONBool& lhs, const JSONBool& rhs) {
        return !(lhs == rhs);
    }

    inline bool JSONBool::getBoolean() const {
        return mValue;
    }

    inline std::string JSONBool::toString() const {
        return mValue ? std::string("true") : std::string("false");
    }

    inline std::string JSONBool::toIndentedString(const std::string&, const std::string&) const {
        return toString();
    }

    // JSONNull

    inline constexpr JSONNull::JSONNull() = default;
    
    inline constexpr JSONNull::JSONNull(const std::nullptr_t) {}

    inline bool constexpr operator==(const JSONNull&, const JSONNull&) {
        return true;
    }

    inline bool constexpr operator!=(const JSONNull&, const JSONNull&) {
        return false;
    }

    inline std::string JSONNull::toString() const {
        return std::string("null");
    }

    inline std::string JSONNull::toIndentedString(const std::string&, const std::string&) const {
        return toString();
    }

    // JSONArray

    inline JSONArray::JSONArray() : mValue(std::vector<JSONObject>{}) {}

    inline JSONArray::JSONArray(const std::initializer_list<JSONObject> list) : mValue(list) {}

    template <typename T>
    inline void JSONArray::append(T&& arg) {
        mValue.emplace_back(std::forward<T>(arg));
        return;
    }

    inline void JSONArray::pop() {
        mValue.pop_back();
        return;
    }

    inline size_t JSONArray::size() const {
        return mValue.size();
    }
    
    inline void JSONArray::clear() {
        mValue.clear();
        return;
    }

    inline JSONObject& JSONArray::operator[](const size_t index) {
        if (index >= mValue.size()) {
            throw JSONException("JSONArray operator[] index out of range");
        }

        return mValue[index];
    }

    inline const JSONObject& JSONArray::operator[](const size_t index) const {
        if (index >= mValue.size()) {
            throw JSONException("JSONArray operator[] index out of range");
        }

        return mValue.at(index);
    }

    inline bool operator==(const JSONArray& lhs, const JSONArray& rhs) {
        return lhs.mValue == rhs.mValue;
    }

    inline bool operator!=(const JSONArray& lhs, const JSONArray& rhs) {
        return !(lhs == rhs);
    }

    inline std::string JSONArray::toString() const {
        if (size() == 0) {
            return "[]";
        }

        std::string res = "[";

        for(const auto& elem : mValue) {
            res += elem.toString() + ",";
        }

        res.back() = ']';

        return res;
    }

    inline std::string JSONArray::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        if (size() == 0) {
            return "[]";
        }

        std::string res = "[\n";

        currentIndentation += indentString;

        for (const auto& elem : mValue) {
            res += currentIndentation + elem.toIndentedString(currentIndentation, indentString) + ",\n";
        }

        currentIndentation.erase(currentIndentation.length() - indentString.length());

        res.erase(res.length() - 2);
        res += "\n" + currentIndentation + "]";

        return res;
    }

    // JSONMap

    inline JSONMap::JSONMap()
        : mValue{}
    {}

    inline JSONMap::JSONMap(const std::initializer_list<std::pair<const JSONString, JSONObject>> list)
        : mValue{list}
    {}

    inline size_t JSONMap::size() const {
        return mValue.size();
    }

    inline void JSONMap::clear() {
        mValue.clear();
    }

    inline void JSONMap::removeField(const JSONString& key){
        auto it = mValue.find(key);
        if (it != std::end(mValue)) {
            mValue.erase(it);
        }
    }

    inline JSONObject& JSONMap::operator[](const JSONString& key) {
        return mValue[key];
    }

    inline const JSONObject& JSONMap::operator[](const JSONString& key) const {
        return mValue.at(key);
    }

    inline bool operator==(const JSONMap& lhs, const JSONMap& rhs) {
        return lhs.mValue == rhs.mValue;
    }

    inline bool operator!=(const JSONMap& lhs, const JSONMap& rhs) {
        return lhs.mValue != rhs.mValue;
    }

    inline std::string JSONMap::toString() const {
        if (mValue.empty()) {
            return "{}";
        }
        std::string res = "{";

        for (const auto& [key, val] : mValue) {
            res += key.toString() + ":" + val.toString() + ",";
        }

        res.back() = '}';

        return res;
    }

    inline std::string JSONMap::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        if (mValue.empty()) {
            return "{}";
        }
  
        std::string res = "{\n";

        currentIndentation += indentString;

        for (const auto& [key, val] : mValue) {
            res += currentIndentation + key.toString() + " : " + val.toIndentedString(currentIndentation, indentString) + ",\n";
        }

        currentIndentation.erase(currentIndentation.length() - indentString.length());

        res.erase(res.length() - 2);
        res += "\n" + currentIndentation + "}";
 
        return res;
    }

    // JSONObject

    inline JSONObject::JSONObject() : mValue(JSONMap{}) {}

    template <typename T>
    inline JSONObject::JSONObject(const T& val) : mValue{resolveJsonObjectInitialValue(val)} {}

    template <typename T>
    inline void JSONObject::append(T&& arg) {
        if (std::holds_alternative<JSONArray>(mValue)) {
            auto& arr = std::get<JSONArray>(mValue);
            arr.append(std::forward<T>(arg));
            return;
        }

        throw JSONException("Cannot append. Current object is not an array");
    }

    inline void JSONObject::pop() {
        if (std::holds_alternative<JSONArray>(mValue)) {
            auto& arr = std::get<JSONArray>(mValue);
            arr.pop();
            return;
        }

        throw JSONException("Cannot pop. Current JSONObject does not hold an array");
    }

    inline void JSONObject::removeField(const JSONString& key) {
        if (std::holds_alternative<JSONMap>(mValue)) {
            auto& map = std::get<JSONMap>(mValue);
            map.removeField(key);
            return;
        }

        throw JSONException("Removing field failed, this JSONObject is not a map");      
    }

    inline size_t JSONObject::size() const {
        if (std::holds_alternative<JSONArray>(mValue)) {
            const auto& arr = std::get<JSONArray>(mValue);
            return arr.size();
        }

        if (std::holds_alternative<JSONMap>(mValue)) {
            const auto& map = std::get<JSONMap>(mValue);
            return map.size();
        }

        throw JSONException("Current JSONObject is not an array or map, cannot call size()");
    }
    
    inline void JSONObject::clear() {
        if (std::holds_alternative<JSONArray>(mValue)) {
            auto& arr = std::get<JSONArray>(mValue);
            return arr.clear();
        }

        if (std::holds_alternative<JSONMap>(mValue)) {
            auto& map = std::get<JSONMap>(mValue);
            return map.clear();
        }

        throw JSONException("Current JSONObject is not an array or map, cannot call clear()");
    }

    inline JSONObject& JSONObject::operator[](const size_t index) {
        if (std::holds_alternative<JSONArray>(mValue)) {
            auto& arr = std::get<JSONArray>(mValue);
            return arr[index];
        }

        throw JSONException("Operator[] failed, this JSONObject is not an array");
    }

    inline const JSONObject& JSONObject::operator[](const size_t index) const {
        if (std::holds_alternative<JSONArray>(mValue)) {
            const auto& arr = std::get<JSONArray>(mValue);
            return arr[index];
        }

        throw JSONException("Operator[] failed, this JSONObject is not an array");
    }

    inline JSONObject& JSONObject::operator[](const JSONString& key) {
        if (std::holds_alternative<JSONMap>(mValue)) {
            auto& map = std::get<JSONMap>(mValue);
            return map[key];
        }

        throw JSONException("Operator[] failed, this JSONObject is not a map");
    }

    inline const JSONObject& JSONObject::operator[](const JSONString& key) const {
        if (std::holds_alternative<JSONMap>(mValue)) {
            const auto& map = std::get<JSONMap>(mValue);
            return map[key];
        }

        throw JSONException("Operator[] failed, this JSONObject is not a map");
    }   

    inline bool operator==(const JSONObject& lhs, const JSONObject& rhs) {
        return lhs.mValue == rhs.mValue;
    }

    inline bool operator!=(const JSONObject& lhs, const JSONObject& rhs) {
        return !(lhs == rhs);
    }

    inline bool operator<(const JSONObject& lhs, const JSONObject& rhs) {
        const auto& lhsValue = lhs.mValue;
        const auto& rhsValue = rhs.mValue;

        if (std::holds_alternative<JSONString>(lhsValue) && std::holds_alternative<JSONString>(rhsValue)) {
            return std::get<JSONString>(lhsValue) < std::get<JSONString>(rhsValue);
        }

        if (std::holds_alternative<JSONNumber>(lhsValue) && std::holds_alternative<JSONNumber>(rhsValue)) {
            return std::get<JSONNumber>(lhsValue) < std::get<JSONNumber>(rhsValue);
        }

        throw JSONException("JSONObjects must hold JSONString or JSONNumber to use operator<");
    }

    inline bool operator<=(const JSONObject& lhs, const JSONObject& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    inline bool operator>(const JSONObject& lhs, const JSONObject& rhs) {
        const auto& lhsValue = lhs.mValue;
        const auto& rhsValue = rhs.mValue;

        if (std::holds_alternative<JSONString>(lhsValue) && std::holds_alternative<JSONString>(rhsValue)) {
            return std::get<JSONString>(lhsValue) > std::get<JSONString>(rhsValue);
        }

        if (std::holds_alternative<JSONNumber>(lhsValue) && std::holds_alternative<JSONNumber>(rhsValue)) {
            return std::get<JSONNumber>(lhsValue) > std::get<JSONNumber>(rhsValue);
        }

        throw JSONException("JSONObjects must hold JSONString or JSONNumber to use operator>");
    }

    inline bool operator>=(const JSONObject& lhs, const JSONObject& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }

    inline std::string JSONObject::toString() const {
        return std::visit(
            [&](const auto& arg) {
                return arg.toString();
            }, mValue);
    }

    inline std::string JSONObject::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        return std::visit(
            [&](const auto& arg) { 
                return arg.toIndentedString(currentIndentation, indentString); 
            }, mValue);
    }

    template <typename T>
    constexpr auto JSONObject::resolveJsonObjectInitialValue(const T& val) const {
        if constexpr (internal::is_same_or_constructible<JSONBool, T>) {
            return JSONBool{val};
        }
        else if constexpr (internal::is_same_or_constructible<JSONNull, T>) {
            return JSONNull{};
        }
        else if constexpr (internal::is_same_or_constructible<JSONString, T>) {
            return JSONString{val};
        }
        else if constexpr (internal::is_same_or_constructible<JSONNumber, T>) {
            return JSONNumber{val};
        }
        else if constexpr (internal::is_same_or_constructible<JSONArray, T>) {
            return JSONArray(val); // MUST NOT USE {} !!!! CAUSES SEGFAULT, TODO: find out why
        }
        else if constexpr (internal::is_same_or_constructible<JSONMap, T>) {
            return JSONMap(val);
        }
        else {
            throw simpleJSON::JSONException("Cannot deduce type of JSONObject constructor argument");
        }
    }
} // namespace simpleJSON 

namespace internal {
    inline simpleJSON::JSONObject beginParseFromStream(std::istream& stream) {
        simpleJSON::JSONObject result;

        char next = peekNextNonSpaceCharacter(stream);

        internal::NextJsonType nextType = internal::detectNextType(next);

        switch (nextType) {
            case internal::NextJsonType::JSON_STRING:
                result = internal::parseString(stream);
                break;
            case internal::NextJsonType::JSON_NUMBER:
                result = internal::parseNumber(stream);
                break;
            case internal::NextJsonType::JSON_BOOL:
                result = internal::parseBool(stream);
                break;
            case internal::NextJsonType::JSON_NULL:
                result = internal::parseNull(stream);
                break;
            case internal::NextJsonType::JSON_ARRAY:
                result = internal::parseArray(stream);
                break;
            case internal::NextJsonType::JSON_OBJECT:
                result = internal::parseObject(stream);
                break;
            case internal::NextJsonType::JSON_END_OF_STREAM: 
                throw simpleJSON::JSONException("Cannot parse empty file or file containing only whitespace");
                break;
            case internal::NextJsonType::JSON_ERROR: [[fallthrough]];
            default:
                throw simpleJSON::JSONException("Error while parsing object, unexpected next character '" + std::string{next} + "'");
                break;
        }
   
        next = peekNextNonSpaceCharacter(stream);

        if (next != std::istream::traits_type::eof()) {
            throw simpleJSON::JSONException("Error after reading a valid json object. Expected EOF but found '" + std::string{next} + "'");
        }

        return result;
    }

    inline NextJsonType detectNextType(const char nextCharInStream) {
        switch (nextCharInStream) {
            case '"':
                return NextJsonType::JSON_STRING;
            case '-': [[fallthrough]];
            case '0': [[fallthrough]];
            case '1': [[fallthrough]];
            case '2': [[fallthrough]];
            case '3': [[fallthrough]];
            case '4': [[fallthrough]];
            case '5': [[fallthrough]];
            case '6': [[fallthrough]];
            case '7': [[fallthrough]];
            case '8': [[fallthrough]];
            case '9':
                return NextJsonType::JSON_NUMBER;
            case 't': [[fallthrough]];
            case 'f':
                return NextJsonType::JSON_BOOL;
            case 'n':
                return NextJsonType::JSON_NULL;
            case '[':
                return NextJsonType::JSON_ARRAY;
            case '{':
                return NextJsonType::JSON_OBJECT;
            case std::istream::traits_type::eof():
                return NextJsonType::JSON_END_OF_STREAM;
            default:
                return NextJsonType::JSON_ERROR;
        }
    }

    inline constexpr bool isValidEscapedCharacter(const char c) {
        return c == '"' || c == '/' || c == '\\' || 
               c == 'b' || c == 'f' || c == 'n' ||
               c == 'r' || c == 't' || c == 'u';
    }

    inline constexpr bool isValidWhitespace(const char c) {
        // form feed (0x0c, '\f') and vertical tab (0x0b, '\v') are not explicitly allowed as whitespace in RFC 8259
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }

    inline char peekNextNonSpaceCharacter(std::istream& stream) {
        char next = static_cast<char>(stream.peek());

        while (isValidWhitespace(next)) {
            stream.get();
            next = static_cast<char>(stream.peek());
        }

        return next;
    }

    inline simpleJSON::JSONFloating strToJSONFloating(const std::string& str) {
        if (str.back() == '.') {
            throw simpleJSON::JSONException("Error while parsing number, decimal point cannot be the last character");
        }

        auto dotIt = std::find(std::cbegin(str), std::cend(str), '.');
        auto eIt = std::find_if(std::cbegin(str), std::cend(str), [](const auto& c) { return c == 'e' || c == 'E'; });

        if (dotIt != std::cend(str) && eIt != std::cend(str) && std::distance(dotIt, eIt) == 1) {
            throw simpleJSON::JSONException("Error while parsing number, 'e' or 'E' cannot be the first character after decimal point");
        }

        if ((str.length() >= 1 && str[0] == '.') || (str.length() >= 2 && (str[0] == '-' && str[1] == '.'))) {
            throw simpleJSON::JSONException("Error while parsing number, missing digit before decimal point");
        }

        char* afterEnd = nullptr;
        simpleJSON::JSONFloating result = std::strtold(str.c_str(), &afterEnd);

        if (*afterEnd != '\0') {
            throw simpleJSON::JSONException("Error while parsing number, invalid floating point number");
        }

        return result;
    }

    inline simpleJSON::JSONInteger strToJSONInteger(const std::string& str) {
        if ((str.length() > 1 && str[0] == '0') || (str.length() > 2 && (str[0] == '-' && str[1] == '0'))) {
            throw simpleJSON::JSONException("Error while parsing number, integer cannot start with 0");
        }

        char* afterEnd = nullptr;
        const int base = 10;
        simpleJSON::JSONInteger result = std::strtoll(str.c_str(), &afterEnd, base);

        if (*afterEnd != '\0') {
            throw simpleJSON::JSONException("Error while parsing number, invalid integer");
        }

        return result;
    }

    inline simpleJSON::JSONString parseString(std::istream& stream) {
        char currentChar{};
        stream.get(currentChar);

        if (currentChar != '"') {
            throw simpleJSON::JSONException("Error while parsing string, expected '\"'");
        }

        std::string result = "";
        bool currentCharIsEscaped = false;
        short numHexDigitsToRead = 0;

        stream.get(currentChar);

        while (stream) {
            if (numHexDigitsToRead > 0) {
                if (!static_cast<bool>(std::isxdigit(static_cast<unsigned char>(currentChar)))) {
                    throw simpleJSON::JSONException("\\u must be followed by 4 hex characters");
                }

                --numHexDigitsToRead;
            }

            if (currentChar == '"' && !currentCharIsEscaped) {
                return result;
            }

            // control characters are in range [0, 31]
            if (0 <= static_cast<int>(currentChar) && static_cast<int>(currentChar) <= 31) {
                throw simpleJSON::JSONException("Error while parsing string, unescaped control character");
            }

            if (currentChar == '\\' && !currentCharIsEscaped) {
                currentCharIsEscaped = true;
            }
            else if (currentCharIsEscaped) {
                if (!isValidEscapedCharacter(currentChar)) {
                    throw simpleJSON::JSONException("Error while parsing string, invalid escaped character");
                }

                if (currentChar == 'u') {
                    numHexDigitsToRead = 4;
                }

                currentCharIsEscaped = false;
            }

            result += currentChar;
            stream.get(currentChar);
        }

        throw simpleJSON::JSONException("Error while parsing string, unexpected end of stream");
    }

    inline simpleJSON::JSONNumber parseNumber(std::istream& stream) {
        char c = static_cast<char>(stream.peek());
        std::string numberAsString;

        short dotCount = 0;
        short eCount = 0;

        while (static_cast<bool>(isdigit(c)) || c == '.' || c == '-' || c == 'e' || c == 'E' || c == '+') {
            if (c == '.') {
                ++dotCount;
            }
            else if (c == 'e' || c == 'E') {
                ++eCount;
            }

            stream.get(c);
            numberAsString += c;

            c = static_cast<char>(stream.peek());
        }

        if (dotCount != 0 || eCount != 0) {
            return simpleJSON::JSONNumber{internal::strToJSONFloating(numberAsString)};
        }

        return simpleJSON::JSONNumber{internal::strToJSONInteger(numberAsString)};
    }

    inline simpleJSON::JSONBool parseBool(std::istream& stream) {
        char c1{};  
        char c2{};  
        char c3{};  
        char c4{};  

        stream.get(c1);
        stream.get(c2);
        stream.get(c3);
        stream.get(c4);

        std::string out = {c1, c2, c3, c4};

        if (out == "true") {
            return simpleJSON::JSONBool(true);
        }

        if (out[0] == 'f') {
            char c5{};
            stream.get(c5);

            out += c5;

            if (out == "false") {
                return simpleJSON::JSONBool(false);
            } 
        }

        throw simpleJSON::JSONException(R"(Error while parsing bool, expected "true" or "false", got ")" + out + "\"");
    }

    inline simpleJSON::JSONNull parseNull(std::istream& stream) {
        char c1{};  
        char c2{};  
        char c3{};  
        char c4{};  

        stream.get(c1);
        stream.get(c2);
        stream.get(c3);
        stream.get(c4);

        std::string out = {c1, c2, c3, c4};

        if (out != "null") {
            throw simpleJSON::JSONException(R"(Error while parsing null, expected "null", got ")" + out + "\"");
        }

        return simpleJSON::JSONNull{};
    }

    inline simpleJSON::JSONArray parseArray(std::istream& stream) {
        char c{};
        stream.get(c);

        if (c != '[') {
            throw simpleJSON::JSONException("Error while parsing array, expected '[', got '" + std::string{c} + "'");
        }

        simpleJSON::JSONArray result;

        bool expectingComma = false;
        bool lastReadWasComma = false;

        while (stream) {
            c = peekNextNonSpaceCharacter(stream);

            if (c == ',') {
                if (expectingComma) {
                    stream.get(c);
                    expectingComma = false;
                    lastReadWasComma = true;
                    continue;
                }

                throw simpleJSON::JSONException("Unexpected comma when parsing array");
            }

            if (c == ']') {
                if (lastReadWasComma) {
                    throw simpleJSON::JSONException("Trailing comma not allowed in array");
                }

                stream.get(c);
                return result;
            }

            if (expectingComma) {
                throw simpleJSON::JSONException("Entries in array must be separated by a comma");
            }

            NextJsonType nextType = detectNextType(c);
            
            switch (nextType) {
                case NextJsonType::JSON_STRING: 
                    result.append(parseString(stream));
                    break;
                case NextJsonType::JSON_NUMBER:
                    result.append(parseNumber(stream));
                    break;
                case NextJsonType::JSON_BOOL:
                    result.append(parseBool(stream));
                    break;
                case NextJsonType::JSON_NULL:
                    result.append(parseNull(stream));
                    break;
                case NextJsonType::JSON_ARRAY:
                    result.append(parseArray(stream));
                    break;
                case NextJsonType::JSON_OBJECT:
                    result.append(parseObject(stream));
                    break;
                case NextJsonType::JSON_END_OF_STREAM:
                    // next read will fail and function will end
                    break;
                case internal::NextJsonType::JSON_ERROR: [[fallthrough]];
                default:
                    throw simpleJSON::JSONException("Error while parsing array, unexpected next character '" + std::string{c} + "'");
                    break;
            }

            expectingComma = true;
            lastReadWasComma = false;
        }

        // should not get here
        throw simpleJSON::JSONException("Error while parsing array, unexpected end of stream");
    }

    inline simpleJSON::JSONObject parseObject(std::istream& stream) {
        char next{};
        stream.get(next);

        if (next != '{') {
            throw simpleJSON::JSONException("Error while parsing object, expected '{', got '" + std::string{next} + "'");
        }

        simpleJSON::JSONObject result;

        bool lastReadWasComma = false;

        while (stream) {
            next = peekNextNonSpaceCharacter(stream);

            if (next == '}') {
                if (lastReadWasComma) {
                    throw simpleJSON::JSONException("Trailing comma not allowed in object");
                }

                stream.get();
                return result;
            }

            lastReadWasComma = false;

            // must read string as map key if object is not empty
            if (next != '"') {
                throw simpleJSON::JSONException("Error while parsing object, expected '\"', got '" + std::string{next} + "'");
            }

            simpleJSON::JSONString key = parseString(stream);

            // possible white space between map key and :
            next = peekNextNonSpaceCharacter(stream);
            stream.get(next);

            // must read separator
            if (next != ':') {
                throw simpleJSON::JSONException("Error while parsing object, expected ':', got '" + std::string{next} + "'");
            }

            // possible white space between : and map value
            next = peekNextNonSpaceCharacter(stream);

            NextJsonType nextType = detectNextType(next);

            switch (nextType) {
                case NextJsonType::JSON_STRING:
                    result[key] = parseString(stream);
                    break;
                case NextJsonType::JSON_NUMBER:
                    result[key] = parseNumber(stream);
                    break;
                case NextJsonType::JSON_BOOL:
                    result[key] = parseBool(stream);
                    break;
                case NextJsonType::JSON_NULL:
                    result[key] = parseNull(stream);
                    break;
                case NextJsonType::JSON_ARRAY:
                    result[key] = parseArray(stream);
                    break;
                case NextJsonType::JSON_OBJECT:
                    result[key] = parseObject(stream);
                    break;
                case NextJsonType::JSON_END_OF_STREAM:
                    // next read will fail and function will end
                    break;
                case internal::NextJsonType::JSON_ERROR: [[fallthrough]];
                default:
                    throw simpleJSON::JSONException("Error while parsing object, unexpected next character '" + std::string{next} + "'");
                    break;
            }

            // possible white space between map value and , or }
            next = peekNextNonSpaceCharacter(stream);
            stream.get(next);

            if (next == ',') {
                lastReadWasComma = true;
                continue;
            }

            if (next == '}') {
                break;
            }

            throw simpleJSON::JSONException("Error while parsing object, unexpected next character '" + std::string{next} + "'");
        }

        return result;
    }
} // namespace internal

#endif //SIMPLE_JSON_HPP
