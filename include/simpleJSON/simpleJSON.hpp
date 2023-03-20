#ifndef __SIMPLE_JSON__
#define __SIMPLE_JSON__

#include <exception>
#include <fstream>
#include <initializer_list>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

// #define DEBUG

#ifdef DEBUG    
#include <iostream>
    static unsigned totalTraces = 0u;
    #define FUNCTRACE { ++totalTraces; std::cout << "Calling: "  << __PRETTY_FUNCTION__ << std::endl << "total traces: " << totalTraces << std::endl; }
#else
    #define FUNCTRACE while(0){};
#endif

//------------------------------------- API -------------------------------------

namespace simpleJSON {
    class JSONString;
    class JSONNumber;
    class JSONBool;
    class JSONNull;
    class JSONArray;
    class JSONObject;

    // When changing what types are used as JSONFloating and JSONIntegral, also change how they are parsed
    // in strToJSONFloating__internal and strToJSONIntegral__internal
    using JSONFloating = long double;
    using JSONIntegral = long long int;

    // You may change this to suit your needs
    const std::string defaultIndentString = "\t";

    JSONObject parseFromFile(const char* fileName);
    JSONObject parseFromString(std::string& jsonString);
    // void dumpToFile(const char* fileName);
    std::string dumpToString(const JSONObject& obj);
    std::string dumpToPrettyString(const JSONObject& obj, const std::string& indentString = defaultIndentString);

    class JSONException : public std::exception {
        public:
            JSONException(const char* msg);

            const char * what () const throw ();

        private:
            const char* message;
    };

    class JSONString {
        public:
            JSONString();
            JSONString(const char* str);
            JSONString(const std::string& str);

            friend bool operator==(const JSONString& lhs, const JSONString& rhs);
            friend bool operator!=(const JSONString& lhs, const JSONString& rhs);
            friend bool operator<(const JSONString& lhs, const JSONString& rhs);
            friend bool operator<=(const JSONString& lhs, const JSONString& rhs);
            friend bool operator>(const JSONString& lhs, const JSONString& rhs);
            friend bool operator>=(const JSONString& lhs, const JSONString& rhs);

            std::string getString() const;
            std::string toString() const;

        private:
            std::string value;
    };

    class JSONNumber {
        public:
            JSONNumber();
            template <typename N, typename = typename std::enable_if_t<std::is_floating_point_v<N> || std::is_integral_v<N>>>
            JSONNumber(const N& num);

