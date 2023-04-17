#ifndef __SIMPLE_JSON__
#define __SIMPLE_JSON__

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

//------------------------------------- API -------------------------------------

/// @brief Namespace containing all relevant classes and functions
namespace simpleJSON {
    class JSONString;
    class JSONNumber;
    class JSONBool;
    class JSONNull;
    class JSONArray;
    class JSONObject;

    /// @brief Type used to represent floating point numbers.
    /// @details If you wish to change the underlying type, don't forget to also change how it is parsed (see internal::strToJSONFloating__internal) 
    using JSONFloating = long double;
    /// @brief Type used to represent both positive and negative whole numbers.
    /// @details If you wish to change the underlying type, don't forget to also change how it is parsed (see internal::strToJSONInteger__internal) 
    using JSONInteger = long long int;

    /// @brief Default string used for indentation
    /// @details This string will be appended once per indent level.
    const std::string defaultIndentString = "\t";

    /// @brief Function to parse a json file
    /// @details Throws JSONException if anything goes wrong
    /// @param[in] fileName File to parse
    /// @return A JSONObject representing data contained in the file
    JSONObject parseFromFile(const std::filesystem::path& fileName);

    /// @brief Function to parse a string containing json data
    /// @details Throws JSONException if anything goes wrong
    /// @param[in] jsonString String to parse
    /// @return A JSONObject representing data contained in the string
    JSONObject parseFromString(const std::string& jsonString);

    /// @brief Convenience function to represent a json object as a non-formatted string
    /// @details Throws JSONException if anything goes wrong
    /// @param[in] obj Object to turn into a string
    /// @return String representation of json object
    std::string dumpToString(const JSONObject& obj);

    /// @brief Convenience function to represent a json object as an indented string
    /// @details Throws JSONException if anything goes wrong
    /// @param[in] obj Object to turn into a string
    /// @param[in] indentString What indentation to use (defaults to one tab per indent level)
    /// @return String representation of json object
    std::string dumpToPrettyString(const JSONObject& obj, const std::string& indentString = defaultIndentString);

    /// @brief Class to represent various exceptions thrown by library functions
    class JSONException : public std::runtime_error {
        public:
            JSONException(const std::string& msg);
            /// @brief Override of std::runtime_error::what
            /// @return Message contained within the exception
            [[nodiscard]] const char* what() const noexcept override;
    };

    /// @brief Class to represent json strings
    /// @details Internally holds a std::string
    class JSONString {
        public:
            /// @brief Default constructor creates an empty string
            JSONString();
            JSONString(const char* str);
            JSONString(std::string str);

            friend bool operator==(const JSONString& lhs, const JSONString& rhs);
            friend bool operator!=(const JSONString& lhs, const JSONString& rhs);
            friend bool operator<(const JSONString& lhs, const JSONString& rhs);
            friend bool operator<=(const JSONString& lhs, const JSONString& rhs);
            friend bool operator>(const JSONString& lhs, const JSONString& rhs);
            friend bool operator>=(const JSONString& lhs, const JSONString& rhs);
            
            /// @brief Getter for the underlying std::string
            /// @return The underlying std::string
            [[nodiscard]] std::string getString() const;
            /// @brief Convert to string as specified in the json standard 
            /// @return A string surrounded by double quotes
            [[nodiscard]] std::string toString() const;

        private:
            std::string value;
    };

    /// @brief Class to represent json numbers
    /// @details Internally, holds either a floating or integer value
    ///          represented by JSONFloating or JSONInteger respectively
    class JSONNumber {
        public:
            /// @brief Default constructor creates integer with value 0
            JSONNumber();
            /// @brief Constructor that only allows numerical types
            /// @details Will throw JSONException if other types are used
            template <typename N, typename = typename std::enable_if_t<std::is_floating_point_v<N> || std::is_integral_v<N>>>
            JSONNumber(const N& num);

