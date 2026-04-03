#pragma once

#ifndef EASTL_UTILS_H
#define EASTL_UTILS_H

#ifdef _WIN32
#define CDeclFunction __cdecl
#else
#define CDeclFunction 
#endif

void * CDeclFunction operator new[](size_t size, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    (void)name;
    (void)flags;
    (void)debugFlags;
    (void)file;
    (void)line;
    return new uint8_t[size];
}

void * CDeclFunction operator new[](size_t size, size_t alignement, size_t offset, const char * name, int flags, unsigned debugFlags, const char * file, int line)
{
    (void)name;
    (void)flags;
    (void)debugFlags;
    (void)file;
    (void)line;
    (void)alignement;
    (void)offset;
    return new uint8_t[size];
}

//int CDeclFunction EA::StdC::Vsnprintf(char * __restrict pDestination, unsigned __int64 n, char const * __restrict pFormat, char * arguments)
//{
//    return vsnprintf(pDestination, n, pFormat, arguments);
//}

int CDeclFunction EA::StdC::Vsnprintf(char*  EA_RESTRICT pDestination, size_t n, const char*  EA_RESTRICT pFormat, va_list arguments)
{
    return vsnprintf(pDestination, n, pFormat, arguments);
}

#include <iosfwd>
#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <EASTL/string.h>

namespace eastl
{
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const eastl::vector<T>& vec)
    {
        for(const auto& elem : vec)
            os << elem;
        return os;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const eastl::list<T>& vec)
    {
        for(const auto& elem : vec)
            os << elem;
        return os;
    }
     //TODO: is this needed?   
    //std::ostream& operator<<(std::ostream& os, const eastl::string& str)
    //{
    //    return os << str.c_str();
    //}
    //TODO: doesn't work very well I guess...
    std::istream& operator>>(std::istream& is, eastl::string& mystring)
    {
        const auto bufSize = 256 * 10;
        char buff[bufSize];

        // getting the string from the stream
        is.get(buff, bufSize, '\n');

        // copying buff into myString
        mystring = eastl::string(buff);

        // clearing the stream buffer
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        return is;
    }
}

#endif