            friend bool operator==(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator!=(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator<(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator<=(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator>(const JSONNumber& lhs, const JSONNumber& rhs);
            friend bool operator>=(const JSONNumber& lhs, const JSONNumber& rhs);

            JSONFloating getFloating() const;
            JSONIntegral getIntegral() const;
            std::string toString() const;

        private:
            std::variant<JSONFloating, JSONIntegral> value;
    };

    class JSONBool {
        public:
            JSONBool();
            JSONBool(const bool val);

            friend bool operator==(const JSONBool& lhs, const JSONBool& rhs);
            friend bool operator!=(const JSONBool& lhs, const JSONBool& rhs);

            bool getBoolean() const;
            std::string toString() const;

        private:
            bool value;
    };

    class JSONNull {
        public:
            JSONNull();
            JSONNull(const std::nullptr_t);

            friend bool operator==(const JSONNull&, const JSONNull&);
            friend bool operator!=(const JSONNull&, const JSONNull&);
            
            std::string toString() const;
    };

    class JSONArray {
        public:
            JSONArray();
            JSONArray(const std::initializer_list<JSONObject> list);

            template <typename T>
            void append(T&& arg);

            void pop();

            JSONObject& operator[](const size_t index);
            const JSONObject& operator[](const size_t index) const;

            friend bool operator==(const JSONArray& lhs, const JSONArray& rhs);
            friend bool operator!=(const JSONArray& lhs, const JSONArray& rhs);
            
            size_t size() const;
            void clear();
            std::string toString() const;
            std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

        private:
            std::vector<JSONObject> value;
    };

    class JSONObject {
        public:
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

            template <typename T>
            void append(T&& arg);
            void pop();
            size_t size() const;
            void clear();

            JSONObject& operator[](const size_t index);
            const JSONObject& operator[](const size_t index) const;

            void removeField(const JSONString& key);
            size_t getNumberOfFields() const;

            JSONObject& operator[](const JSONString& key);
            const JSONObject& operator[](const JSONString& key) const;

            friend bool operator==(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator!=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator<(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator<=(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator>(const JSONObject& lhs, const JSONObject& rhs);
            friend bool operator>=(const JSONObject& lhs, const JSONObject& rhs);

            std::string toString() const;
            std::string toIndentedString(std::string& currentIndentation, const std::string& indentString) const;

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

    NextJsonType detectNextType__internal(char nextCharInStream);

    char peekNextNonSpaceCharacter__internal(std::istream& stream);

    simpleJSON::JSONFloating strToJSONFloating__internal(const std::string& str);
    simpleJSON::JSONIntegral strToJSONIntegral__internal(const std::string& str);

    simpleJSON::JSONString parseString__internal(std::istream& stream);
    simpleJSON::JSONNumber parseNumber__internal(std::istream& stream);
    simpleJSON::JSONBool parseBool__internal(std::istream& stream);
    simpleJSON::JSONNull parseNull__internal(std::istream& stream);
    simpleJSON::JSONArray parseArray__internal(std::istream& stream);
    simpleJSON::JSONObject parseObject__internal(std::istream& stream);
} // namespace internal

namespace simpleJSON {
    JSONObject parseFromFile(const char* fileName) {
        FUNCTRACE

        std::ifstream stream(fileName);
        return internal::beginParseFromStream__internal(stream);
    }

    JSONObject parseFromString(std::string& jsonString) {
        FUNCTRACE

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

    JSONException::JSONException(const char* msg) : message(msg) {}

    const char* JSONException::what () const throw () {
        return message;
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

    JSONString::JSONString() : value(std::string{}) { FUNCTRACE }
    JSONString::JSONString(const char* str) : value(std::string(str)) { FUNCTRACE }
    JSONString::JSONString(const std::string& str) : value(str) { FUNCTRACE }

    std::string JSONString::getString() const {
        return value;
    }

    std::string JSONString::toString() const {
        return "\"" + value + "\"";
    }

    // JSONNumber

    JSONNumber::JSONNumber() : value(JSONIntegral(0)) { FUNCTRACE }
    
    template <typename N, typename>
    JSONNumber::JSONNumber(const N& num) {
        FUNCTRACE 

        if constexpr (std::is_floating_point_v<N>) {
			value = JSONFloating(num);
		}
		else if constexpr (std::is_integral_v<N>) {
			value = JSONIntegral(num);
		}
        else {
            throw JSONException("JSONNumber must be created with floating point or integral value");
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
        auto& lhsValue = lhs.value;
        auto& rhsValue = rhs.value;

        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) < std::get<JSONFloating>(rhsValue);
        }
        else if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONIntegral>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) < std::get<JSONIntegral>(rhsValue);
        }
        else if (std::holds_alternative<JSONIntegral>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONIntegral>(lhsValue) < std::get<JSONFloating>(rhsValue);
        }
        else if (std::holds_alternative<JSONIntegral>(lhsValue) && std::holds_alternative<JSONIntegral>(rhsValue)) { 
            return std::get<JSONIntegral>(lhsValue) < std::get<JSONIntegral>(rhsValue);
        }
        else {
            // should not get here
            throw JSONException("Unexpected comparison on JSONNumber");
        }
    }

    bool operator<=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    bool operator>(const JSONNumber& lhs, const JSONNumber& rhs) {
        // FIXME: find a better way to implement this method!!!
        // std::variant::operator< and operator > do not behave the way we need
        auto& lhsValue = lhs.value;
        auto& rhsValue = rhs.value;
        
        if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) > std::get<JSONFloating>(rhsValue);
        }
        else if (std::holds_alternative<JSONFloating>(lhsValue) && std::holds_alternative<JSONIntegral>(rhsValue)) { 
            return std::get<JSONFloating>(lhsValue) > std::get<JSONIntegral>(rhsValue);
        }
        else if (std::holds_alternative<JSONIntegral>(lhsValue) && std::holds_alternative<JSONFloating>(rhsValue)) { 
            return std::get<JSONIntegral>(lhsValue) > std::get<JSONFloating>(rhsValue);
        }
        else if (std::holds_alternative<JSONIntegral>(lhsValue) && std::holds_alternative<JSONIntegral>(rhsValue)) { 
            return std::get<JSONIntegral>(lhsValue) > std::get<JSONIntegral>(rhsValue);
        }
        else {
            // should not get here
            throw JSONException("Unexpected comparison on JSONNumber");
        }
    }

    bool operator>=(const JSONNumber& lhs, const JSONNumber& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }

    JSONFloating JSONNumber::getFloating() const {
        if (std::holds_alternative<JSONFloating>(value)) {
            return std::get<JSONFloating>(value);
        }
        else {
            throw JSONException("This JSONNumber does not contain a floating point value");
        }
    }

    JSONIntegral JSONNumber::getIntegral() const {
        if (std::holds_alternative<JSONIntegral>(value)) {
            return std::get<JSONIntegral>(value);
        }
        else {
            throw JSONException("This JSONNumber does not contain a signed integral value");
        }
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

    JSONBool::JSONBool() : value(false) { FUNCTRACE }
    JSONBool::JSONBool(bool val) : value(val) { FUNCTRACE }

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

    JSONNull::JSONNull() {}
    
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

    JSONArray::JSONArray() : value(std::vector<JSONObject>{}) { FUNCTRACE }
    JSONArray::JSONArray(const std::initializer_list<JSONObject> list) : value(list) { FUNCTRACE }

    template <typename T>
    void JSONArray::append(T&& arg) {
        FUNCTRACE
        value.emplace_back(std::forward<T>(arg));
        return;
    }

    void JSONArray::pop() {
        FUNCTRACE
        value.pop_back();
        return;
    }

    JSONObject& JSONArray::operator[](const size_t index) {
        if (index >= value.size()) {
            throw JSONException("JSONArray operator[] index out of range");
        }
        else {
            return value[index];
        }
    }

    const JSONObject& JSONArray::operator[](const size_t index) const {
        if (index >= value.size()) {
            throw JSONException("JSONArray operator[] index out of range");
        }
        else {
            return value.at(index);
        }
    }

    bool operator==(const JSONArray& lhs, const JSONArray& rhs) {
        return lhs.value == rhs.value;
    }

    bool operator!=(const JSONArray& lhs, const JSONArray& rhs) {
        return !(lhs == rhs);
    }

    size_t JSONArray::size() const {
        return value.size();
    }
    
    void JSONArray::clear() {
        value.clear();
        return;
    }

    std::string JSONArray::toString() const {
        if (size() == 0) {
            return "[]";
        }
        else {
            std::string res = "[";
         
            for(auto& elem : value) {
                res += elem.toString() + ",";
            }

            res.back() = ']';

            return res;
        }
    }

    std::string JSONArray::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        if (size() == 0) {
            return "[]";
        }
        else {
            std::string res = "[\n";

            currentIndentation += indentString;

            for (auto& elem : value) {
                res += currentIndentation + elem.toIndentedString(currentIndentation, indentString) + ",\n";
            }

            currentIndentation.erase(currentIndentation.length() - indentString.length());

            res.erase(res.length() - 2);
            res += "\n" + currentIndentation + "]";

            return res;
        }
    }

    // JSONObject

    JSONObject::JSONObject() : value(std::map<JSONString, JSONObject>{}) { FUNCTRACE }
    JSONObject::JSONObject(const char* str) : value(JSONString(str)) { FUNCTRACE }
    JSONObject::JSONObject(const std::string& str) : value(JSONString(str)) { FUNCTRACE }
    JSONObject::JSONObject(const JSONString& str) : value(str) { FUNCTRACE }
    template <typename N, typename>
    JSONObject::JSONObject(const N& num) : value(JSONNumber(num)) { FUNCTRACE }
    JSONObject::JSONObject(const JSONNumber& num) : value(num) { FUNCTRACE }
    JSONObject::JSONObject(const bool b) : value(JSONBool(b)) { FUNCTRACE }
    JSONObject::JSONObject(const JSONBool b) : value(b) { FUNCTRACE }
    JSONObject::JSONObject(const std::nullptr_t) : value(JSONNull{}) { FUNCTRACE }
    JSONObject::JSONObject(const JSONNull n) : value(n) { FUNCTRACE }
    JSONObject::JSONObject(const JSONArray& arr) : value(arr) { FUNCTRACE }
    JSONObject::JSONObject(const std::initializer_list<std::pair<const JSONString, JSONObject>> list) : value(list) { FUNCTRACE }

    template <typename T>
    void JSONObject::append(T&& arg) {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            arr.append(std::forward<T>(arg));
        }
        else {
            throw JSONException("Cannot append. Current object is not an array");
        }
        return;
    }

    void JSONObject::pop() {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            arr.pop();
            return;
        }
        else {
            throw JSONException("Cannot pop. Current JSONObject does not hold an array");
        }
    }

    JSONObject& JSONObject::operator[](const size_t index) {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            return arr[index];
        }
        else {
            throw JSONException("Operator[] failed, this JSONObject is not an array");
        }
    }

    const JSONObject& JSONObject::operator[](const size_t index) const {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            return arr[index];
        }
        else {
            throw JSONException("Operator[] failed, this JSONObject is not an array");
        }
    }