            friend bool operator==(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator!=(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator<(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator<=(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator>(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator>=(const JSONNumber& lhs, const JSONNumber& rhs);

            /// @brief Getter for the underlying floating point number
            /// @details Only valid if JSONNumber actually contains a floating point value, otherwise throws a JSONException
            /// @return The underlying JSONFloating
            [[nodiscard]] JSONFloating getFloating() const;
            /// @brief Getter for the underlying integer number
            /// @details Only valid if JSONNumber actually contains an integer value, otherwise throws a JSONException
            /// @return The underlying JSONInteger
            [[nodiscard]] JSONInteger getInteger() const;
            /// @brief Convert to string as specified in the json standard 
            /// @return String representation of underlying value
            [[nodiscard]] std::string toString() const;

        private:
            std::variant<JSONFloating, JSONInteger> value;
    };

    /// @brief Class to represent json booleans
    /// @details Internally, holds a bool
    class JSONBool {
        public:
            /// @brief Default constructor creates boolean that holds false
            JSONBool();
            JSONBool(const bool val);

            friend bool operator==(const JSONBool& lhs, const JSONBool& rhs);
            friend bool operator!=(const JSONBool& lhs, const JSONBool& rhs);
            
            /// @brief Getter for the underlying boolean
            /// @return The underlying bool
            [[nodiscard]] bool getBoolean() const;
            /// @brief Convert to string as specified in the json standard 
            /// @return String containing either true or false
            [[nodiscard]] std::string toString() const;

        private:
            bool value;
    };

    /// @brief Class to represent json nulls
    /// @details Internally, this is just an empty object
    class JSONNull {
        public:
            JSONNull();
            JSONNull(const std::nullptr_t);

            friend bool operator==(const JSONNull&, const JSONNull&);
            friend bool operator!=(const JSONNull&, const JSONNull&);
            
            /// @brief Convert to string as specified in the json standard 
            /// @return String containing null
            [[nodiscard]] std::string toString() const;
    };

    /// @brief Class to represent json arrays
    /// @details Internally, holds a std::vector of JSONObject-s
    class JSONArray {
        public:
            /// @brief Default constructor creates an empty array
            JSONArray();
            /// @brief Array can be contructed from an initializer list
            /// @note Currently, if you wish to construct a JSONObject
            ///       and not rely on type deduction you must explicitly
            ///       call the JSONObject constructor
            JSONArray(const std::initializer_list<JSONObject> list);

            /// @brief Append anything that can be deduced as JSONObject to this array
            /// @param[in] arg Element to append to this array
            template <typename T>
            void append(T&& arg);
            /// @brief Remove element from the back of the array
            void pop();
            /// @brief Get number of elements in the array
            /// @return Number of elements in the array
            [[nodiscard]] size_t size() const;
            /// @brief Remove all elements from the array
            void clear();

            /// @brief Access the element at specified index
            /// @details Throws JSONException if index is greater than size
            /// @param[in] index Index to access
            JSONObject& operator[](const size_t index);
            /// @brief Constant acces to element at specified index
            /// @details Throws JSONException if index is greater than size
            /// @param[in] index Index to access
            const JSONObject& operator[](const size_t index) const;
            
            friend bool operator==(const JSONArray& lhs, const JSONArray& rhs);
            friend bool operator!=(const JSONArray& lhs, const JSONArray& rhs);
            
            /// @brief Convert to string as specified in the json standard 
            /// @return String representation of underlying array
            [[nodiscard]] std::string toString() const;
            /// @brief Convert to indented string as specified in the json standard 
            /// @return String representation of underlying array
            [[nodiscard]] std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

        private:
            std::vector<JSONObject> value;
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
            JSONObject(const char* str);
            JSONObject(const std::string& str);
            JSONObject(const JSONString& str);
            template <typename N, typename = typename std::enable_if_t<std::is_floating_point_v<N> || std::is_integral_v<N>>>
            JSONObject(const N& num);
            JSONObject(const JSONNumber& num);
            JSONObject(const bool b);
            JSONObject(const JSONBool b);
            JSONObject(const std::nullptr_t);
            JSONObject(const JSONNull n);
            JSONObject(const JSONArray& arr);
            JSONObject(const std::initializer_list<std::pair<const JSONString, JSONObject>> list);

            /// @brief Append anything that can be deduced as JSONObject to array contained in this object 
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            /// @param[in] arg Element to append to this array
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
            /// @return Number of elements in the array
            [[nodiscard]] size_t size() const;
            /// @brief Remove all elements from the array or map contained in this object
            /// @details Only valid if this JSONObject holds an array or map, otherwise throws a JSONException
            void clear();

            /// @brief Access the element at specified index in array conained in this object
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            /// @param[in] index Index to access
            JSONObject& operator[](const size_t index);
            /// @brief Constant access the element at specified index in array conained in this object
            /// @details Only valid if this JSONObject holds an array, otherwise throws a JSONException
            /// @param[in] index Index to access
            const JSONObject& operator[](const size_t index) const;
            /// @brief Access the value at specified key in map conained in this object
            /// @details Only valid if this JSONObject holds a map, otherwise throws a JSONException
            /// @param[in] key Key to access
            JSONObject& operator[](const JSONString& key);
            /// @brief Constant access the value at specified key in map conained in this object
            /// @details Only valid if this JSONObject holds a map, otherwise throws a JSONException
            /// @param[in] key Key to access
            const JSONObject& operator[](const JSONString& key) const;

            friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator<(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator<=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator>(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator>=(const JSONObject& lhs, const JSONObject& rhs);

            /// @brief Convert to string as specified in the json standard 
            /// @details Format of string depends on the type contained within this object
            /// @return String representation of underlying object
            [[nodiscard]] std::string toString() const;
            /// @brief Convert to indented string as specified in the json standard 
            /// @details Format of string depends on the type contained within this object
            /// @return String representation of underlying object
            [[nodiscard]] std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

        private:
            std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, std::map<JSONString, JSONObject>> value;
    };
}   // namespace simpleJSON 

//------------------------------------- IMPLEMENTATION -------------------------------------

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

    simpleJSON::JSONObject beginParseFromStream__internal(std::istream& stream);

    NextJsonType detectNextType__internal(const char nextCharInStream);

    bool isValidEscapedCharacter__internal(const char c);
    bool isValidWhitespace__internal(const char c);
    char peekNextNonSpaceCharacter__internal(std::istream& stream);

    simpleJSON::JSONFloating strToJSONFloating__internal(const std::string& str);
    simpleJSON::JSONInteger strToJSONInteger__internal(const std::string& str);

    simpleJSON::JSONString parseString__internal(std::istream& stream);
    simpleJSON::JSONNumber parseNumber__internal(std::istream& stream);
    simpleJSON::JSONBool parseBool__internal(std::istream& stream);
    simpleJSON::JSONNull parseNull__internal(std::istream& stream);
    simpleJSON::JSONArray parseArray__internal(std::istream& stream);
    simpleJSON::JSONObject parseObject__internal(std::istream& stream);
} // namespace internal

namespace simpleJSON {
    JSONObject parseFromFile(const std::filesystem::path& fileName) {
        std::ifstream stream(fileName);
        return internal::beginParseFromStream__internal(stream);
    }

    JSONObject parseFromString(const std::string& jsonString) {
        std::stringstream stream(jsonString);
        return internal::beginParseFromStream__internal(stream);
    }

    // void dumpToFile(const char* fileName);

    std::string dumpToString(const JSONObject& obj) {
        return obj.toString();
    }

    std::string dumpToPrettyString(const JSONObject& obj, const std::string& indentString) {
        std::string currentIndentation = "";
        return obj.toIndentedString(currentIndentation, indentString);
    }

    // JSONexception

    JSONException::JSONException(const std::string& msg) : std::runtime_error(msg) {}

    const char* JSONException::what() const noexcept {
        return std::runtime_error::what();
    }

    // JSONString

    bool operator==(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.value == rhs.value;
    }

    bool operator!=(const JSONString& lhs, const JSONString& rhs) { 
        return !(lhs == rhs);
    }

    bool operator<(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.value < rhs.value;
    }

    bool operator<=(const JSONString& lhs, const JSONString& rhs) { 
        return (lhs < rhs) || (lhs == rhs);
    }

    bool operator>(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.value > rhs.value;
    }

    bool operator>=(const JSONString& lhs, const JSONString& rhs) { 
        return (lhs > rhs) || (lhs == rhs);
    }

    JSONString::JSONString() : value(std::string{}) {}
    JSONString::JSONString(const char* str) : value(std::string(str)) {}
    JSONString::JSONString(std::string str) : value(std::move(str)) {}

    std::string JSONString::getString() const {
        return value;
    }

    std::string JSONString::toString() const {
        return "\"" + value + "\"";
    }

    // JSONNumber

    JSONNumber::JSONNumber() : value(JSONInteger(0)) {}
    
    template <typename N, typename>
    JSONNumber::JSONNumber(const N& num) {
       

        if constexpr (std::is_floating_point_v<N>) {
			value = JSONFloating(num);
		}
		else if constexpr (std::is_integral_v<N>) {
			value = JSONInteger(num);
		}
        else {
            throw JSONException("JSONNumber must be created with floating point or integer value");
        }
    }

    bool operator==(const JSONNumber& lhs, const JSONNumber& rhs) {
        return lhs.value == rhs.value;
    }

    bool operator!=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return !(lhs == rhs);
    }

    bool operator<(const JSONNumber& lhs, const JSONNumber& rhs) {
        // FIXME: find a better way to implement this method!!!
        // std::variant::operator< and operator > do not behave the way we need
        const auto& lhsValue = lhs.value;
        const auto& rhsValue = rhs.value;

        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) < std::get<JSONFloating>(rhsValue);
        }
        
        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONInteger>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) < std::get<JSONInteger>(rhsValue);
        }
        
        if (std::holds_alternative<JSONInteger>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONInteger>(lhsValue) < std::get<JSONFloating>(rhsValue);
        }
        
        if (std::holds_alternative<JSONInteger>(lhsValue) && std::holds_alternative<JSONInteger>(rhsValue)) { 
            return std::get<JSONInteger>(lhsValue) < std::get<JSONInteger>(rhsValue);
        }
        
        // should not get here
        throw JSONException("Unexpected comparison on JSONNumber");
    }

