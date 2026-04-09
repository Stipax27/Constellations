#ifndef _CLASSNAME_H_
#define _CLASSNAME_H_

#include <string>
#include <typeinfo>
#include <iostream>

#ifdef __GNUG__
#include <cxxabi.h>

template<typename T>
std::string class_name() {
    const char* name = typeid(T).name();
    int status;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = (status == 0) ? demangled : name;
    free(demangled);
    return result;
}
#elif _MSC_VER
template<typename T>
std::string class_name() {
    std::string name = typeid(T).name();
    size_t space = name.find(' ');
    return (space != std::string::npos) ? name.substr(space + 1) : name;
}
#endif

#endif