    size_t JSONObject::size() const {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            return arr.size();
        }
        else {
            throw JSONException("Current JSONObject is not an array, cannot call size()");
        }
    }
    
    void JSONObject::clear() {
        if (std::holds_alternative<JSONArray>(value)) {
            auto& arr = std::get<JSONArray>(value);
            return arr.clear();
        }
        else {
            throw JSONException("Current JSONObject is not an array, cannot call clear()");
        }
    }

    void JSONObject::removeField(const JSONString& key) {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);

            auto it = map.find(key);
            if (it != std::end(map)) {
                map.erase(it);
            }
        }
        else {
            throw JSONException("Removing field failed, this JSONObject is not a map");
        }
        return;
    }

    
    size_t JSONObject::getNumberOfFields() const {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map.size();
        }
        else {
            throw JSONException("Removing field failed, this JSONObject is not a map");
        }
    }

    JSONObject& JSONObject::operator[](const JSONString& key) {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map[key];
        }
        else {
            throw JSONException("Operator[] failed, this JSONObject is not a map");
        }
    }

    const JSONObject& JSONObject::operator[](const JSONString& key) const {
        if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            return map.at(key);
        }
        else {
            throw JSONException("Operator[] failed, this JSONObject is not a map");
        }
    }   

    bool operator==(const JSONObject& lhs, const JSONObject& rhs) {
        return lhs.value == rhs.value;
    }
    
    bool operator!=(const JSONObject& lhs, const JSONObject& rhs) {
        return !(lhs == rhs);
    }

    bool operator<(const JSONObject& lhs, const JSONObject& rhs) {
        auto& lhsValue = lhs.value;
        auto& rhsValue = rhs.value;
        
        if (std::holds_alternative<JSONString>(lhsValue) && std::holds_alternative<JSONString>(rhsValue)) {
            return std::get<JSONString>(lhsValue) < std::get<JSONString>(rhsValue);
        }
        else if (std::holds_alternative<JSONNumber>(lhsValue) && std::holds_alternative<JSONNumber>(rhsValue)) {
            return std::get<JSONNumber>(lhsValue) < std::get<JSONNumber>(rhsValue);
        }
        else {
            throw JSONException("JSONObjects must hold JSONString or JSONNumber to use operator<");
        }
    }

    bool operator<=(const JSONObject& lhs, const JSONObject& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }

    bool operator>(const JSONObject& lhs, const JSONObject& rhs) {
        auto& lhsValue = lhs.value;
        auto& rhsValue = rhs.value;
        
        if (std::holds_alternative<JSONString>(lhsValue) && std::holds_alternative<JSONString>(rhsValue)) {
            return std::get<JSONString>(lhsValue) > std::get<JSONString>(rhsValue);
        }
        else if (std::holds_alternative<JSONNumber>(lhsValue) && std::holds_alternative<JSONNumber>(rhsValue)) {
            return std::get<JSONNumber>(lhsValue) > std::get<JSONNumber>(rhsValue);
        }
        else {
            throw JSONException("JSONObjects must hold JSONString or JSONNumber to use operator>");
        }
    }

    bool operator>=(const JSONObject& lhs, const JSONObject& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }
    
    std::string JSONObject::toString() const {
        if (std::holds_alternative<JSONString>(value)) {
            return std::get<JSONString>(value).toString();
        }
        else if (std::holds_alternative<JSONNumber>(value)) {
            return std::get<JSONNumber>(value).toString();
        }
        else if (std::holds_alternative<JSONBool>(value)) {
            return std::get<JSONBool>(value).toString();
        }
        else if (std::holds_alternative<JSONNull>(value)) {
            return std::get<JSONNull>(value).toString();
        }
        else if (std::holds_alternative<JSONArray>(value)) {
            return std::get<JSONArray>(value).toString();
        }
        else if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);
            
            if (map.size() == 0) {
                return "{}";
            }
            else {
                std::string res = "{";

                for (auto& [key, val] : map) {
                    res += key.toString() + ":" + val.toString() + ",";
                }

                res.back() = '}';
                
                return res;
            }
        }
        
        // should not get here
        throw JSONException("Error when converting JSONObject to string");
    }

    std::string JSONObject::toIndentedString(std::string& currentIndentation, const std::string& indentString) const {
        if (std::holds_alternative<JSONString>(value)) {
            return std::get<JSONString>(value).toString();
        }
        else if (std::holds_alternative<JSONNumber>(value)) {
            return std::get<JSONNumber>(value).toString();
        }
        else if (std::holds_alternative<JSONBool>(value)) {
            return std::get<JSONBool>(value).toString();
        }
        else if (std::holds_alternative<JSONNull>(value)) {
            return std::get<JSONNull>(value).toString();
        }
        else if (std::holds_alternative<JSONArray>(value)) {
            return std::get<JSONArray>(value).toIndentedString(currentIndentation, indentString);
        }
        else if (std::holds_alternative<std::map<JSONString, JSONObject>>(value)) {
            auto& map = std::get<std::map<JSONString, JSONObject>>(value);

            if (map.size() == 0) {
                return "{}";
            }
            else {
                std::string res = "{\n";

                currentIndentation += indentString;

                for (auto& [key, val] : map) {
                    res += currentIndentation + key.toString() + " : " + val.toIndentedString(currentIndentation, indentString) + ",\n";
                }

                currentIndentation.erase(currentIndentation.length() - indentString.length());

                res.erase(res.length() - 2);
                res += "\n" + currentIndentation + "}";
                
                return res;
            }
        }
        
        // should not get here
        throw JSONException("Error when converting JSONObject to string");
    }

} // namespace simpleJSON 