    bool operator<=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    bool operator>(const JSONNumber& lhs, const JSONNumber& rhs) {
        // FIXME: find a better way to implement this method!!!
        // std::variant::operator< and operator > do not behave the way we need
        const auto& lhsValue = lhs.value;
        const auto& rhsValue = rhs.value;
        
        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) > std::get<JSONFloating>(rhsValue);
        }
        
        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONInteger>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) > std::get<JSONInteger>(rhsValue);
        }
        
        if (std::holds_alternative<JSONInteger>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONInteger>(lhsValue) > std::get<JSONFloating>(rhsValue);
        }
         
        if (std::holds_alternative<JSONInteger>(lhsValue) && std::holds_alternative<JSONInteger>(rhsValue)) { 
            return std::get<JSONInteger>(lhsValue) > std::get<JSONInteger>(rhsValue);
        }

        // should not get here
        throw JSONException("Unexpected comparison on JSONNumber");
    }

    bool operator>=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }

    JSONFloating JSONNumber::getFloating() const {
        if (std::holds_alternative<JSONFloating>(value)) {
            return std::get<JSONFloating>(value);
        }
        
        throw JSONException("This JSONNumber does not contain a floating point value");
    }

    JSONInteger JSONNumber::getInteger() const {
        if (std::holds_alternative<JSONInteger>(value)) {
            return std::get<JSONInteger>(value);
        }
       
        throw JSONException("This JSONNumber does not contain an integer value");
    }

    std::string JSONNumber::toString() const {
        std::string res;

        auto toStringLambda = [&](auto&& arg) {
            res += std::to_string(arg);
        };
        
        std::visit(toStringLambda, value);

        return res;
    }

    // JSONBool

    JSONBool::JSONBool() : value(false) {}
    JSONBool::JSONBool(bool val) : value(val) {}

    bool operator==(const JSONBool& lhs, const JSONBool& rhs) {
        return lhs.value == rhs.value;
    }

    bool operator!=(const JSONBool& lhs, const JSONBool& rhs) {
        return !(lhs == rhs);
    }

    bool JSONBool::getBoolean() const {
        return value;
    }

    std::string JSONBool::toString() const {
        return value ? std::string("true") : std::string("false");
    }

    // JSONNull

    JSONNull::JSONNull() = default;
    
    JSONNull::JSONNull(const std::nullptr_t) {}

    bool operator==(const JSONNull&, const JSONNull&) {
        return true;
    }

    bool operator!=(const JSONNull&, const JSONNull&) {
        return false;
    }

    std::string JSONNull::toString() const {
        return std::string("null");
    }

    // JSONArray

    JSONArray::JSONArray() : value(std::vector<JSONObject>{}) {}
    JSONArray::JSONArray(const std::initializer_list<JSONObject> list) : value(list) {}

    template <typename T>
    void JSONArray::append(T&& arg) {
        value.emplace_back(std::forward<T>(arg));
        return;
    }

    void JSONArray::pop() {
        value.pop_back();
        return;
    }

    size_t JSONArray::size() const {
        return value.size();
    }
    
    void JSONArray::clear() {
        value.clear();
        return;
    }

    JSONObject& JSONArray::operator[](const size_t index) {
        if (index >= value.size()) {
            throw JSONException("JSONArray operator[] index out of range");
        }
        
        return value[index];
    }

    const JSONObject& JSONArray::operator[](const size_t index) const {
        if (index >= value.size()) {
            throw JSONException("JSONArray operator[] index out of range");
        }
        
        return value.at(index);
    }

    bool operator==(const JSONArray& lhs, const JSONArray& rhs) {
        return lhs.value == rhs.value;
    }

    bool operator!=(const JSONArray& lhs, const JSONArray& rhs) {
        return !(lhs == rhs);
    }

    std::string JSONArray::toString() const {
        if (size() == 0) {
            return "[]";
        }
        
        std::string res = "[";

        for(const auto& elem : value) {
            res += elem.toString() + ",";
        }

        res.back() = ']';

        return res;
    }

    std::string JSONArray::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        if (size() == 0) {
            return "[]";
        }
        
        std::string res = "[\n";

        currentIndentation += indentString;

        for (const auto& elem : value) {
            res += currentIndentation + elem.toIndentedString(currentIndentation, indentString) + ",\n";
        }

        currentIndentation.erase(currentIndentation.length() - indentString.length());

        res.erase(res.length() - 2);
        res += "\n" + currentIndentation + "]";

        return res;
    }

    // JSONObject

    JSONObject::JSONObject() : value(std::map<JSONString, JSONObject>{}) {}
    JSONObject::JSONObject(const char* str) : value(JSONString(str)) {}
    JSONObject::JSONObject(const std::string& str) : value(JSONString(str)) {}
    JSONObject::JSONObject(const JSONString& str) : value(str) {}
    template <typename N, typename>
    JSONObject::JSONObject(const N& num) : value(JSONNumber(num)) {}
    JSONObject::JSONObject(const JSONNumber& num) : value(num) {}
    JSONObject::JSONObject(const bool b) : value(JSONBool(b)) {}
    JSONObject::JSONObject(const JSONBool b) : value(b) {}
    JSONObject::JSONObject(const std::nullptr_t) : value(JSONNull{}) {}
    JSONObject::JSONObject(const JSONNull n) : value(n) {}
    JSONObject::JSONObject(const JSONArray& arr) : value(arr) {}
    JSONObject::JSONObject(const std::initializer_list<std::pair<const JSONString, JSONObject>> list) : value(list) {}

    template <typename T>
    void JSONObject::append(T&& arg) {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            arr.append(std::forward<T>(arg));
            return;
        }
        
        throw JSONException("Cannot append. Current object is not an array");
    }

    void JSONObject::pop() {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            arr.pop();
            return;
        }

        throw JSONException("Cannot pop. Current JSONObject does not hold an array");
    }

    void JSONObject::removeField(const JSONString& key) {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);

            auto it = map.find(key);
            if (it != std::end(map)) {
                map.erase(it);
            }

            return;
        }
        
        throw JSONException("Removing field failed, this JSONObject is not a map");      
    }

    size_t JSONObject::size() const {
        if (std::holds_alternative<JSONArray>(value)) {
            const auto& arr = std::get<JSONArray>(value);
            return arr.size();
        }

        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            const auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map.size();
        }
        
        throw JSONException("Current JSONObject is not an array or map, cannot call size()");
    }
    
    void JSONObject::clear() {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            return arr.clear();
        }

        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map.clear();
        }
        
        throw JSONException("Current JSONObject is not an array or map, cannot call clear()");
    }

    JSONObject& JSONObject::operator[](const size_t index) {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            return arr[index];
        }

        throw JSONException("Operator[] failed, this JSONObject is not an array");
    }

    const JSONObject& JSONObject::operator[](const size_t index) const {
        if (std::holds_alternative<JSONArray>(value)) {
            const auto& arr = std::get<JSONArray>(value);
            return arr[index];
        }
        
        throw JSONException("Operator[] failed, this JSONObject is not an array");
    }

    JSONObject& JSONObject::operator[](const JSONString& key) {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map[key];
        }
       
        throw JSONException("Operator[] failed, this JSONObject is not a map");
    }

    const JSONObject& JSONObject::operator[](const JSONString& key) const {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            const auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map.at(key);
        }
        
        throw JSONException("Operator[] failed, this JSONObject is not a map");
    }   

    bool operator==(const JSONObject& lhs, const JSONObject& rhs) {
        return lhs.value == rhs.value;
    }
    
    bool operator!=(const JSONObject& lhs, const JSONObject& rhs) {
        return !(lhs == rhs);
    }

    bool operator<(const JSONObject& lhs, const JSONObject& rhs) {
        const auto& lhsValue = lhs.value;
        const auto& rhsValue = rhs.value;
        
        if (std::holds_alternative<JSONString>(lhsValue) && std::holds_alternative<JSONString>(rhsValue)) {
            return std::get<JSONString>(lhsValue) < std::get<JSONString>(rhsValue);
        }
        
        if (std::holds_alternative<JSONNumber>(lhsValue) && std::holds_alternative<JSONNumber>(rhsValue)) {
            return std::get<JSONNumber>(lhsValue) < std::get<JSONNumber>(rhsValue);
        }
        
        throw JSONException("JSONObjects must hold JSONString or JSONNumber to use operator<");
    }

    bool operator<=(const JSONObject& lhs, const JSONObject& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    bool operator>(const JSONObject& lhs, const JSONObject& rhs) {
        const auto& lhsValue = lhs.value;
        const auto& rhsValue = rhs.value;
        
        if (std::holds_alternative<JSONString>(lhsValue) && std::holds_alternative<JSONString>(rhsValue)) {
            return std::get<JSONString>(lhsValue) > std::get<JSONString>(rhsValue);
        }
        
        if (std::holds_alternative<JSONNumber>(lhsValue) && std::holds_alternative<JSONNumber>(rhsValue)) {
            return std::get<JSONNumber>(lhsValue) > std::get<JSONNumber>(rhsValue);
        }
        
        throw JSONException("JSONObjects must hold JSONString or JSONNumber to use operator>");
    }

    bool operator>=(const JSONObject& lhs, const JSONObject& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }
    
    std::string JSONObject::toString() const {
        if (std::holds_alternative<JSONString>(value)) {
            return std::get<JSONString>(value).toString();
        }
        
        if (std::holds_alternative<JSONNumber>(value)) {
            return std::get<JSONNumber>(value).toString();
        }
        
        if (std::holds_alternative<JSONBool>(value)) {
            return std::get<JSONBool>(value).toString();
        }
        
        if (std::holds_alternative<JSONNull>(value)) {
            return std::get<JSONNull>(value).toString();
        }
        
        if (std::holds_alternative<JSONArray>(value)) {
            return std::get<JSONArray>(value).toString();
        }
        
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            const auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            
            if (map.empty()) {
                return "{}";
            }

            std::string res = "{";

            for (const auto& [key, val] : map) {
                res += key.toString() + ":" + val.toString() + ",";
            }

            res.back() = '}';
                
            return res;
        }
        
        // should not get here
        throw JSONException("Error when converting JSONObject to string");
    }

    std::string JSONObject::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        if (std::holds_alternative<JSONString>(value)) {
            return std::get<JSONString>(value).toString();
        }
        
        if (std::holds_alternative<JSONNumber>(value)) {
            return std::get<JSONNumber>(value).toString();
        }
        
        if (std::holds_alternative<JSONBool>(value)) {
            return std::get<JSONBool>(value).toString();
        }
        
        if (std::holds_alternative<JSONNull>(value)) {
            return std::get<JSONNull>(value).toString();
        }
        
        if (std::holds_alternative<JSONArray>(value)) {
            return std::get<JSONArray>(value).toIndentedString(currentIndentation, indentString);
        }
        
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            const auto& map = std::get<std::map<JSONString, JSONObject>>(value);

            if (map.empty()) {
                return "{}";
            }
            
            std::string res = "{\n";

            currentIndentation += indentString;

            for (const auto& [key, val] : map) {
                res += currentIndentation + key.toString() + " : " + val.toIndentedString(currentIndentation, indentString) + ",\n";
            }

            currentIndentation.erase(currentIndentation.length() - indentString.length());

            res.erase(res.length() - 2);
            res += "\n" + currentIndentation + "}";
                
            return res;
        }
        
        // should not get here
        throw JSONException("Error when converting JSONObject to string");
    }
} // namespace simpleJSON 

