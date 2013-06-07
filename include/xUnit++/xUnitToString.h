#ifndef XUNITTOSTRING_H_
#define XUNITTOSTRING_H_

// from:
// http://stackoverflow.com/questions/1386183/how-to-call-a-templated-function-if-it-exists-and-something-else-otherwise
// modified to fix Visual Studio warning C4913

#include <string>

namespace xUnitpp
{

namespace ToStringImpl
{
    namespace fallback
    {
        struct flag { };

        flag to_string(...);

        // char would normally be treated like an int, which is probably not what we want
        inline std::string to_string(char c)
        {
            return std::string() + c;
        }

        // you can't normally call std::to_string with a std::string
        inline std::string to_string(std::string s)
        {
            return s;
        }

        long operator,(flag, flag);

        template<typename T>
        flag operator,(flag, T &&);

        char operator,(long, flag);
    }

    template<bool>
    struct to_string_impl;

    template<>
    struct to_string_impl<true>
    {
        template<typename T>
        static std::string to_string(const T &t)
        {
            using fallback::to_string;
            using std::to_string;

            return to_string(t);
        }
    };

    template<>
    struct to_string_impl<false>
    {
        template<typename T>
        static std::string to_string(T &&)
        {
            // going to truncate the type if it is longer than 20 characters
            std::string type = typeid(T).name();

            return type.size() > 20 ?
                type.substr(0, 20) + "..." :
                type;
        }
    };
}

template <typename T>
std::string ToString(T &&t)
{
    using ToStringImpl::fallback::to_string;
    using std::to_string;

    return ToStringImpl::to_string_impl<(sizeof (ToStringImpl::fallback::flag(), to_string(std::declval<T>()), ToStringImpl::fallback::flag()) != sizeof(char))>::to_string(t);
}

template<typename T>
bool has_to_string(T &&)
{
    return has_to_string<typename std::remove_reference<T>::type>();
}

// !!!VS this could be a constexpr
// if it's a constexpr, then the repeated code in ToString could be simplified
template<typename T>
bool has_to_string()
{
    using ToStringImpl::fallback::to_string;
    using std::to_string;

    return (sizeof (ToStringImpl::fallback::flag(), to_string(std::declval<T>()), ToStringImpl::fallback::flag()) != sizeof(char));
}

}

#endif