namespace internal {
    simpleJSON::JSONObject beginParseFromStream__internal(std::istream& stream) {
        FUNCTRACE

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
                // next read will fail and function will end
                break;
            default: 
                std::string errorMessage = "Error while parsing object, unexpected next character '" + std::string{next} + "'";
                throw simpleJSON::JSONException(errorMessage.c_str());
                break;
        }
   
        next = peekNextNonSpaceCharacter__internal(stream);

        if (next != std::istream::traits_type::eof()) {
            std::string errorMessage = "Error after reading a valid json object. Expected EOF but found '" + std::string{next} + "'"; 
            throw simpleJSON::JSONException(errorMessage.c_str());
        }
        
        return result;
    }

    NextJsonType detectNextType__internal(char nextCharInStream) {
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

    char peekNextNonSpaceCharacter__internal(std::istream& stream) {
        char next = stream.peek();
        
        while (isspace(next)) {
            stream.get();
            next = stream.peek();
        }

        return next;
    }

    simpleJSON::JSONFloating strToJSONFloating__internal(const std::string& str) {
        char* afterEnd;
        simpleJSON::JSONFloating result = std::strtold(str.c_str(), &afterEnd);

        if (*afterEnd != '\0') {
            std::string errorMessage = "Error while parsing number, invalid floating point number";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        return result;
    }

    simpleJSON::JSONIntegral strToJSONIntegral__internal(const std::string& str) {
        char* afterEnd;
        simpleJSON::JSONIntegral result = std::strtoll(str.c_str(), &afterEnd, 10);
            
        if (*afterEnd != '\0') {
            std::string errorMessage = "Error while parsing number, invalid signed integer";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        return result;
    }

    simpleJSON::JSONString parseString__internal(std::istream& stream) {
        FUNCTRACE

        char currentChar;
        stream.get(currentChar);

        if (currentChar != '"') {
            std::string errorMessage = "Error while parsing string, expected '\"'";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        std::string result;
        bool currentCharIsEscaped = false;

        while (stream) {
            stream.get(currentChar);

            if (currentChar == '"' && !currentCharIsEscaped) {
                return result;
            }
            
            if (currentChar == '\\' && !currentCharIsEscaped) {
                currentCharIsEscaped = true;
            }
            else if (currentCharIsEscaped) {
                currentCharIsEscaped = false;
            }
            
            result += currentChar;
        }

        // should not get here
        std::string errorMessage = "Error while parsing string, unexpected end of stream";
        throw simpleJSON::JSONException(errorMessage.c_str());
    }

    simpleJSON::JSONNumber parseNumber__internal(std::istream& stream) {
        FUNCTRACE

        char c = stream.peek();
        std::string numberAsString;

        short dotCount = 0;
        short eCount = 0;

        while (isdigit(c) || c == '.' || c == '-' || c == 'e' || c == 'E' || c == '+') {
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
        else {
            return simpleJSON::JSONNumber{internal::strToJSONIntegral__internal(numberAsString)};
        }
    }
    
    simpleJSON::JSONBool parseBool__internal(std::istream& stream) {
        FUNCTRACE

        char c1, c2, c3, c4;
        stream.get(c1);
        stream.get(c2);
        stream.get(c3);
        stream.get(c4);

        std::string out = {c1, c2, c3, c4};

        if (out == "true") {
            return simpleJSON::JSONBool(true);
        }
        else if (out[0] == 'f') {
            char c5;
            stream.get(c5);

            out += c5;

            if (out == "false") {
                return simpleJSON::JSONBool(false);
            } 
        }

        std::string errorMessage = "Error while parsing bool, expected \"true\" or \"false\", got \"" + out + "\"";
        throw simpleJSON::JSONException(errorMessage.c_str());
    }
    
    simpleJSON::JSONNull parseNull__internal(std::istream& stream) {
        FUNCTRACE

        char c1, c2, c3, c4;
        stream.get(c1);
        stream.get(c2);
        stream.get(c3);
        stream.get(c4);

        std::string out = {c1, c2, c3, c4};

        if (out != "null") {
            std::string errorMessage = "Error while parsing null, expected \"null\", got \"" + out + "\"";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        return simpleJSON::JSONNull{};
    }

    simpleJSON::JSONArray parseArray__internal(std::istream& stream) {
        FUNCTRACE

        char c;
        stream.get(c);

        if (c != '[') {
            std::string errorMessage = "Error while parsing array, expected '[', got '" + std::string{c} + "'";
            throw simpleJSON::JSONException(errorMessage.c_str());
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
                else {
                    throw simpleJSON::JSONException("Unexpected comma when parsing array");
                }
            }

            if (c == ']') {
                if (lastReadWasComma) {
                    throw simpleJSON::JSONException("Trailing comma not allowed in array");
                }
                else {
                    stream.get(c);
                    return result;
                }
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
                default: 
                    std::string errorMessage = "Error while parsing array, unexpected next character '" + std::string{c} + "'"; 
                    throw simpleJSON::JSONException(errorMessage.c_str());
                    break;
            }

            expectingComma = true;
            lastReadWasComma = false;
        }

        // should not get here
        std::string errorMessage = "Error while parsing array, unexpected end of stream";
        throw simpleJSON::JSONException(errorMessage.c_str());
    }

    simpleJSON::JSONObject parseObject__internal(std::istream& stream) {
        FUNCTRACE

        char next;
        stream.get(next);

        if (next != '{') {
            std::string errorMessage = "Error while parsing object, expected '{', got '" + std::string{next} + "'";
            throw simpleJSON::JSONException(errorMessage.c_str());
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
                std::string errorMessage = "Error while parsing object, expected '\"', got '" + std::string{next} + "'";
                throw simpleJSON::JSONException(errorMessage.c_str());
            }

            simpleJSON::JSONString key = parseString__internal(stream);

            // possible white space between map key and :
            next = peekNextNonSpaceCharacter__internal(stream);
            stream.get(next);

            // must read separator
            if (next != ':') {
                std::string errorMessage = "Error while parsing object, expected ':', got '" + std::string{next} + "'";
                throw simpleJSON::JSONException(errorMessage.c_str());
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
                default: 
                    std::string errorMessage = "Error while parsing object, unexpected next character '" + std::string{next} + "'"; 
                    throw simpleJSON::JSONException(errorMessage.c_str());
                    break;
            }

            //  possible white space between map value and , or }
            next = peekNextNonSpaceCharacter__internal(stream);
            stream.get(next);
            
            if (next == ',') {
                lastReadWasComma = true;
                continue;
            }
            else if (next == '}') {
                break;
            }
            else {
                std::string errorMessage = "Error while parsing object, unexpected next character '" + std::string{next} + "'"; 
                throw simpleJSON::JSONException(errorMessage.c_str());
            }
        }
        
        return result;
    }
} // namespace internal

#endif //__SIMPLE_JSON__