namespace internal {
    simpleJSON::JSONObject beginParseFromStream__internal(std::istream& stream) {
        simpleJSON::JSONObject result;
        
        char next = peekNextNonSpaceCharacter__internal(stream);

        internal::NextJsonType nextType = internal::detectNextType__internal(next);

        switch (nextType) {
            case internal::NextJsonType::JSON_STRING:
                result = internal::parseString__internal(stream);
                break;
            case internal::NextJsonType::JSON_NUMBER:
                result = internal::parseNumber__internal(stream);
                break;
            case internal::NextJsonType::JSON_BOOL:
                result = internal::parseBool__internal(stream);
                break;
            case internal::NextJsonType::JSON_NULL:
                result = internal::parseNull__internal(stream);
                break;
            case internal::NextJsonType::JSON_ARRAY:
                result = internal::parseArray__internal(stream);
                break;
            case internal::NextJsonType::JSON_OBJECT:
                result = internal::parseObject__internal(stream);
                break;
            case internal::NextJsonType::JSON_END_OF_STREAM: 
                throw simpleJSON::JSONException("Cannot parse empty file or file containing only whitespace");
                break;
            case internal::NextJsonType::JSON_ERROR: [[fallthrough]];
            default:
                throw simpleJSON::JSONException("Error while parsing object, unexpected next character '" + std::string{next} + "'");
                break;
        }
   
        next = peekNextNonSpaceCharacter__internal(stream);

        if (next != std::istream::traits_type::eof()) {
            throw simpleJSON::JSONException("Error after reading a valid json object. Expected EOF but found '" + std::string{next} + "'");
        }
        
        return result;
    }

