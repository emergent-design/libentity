#ifndef ITESTDETAILS_H_
#define ITESTDETAILS_H_

// !!!VS remove the #if/#endif when VS can compile this code
#if defined(_MSC_VER)
# define DEFAULT {}
#else
# define DEFAULT = default;
#endif

namespace xUnitpp
{

struct ITestDetails
{
protected:
    virtual ~ITestDetails() DEFAULT

public:
    // These are explicitly NOT returning C++ classes, as they will cross dll boundaries.
    // If one side is Debug and the other Release, or if they are different compilers,
    // bad things will happen.
    virtual int __stdcall GetId() const = 0;
    virtual const char * __stdcall GetName() const = 0;
    virtual const char * __stdcall GetFullName() const = 0;
    virtual const char * __stdcall GetSuite() const = 0;
    virtual const char * __stdcall GetParams() const = 0;
    virtual int __stdcall GetTestInstance() const = 0;
    virtual size_t __stdcall GetAttributeCount() const = 0;
    virtual const char * __stdcall GetAttributeKey(size_t index) const = 0;
    virtual const char * __stdcall GetAttributeValue(size_t index) const = 0;
    virtual void __stdcall FindAttributeKey(const char *key, size_t &begin, size_t &end) const = 0;
    virtual const char * __stdcall GetFile() const = 0;
    virtual int __stdcall GetLine() const = 0;
};

}

#endif
