// Single header file JSON emitter library, designed to
// be used as a Git submodule. This code is super evil! 
// This should be inside SatanLib because it's so evil!

// Author: The Devil <devil@hell.com>
// Author: Justus Languell <jus@justusl.com>

// Eventually, I am going to rewrite it to be even more
// better and even more evil! Reference for the future.
// http://coliru.stacked-crooked.com/a/a5902693e51b59b9


#ifndef _COMPILER_JSON_H_
#define _COMPILER_JSON_H_

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class JSON;

class JSONNode {
  public:
    virtual JSON* ToJSON(void) = 0;
};

#define __REGISTER_ADD_FUNC(T, E)                                                                             \
  private:                                                                                         \
    inline std::string _encode(T x) {                                                                     \
        return E;                                                                                  \
    }                                                                                              \
                                                                                                   \
  public:                                                                                          \
    inline JSON* Add(const std::string& k, T v) {                                                         \
        object.insert({k, _encode(v)});                                                            \
        return this;                                                                               \
    }                                                                                              \
    inline JSON* Add(const std::string& k, std::vector<T> v) {                                            \
        std::string a = "[";                                                                       \
        for (T l : v) a += _encode(l) + ",";                                                       \
        a.pop_back();                                                                              \
        object.insert({k, a + "]"});                                                               \
        return this;                                                                               \
    }

class JSON {
  public:
    inline static JSON* Node(const std::string& type) {
        JSON* json = new JSON();
        json->Add("__type__", type);
        return json;
    }

  private:
    inline std::string _encode(JSONNode* x) {
        return x->ToJSON()->ToStr();
    }

  public:
    template<typename T, typename std::enable_if<std::is_base_of<JSONNode, T>::value>::type* = nullptr>
    inline JSON* Add(const std::string& k, JSONNode* v) {
        object.insert({k, _encode(v)});
        return this;
    }
    template<typename T, typename std::enable_if<std::is_base_of<JSONNode, T>::value>::type* = nullptr>
    inline JSON* Add(const std::string& k, std::vector<T*> v) {
        std::string a = "[";
        for (T* l : v) a += _encode(l) + ",";
        a.pop_back();
        object.insert({k, a + "]"});
        return this;
    }

    // These functions are setup with macros because I am evil.
    __REGISTER_ADD_FUNC(std::string, "\"" + x + "\"")
    __REGISTER_ADD_FUNC(int, std::to_string(x))
    __REGISTER_ADD_FUNC(long, std::to_string(x))
    __REGISTER_ADD_FUNC(float, std::to_string(x))
    __REGISTER_ADD_FUNC(double, std::to_string(x))
    __REGISTER_ADD_FUNC(bool, x ? "true" : "false")

    std::string ToStr(void) {
        std::string result_str = "{";
        for (const auto line : object) {
            result_str += _encode(line.first) + ":" + line.second + ",";
        }
        result_str.pop_back();
        return result_str + "}";
    }

  private:
    std::map<std::string, std::string> object;
};

// Evil macro! This is as evil as macros get.
#define FIELD(var_name) Add(#var_name, var_name)

// Nah, these are even more evil!
#define JSON_DECL JSON* ToJSON(void) override;
#define JSON_VIRTUAL virtual JSON* ToJSON(void) override;
#define JSON_IMPL(T, S)                                                                            \
    JSON* T::ToJSON(void) {                                                                        \
        return JSON::Node(#T) S;                                                                   \
    }

#endif