    NextJsonType detectNextType__internal(const char nextCharInStream) {
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

    bool isValidEscapedCharacter__internal(const char c) {
        return c == '"' || c == '\\' || c == '/' || c == 'b' || 
               c == 'f' || c == 'n' || c == 'r' || c == 't' || c == 'u';
    }

    bool isValidWhitespace__internal(const char c) {
        // form feed (0x0c, '\f') and vertical tab (0x0b, '\v') are not explicitly allowed as whitespace in RFC 8259
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }

    char peekNextNonSpaceCharacter__internal(std::istream& stream) {
        char next = stream.peek();
        
        while (isValidWhitespace__internal(next)) {
            stream.get();
            next = stream.peek();
        }

        return next;
    }

    simpleJSON::JSONFloating strToJSONFloating__internal(const std::string& str) {
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

    simpleJSON::JSONInteger strToJSONInteger__internal(const std::string& str) {
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

    simpleJSON::JSONString parseString__internal(std::istream& stream) {
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
                if (!isValidEscapedCharacter__internal(currentChar)) {
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

    simpleJSON::JSONNumber parseNumber__internal(std::istream& stream) {
        char c = stream.peek();
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

            c = stream.peek();
        }
        
        if (dotCount != 0 || eCount != 0) {
            return simpleJSON::JSONNumber{internal::strToJSONFloating__internal(numberAsString)};
        }
        
        return simpleJSON::JSONNumber{internal::strToJSONInteger__internal(numberAsString)};
    }
    
    simpleJSON::JSONBool parseBool__internal(std::istream& stream) {
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
    
    simpleJSON::JSONNull parseNull__internal(std::istream& stream) {
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

    simpleJSON::JSONArray parseArray__internal(std::istream& stream) {
        char c{};
        stream.get(c);

        if (c != '[') {
            throw simpleJSON::JSONException("Error while parsing array, expected '[', got '" + std::string{c} + "'");
        }

        simpleJSON::JSONArray result;

        bool expectingComma = false;
        bool lastReadWasComma = false;

        while (stream) {
            c = peekNextNonSpaceCharacter__internal(stream);

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

            NextJsonType nextType = detectNextType__internal(c);
            
            switch (nextType) {
                case NextJsonType::JSON_STRING: 
                    result.append(parseString__internal(stream));
                    break;
                case NextJsonType::JSON_NUMBER:
                    result.append(parseNumber__internal(stream));
                    break;
                case NextJsonType::JSON_BOOL:
                    result.append(parseBool__internal(stream));
                    break;
                case NextJsonType::JSON_NULL:
                    result.append(parseNull__internal(stream));
                    break;
                case NextJsonType::JSON_ARRAY:
                    result.append(parseArray__internal(stream));
                    break;
                case NextJsonType::JSON_OBJECT:
                    result.append(parseObject__internal(stream));
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

    simpleJSON::JSONObject parseObject__internal(std::istream& stream) {
        char next{};
        stream.get(next);

        if (next != '{') {
            throw simpleJSON::JSONException("Error while parsing object, expected '{', got '" + std::string{next} + "'");
        }

        simpleJSON::JSONObject result;

        bool lastReadWasComma = false;

        while (stream) {
            next = peekNextNonSpaceCharacter__internal(stream);

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

            simpleJSON::JSONString key = parseString__internal(stream);

            // possible white space between map key and :
            next = peekNextNonSpaceCharacter__internal(stream);
            stream.get(next);

            // must read separator
            if (next != ':') {
                throw simpleJSON::JSONException("Error while parsing object, expected ':', got '" + std::string{next} + "'");
            }

            // possible white space between : and map value
            next = peekNextNonSpaceCharacter__internal(stream);

            NextJsonType nextType = detectNextType__internal(next);
            
            switch (nextType) {
                case NextJsonType::JSON_STRING:
                    result[key] = parseString__internal(stream);
                    break;
                case NextJsonType::JSON_NUMBER:
                    result[key] = parseNumber__internal(stream);
                    break;
                case NextJsonType::JSON_BOOL:
                    result[key] = parseBool__internal(stream);
                    break;
                case NextJsonType::JSON_NULL:
                    result[key] = parseNull__internal(stream);
                    break;
                case NextJsonType::JSON_ARRAY:
                    result[key] = parseArray__internal(stream);
                    break;
                case NextJsonType::JSON_OBJECT:
                    result[key] = parseObject__internal(stream);
                    break;
                case NextJsonType::JSON_END_OF_STREAM:
                    // next read will fail and function will end
                    break;
                case internal::NextJsonType::JSON_ERROR: [[fallthrough]];
                default:
                    throw simpleJSON::JSONException("Error while parsing object, unexpected next character '" + std::string{next} + "'");
                    break;
            }

            //  possible white space between map value and , or }
            next = peekNextNonSpaceCharacter__internal(stream);
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

#endif //__SIMPLE_JSON__