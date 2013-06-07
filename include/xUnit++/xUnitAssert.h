#ifndef XUNITASSERT_H_
#define XUNITASSERT_H_

#if defined(_MSC_VER)
# if !defined(_ALLOW_KEYWORD_MACROS)
#  define _ALLOW_KEYWORD_MACROS
# endif
#define noexcept(x)
#endif

#include <algorithm>
#include <exception>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "LineInfo.h"
#include "xUnitToString.h"

namespace xUnitpp
{

class xUnitAssert
{
public:
    xUnitAssert(std::string &&call, LineInfo &&lineInfo);

    xUnitAssert &CustomMessage(std::string &&message);
    xUnitAssert &Expected(std::string &&expected);
    xUnitAssert &Actual(std::string &&actual);

    template<typename T>
    xUnitAssert &AppendUserMessage(T &&value)
    {
        *userMessage << ToString(std::forward<T>(value));
        return *this;
    }

    const std::string &Call() const;
    std::string UserMessage() const;
    const std::string &CustomMessage() const;
    const std::string &Expected() const;
    const std::string &Actual() const;

    const xUnitpp::LineInfo &LineInfo() const;

    static xUnitAssert None();

private:
    xUnitpp::LineInfo lineInfo;
    std::string call;
    std::string customMessage;
    std::string expected;
    std::string actual;
    std::shared_ptr<std::stringstream> userMessage;
};

class xUnitFailure
{
private:
    xUnitFailure();

public:
    xUnitFailure(xUnitAssert &&assert, std::function<void(const xUnitAssert &)> onFailureComplete);
    xUnitFailure(const xUnitFailure &other);

#if !defined(_MSC_VER) // !!!VS remove the #if/#endif when VS can compile this code
    xUnitFailure(xUnitFailure &&) = default;
#endif

    ~xUnitFailure() noexcept(false);

    static xUnitFailure None();

    template<typename T>
    xUnitFailure &operator <<(T &&value)
    {
        assert.AppendUserMessage(std::forward<T>(value));
        return *this;
    }

    template<typename T>
    xUnitFailure &operator <<(const T &value)
    {
        assert.AppendUserMessage(value);
        return *this;
    }

private:
    xUnitFailure &operator =(xUnitFailure other) /* = delete */;

private:
    std::function<void(const xUnitAssert &)> OnFailureComplete;

    xUnitAssert assert;
    int &refCount;
};

class Assert
{
protected:
    static double round(double value, size_t precision);

    template<typename T>
    static std::string RangeToString(T begin, T end)
    {
        typedef decltype(*begin) val_type;

        std::string result = "[ ";

        std::for_each(std::forward<T>(begin), std::forward<T>(end), [&result](val_type val) { result += ToString(std::forward<val_type>(val)) + ", "; });

        result[result.size() - 2] = ' ';
        result[result.size() - 1] = ']';

        return result;
    }

    template<typename T>
    struct has_empty
    {
    private:
        template<typename U, U>
        class check {};

        template<typename C>
        static char f(check<bool (C::*)() const, &C::empty> *);

        template<typename C>
        static long f(...);

    public:
        static const bool value = (sizeof(f<T>(nullptr)) == sizeof(char));
    };

    xUnitFailure OnFailure(xUnitAssert &&assert) const;
    xUnitFailure OnSuccess() const;

    std::string callPrefix;
    std::function<void (const xUnitAssert &)> handleFailure;

public:
    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure Equal(TExpected expected, TActual actual, TComparer &&comparer, LineInfo &&lineInfo = LineInfo()) const
    {
        if (!comparer(std::forward<TExpected>(expected), std::forward<TActual>(actual)))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Equal", std::move(lineInfo))
                .Expected(ToString(std::forward<TExpected>(expected)))
                .Actual(ToString(std::forward<TActual>(actual)))));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    typename std::enable_if<
        !std::is_constructible<std::string, TExpected>::value || !std::is_constructible<std::string, TActual>::value,
        xUnitFailure>::type Equal(TExpected expected, TActual actual, LineInfo &&lineInfo = LineInfo()) const
    {
        return Equal(std::forward<TExpected>(expected), std::forward<TActual>(actual), [](TExpected &&expected, TActual &&actual) { return expected == actual; }, std::move(lineInfo));
    }

    template<typename TExpected, typename TActual>
    typename std::enable_if<
        std::is_constructible<std::string, TExpected>::value && std::is_constructible<std::string, TActual>::value,
        xUnitFailure>::type Equal(TExpected expected, TActual actual, LineInfo &&lineInfo = LineInfo()) const
    {
        return Equal(std::string(std::forward<TExpected>(expected)), std::string(std::forward<TActual>(actual)), std::move(lineInfo));
    }

    xUnitFailure Equal(const std::string &expected, const std::string &actual, LineInfo &&lineInfo = LineInfo()) const;

    xUnitFailure Equal(float expected, float actual, int precision, LineInfo &&lineInfo = LineInfo()) const;
    xUnitFailure Equal(double expected, double actual, int precision, LineInfo &&lineInfo = LineInfo()) const;

    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure Equal(TExpected &&expectedBegin, TExpected &&expectedEnd, TActual &&actualBegin, TActual &&actualEnd, TComparer &&comparer, LineInfo &&lineInfo = LineInfo()) const
    {
        auto expected = expectedBegin;
        auto actual = actualBegin;

        size_t index = 0;
        while (expected != expectedEnd && actual != actualEnd)
        {
            if (!comparer(*expected, *actual))
            {
                break;
            }

            ++expected;
            ++actual;
            ++index;
        }

        if (expected != expectedEnd || actual != actualEnd)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Equal", std::move(lineInfo))
                .CustomMessage("Sequence unequal at location " + ToString(index) + ".")
                .Expected(RangeToString(std::forward<TExpected>(expectedBegin), std::forward<TExpected>(expectedEnd)))
                .Actual(RangeToString(std::forward<TActual>(actualBegin), std::forward<TActual>(actualEnd)))));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    xUnitFailure Equal(TExpected &&expectedBegin, TExpected &&expectedEnd, TActual &&actualBegin, TActual &&actualEnd, LineInfo &&lineInfo = LineInfo()) const
    {
        return Equal(std::forward<TExpected>(expectedBegin), std::forward<TExpected>(expectedEnd),
            std::forward<TActual>(actualBegin), std::forward<TActual>(actualEnd),
            [](decltype(*expectedBegin) &&a, decltype(*actualBegin) &&b) { return a == b; }, std::move(lineInfo));
    }

    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure NotEqual(TExpected expected, TActual actual, TComparer &&comparer, LineInfo &&lineInfo = LineInfo()) const
    {
        if (comparer(std::forward<TExpected>(expected), std::forward<TActual>(actual)))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotEqual", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    typename std::enable_if<
        !std::is_constructible<std::string, TExpected>::value || !std::is_constructible<std::string, TActual>::value,
        xUnitFailure>::type NotEqual(TExpected expected, TActual actual, LineInfo &&lineInfo = LineInfo()) const
    {
        return NotEqual(std::forward<TExpected>(expected), std::forward<TActual>(actual), [](TExpected &&expected, TActual &&actual) { return expected == actual; }, std::move(lineInfo));
    }

    template<typename TExpected, typename TActual>
    typename std::enable_if<
        std::is_constructible<std::string, TExpected>::value && std::is_constructible<std::string, TActual>::value,
        xUnitFailure>::type NotEqual(TExpected expected, TActual actual, LineInfo &&lineInfo = LineInfo()) const
    {
        return NotEqual(std::string(std::forward<TExpected>(expected)), std::string(std::forward<TActual>(actual)), std::move(lineInfo));
    }

    xUnitFailure NotEqual(const std::string &expected, const std::string &actual, LineInfo &&lineInfo = LineInfo()) const;

    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure NotEqual(TExpected &&expectedBegin, TExpected &&expectedEnd, TActual &&actualBegin, TActual &&actualEnd, TComparer &&comparer, LineInfo &&lineInfo = LineInfo()) const
    {
        auto expected = expectedBegin;
        auto actual = actualBegin;

        while (expected != expectedEnd && actual != actualEnd)
        {
            if (!comparer(*expected, *actual))
            {
                break;
            }

            ++expected;
            ++actual;
        }

        if (expected == expectedEnd && actual == actualEnd)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotEqual", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    xUnitFailure NotEqual(TExpected &&expectedBegin, TExpected &&expectedEnd, TActual &&actualBegin, TActual &&actualEnd, LineInfo &&lineInfo = LineInfo()) const
    {
        return NotEqual(std::forward<TExpected>(expectedBegin), std::forward<TExpected>(expectedEnd),
            std::forward<TActual>(actualBegin), std::forward<TActual>(actualEnd),
            [](decltype(*expectedBegin) &&a, decltype(*actualBegin) &&b) { return a == b; }, std::move(lineInfo));
    }


    template<typename TFunc>
    xUnitFailure DoesNotThrow(TFunc &&fn, LineInfo &&lineInfo = LineInfo()) const
    {
        try
        {
            fn();
        }
        catch (const std::exception &e)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "DoesNotThrow", std::move(lineInfo))
                .Expected("(no exception)")
                .Actual(e.what())));
        }
        catch (...)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "DoesNotThrow", std::move(lineInfo))
                .Expected("(no exception)")
                .Actual("Crash: unknown exception.")));
        }

        return OnSuccess();
    }

    xUnitFailure Fail(LineInfo &&lineInfo = LineInfo()) const;

    xUnitFailure False(bool b, LineInfo &&lineInfo = LineInfo()) const;

    xUnitFailure True(bool b, LineInfo &&lineInfo = LineInfo()) const;

    template<typename TSequence>
    typename std::enable_if<has_empty<typename std::remove_reference<TSequence>::type>::value, xUnitFailure>::type Empty(TSequence &&sequence, LineInfo &&lineInfo = LineInfo()) const
    {
        if (!sequence.empty())
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Empty", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<typename std::remove_reference<TSequence>::type>::value, xUnitFailure>::type Empty(TSequence &&sequence, LineInfo &&lineInfo = LineInfo()) const
    {
        using std::begin;
        using std::end;

        if (begin(std::forward<TSequence>(sequence)) != end(std::forward<TSequence>(sequence)))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Empty", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TSequence>
    typename std::enable_if<has_empty<TSequence>::value, xUnitFailure>::type NotEmpty(TSequence &&sequence, LineInfo &&lineInfo = LineInfo()) const
    {
        if (sequence.empty())
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotEmpty", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<TSequence>::value, xUnitFailure>::type NotEmpty(TSequence &&sequence, LineInfo &&lineInfo = LineInfo()) const
    {
        using std::begin;
        using std::end;

        if (begin(std::forward<TSequence>(sequence)) == end(std::forward<TSequence>(sequence)))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotEmpty", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TSequence, typename TPredicate>
    xUnitFailure DoesNotContainPred(const TSequence &sequence, TPredicate &&predicate, LineInfo &&lineInfo = LineInfo()) const
    {
        using std::begin;
        using std::end;

        auto found = std::find_if(begin(sequence), end(sequence), std::forward<TPredicate>(predicate));
        if (found != end(std::forward<TSequence>(sequence)))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "DoesNotContain", std::move(lineInfo))
                .CustomMessage("Found: matching value at position " + ToString(std::distance(begin(sequence), found)) + ".")));
        }

        return OnSuccess();
    }

    template<typename TSequence, typename T>
    typename std::enable_if<
        !std::is_constructible<std::string, TSequence>::value || !std::is_constructible<std::string, T>::value,
        xUnitFailure
    >::type DoesNotContain(const TSequence &sequence, T &&value, LineInfo &&lineInfo = LineInfo()) const
    {
        using std::begin;
        using std::end;

        if (std::find(begin(sequence), end(sequence), std::forward<T>(value)) != end(sequence))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "DoesNotContain", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TActualString, typename TValueString>
    typename std::enable_if<
        std::is_constructible<std::string, TActualString>::value && std::is_constructible<std::string, TValueString>::value,
        xUnitFailure
    >::type DoesNotContain(TActualString actualString, TValueString value, LineInfo &&lineInfo = LineInfo()) const
    {
        return DoesNotContain(std::string(std::forward<TActualString>(actualString)), std::string(std::forward<TValueString>(value)), std::move(lineInfo));
    }

    xUnitFailure DoesNotContain(const std::string &actualString, const std::string &value, LineInfo &&lineInfo = LineInfo()) const;

    template<typename TSequence, typename TPredicate>
    xUnitFailure ContainsPred(const TSequence &sequence, TPredicate &&predicate, LineInfo &&lineInfo = LineInfo()) const
    {
        using std::begin;
        using std::end;

        if (std::find_if(begin(sequence), end(sequence), std::forward<TPredicate>(predicate)) == end(sequence))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Contains", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TSequence, typename T>
    typename std::enable_if<
        !std::is_constructible<std::string, TSequence>::value || !std::is_constructible<std::string, T>::value,
        xUnitFailure
    >::type Contains(const TSequence &sequence, T &&value, LineInfo &&lineInfo = LineInfo()) const
    {
        using std::begin;
        using std::end;

        if (std::find(begin(sequence), end(sequence), std::forward<T>(value)) == end(sequence))
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Contains", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename TActualString, typename TValueString>
    typename std::enable_if<
        std::is_constructible<std::string, TActualString>::value && std::is_constructible<std::string, TValueString>::value,
        xUnitFailure
    >::type Contains(TActualString actualString, TValueString value, LineInfo &&lineInfo = LineInfo()) const
    {
        return Contains(std::string(std::forward<TActualString>(actualString)), std::string(std::forward<TValueString>(value)), std::move(lineInfo));
    }

    xUnitFailure Contains(const std::string &actualString, const std::string &value, LineInfo &&lineInfo = LineInfo()) const;

    template<typename TActual, typename TRange>
    xUnitFailure InRange(TActual &&actual, TRange &&min, TRange &&max, LineInfo &&lineInfo = LineInfo()) const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.InRange argument error: min (" + ToString(std::forward<TRange>(min)) + ") must be strictly less than max (" + ToString(std::forward<TRange>(max)) + ").");
        }

        if (actual < min || actual >= max)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "InRange", std::move(lineInfo))
                .Expected("[" + ToString(std::forward<TRange>(min)) + " - " + ToString(std::forward<TRange>(max)) + ")")
                .Actual(ToString(std::forward<TActual>(actual)))));
        }

        return OnSuccess();
    }

    template<typename TActual, typename TRange>
    xUnitFailure NotInRange(TActual &&actual, TRange &&min, TRange &&max, LineInfo &&lineInfo = LineInfo()) const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.NotInRange argument error: min (" + ToString(std::forward<TRange>(min)) + ") must be strictly less than max (" + ToString(std::forward<TRange>(max)) + ").");
        }

        if (actual >= min && actual < max)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotInRange", std::move(lineInfo))
                .Expected("[" + ToString(std::forward<TRange>(min)) + " - " + ToString(std::forward<TRange>(max)) + ")")
                .Actual(ToString(std::forward<TActual>(actual)))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure NotNull(T &&value, LineInfo &&lineInfo = LineInfo()) const
    {
        if (value == nullptr)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotNull", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Null(T &&value, LineInfo &&lineInfo = LineInfo()) const
    {
        if (value != nullptr)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Null", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure NotSame(const T &expected, const T &actual, LineInfo &&lineInfo = LineInfo()) const
    {
        if (&expected == &actual)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotSame", std::move(lineInfo))));
        }

        return OnSuccess();
    }


    template<typename T>
    xUnitFailure NotSame(const T *expected, const T *actual, LineInfo &&lineInfo = LineInfo()) const
    {
        if (expected == actual)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotSame", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure NotSame(T *expected, T *actual, LineInfo &&lineInfo = LineInfo()) const
    {
        if (expected == actual)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "NotSame", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Same(const T &expected, const T &actual, LineInfo &&lineInfo = LineInfo()) const
    {
        if (&expected != &actual)
        {
            return OnFailure(std::move(xUnitAssert(callPrefix + "Same", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Same(T *expected, T *actual, LineInfo &&lineInfo = LineInfo()) const
    {
        if (expected != actual)
        {
            return OnFailure(std::move(xUnitAssert("Same", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Same(const T *expected, const T *actual, LineInfo &&lineInfo = LineInfo()) const
    {
        if (expected != actual)
        {
            return OnFailure(std::move(xUnitAssert("Same", std::move(lineInfo))));
        }

        return OnSuccess();
    }

    Assert(std::string &&callPrefix = "Assert.",
           std::function<void (const xUnitAssert &)> &&onFailure = [](const xUnitAssert &assert) { throw assert; });
};

const class : public Assert
{
private:
    // Fixes #10: can't Assert.Throws<std::exception> because std::exception is then caught twice
    // with thanks to Alf on StackOverflow for this one
    // http://stackoverflow.com/questions/5101516/why-function-template-cannot-be-partially-specialized
    template<typename TException, typename TFunc>
    struct ThrowsImpl
    {
        static TException impl(const std::string &callPrefix, TFunc &&fn, const std::string &msg, LineInfo &&lineInfo = LineInfo())
        {
            try
            {
                fn();
            }
            catch (const TException &e)
            {
                return e;
            }
            catch (const std::exception &e)
            {
                throw xUnitAssert(callPrefix + "Throws", std::move(lineInfo))
                    .Expected(typeid(TException).name())
                    .Actual(e.what())
                    .AppendUserMessage(msg);
            }
            catch (...)
            {
                throw xUnitAssert(callPrefix + "Throws", std::move(lineInfo))
                    .Expected(typeid(TException).name())
                    .Actual("Crash: unknown exception.")
                    .AppendUserMessage(msg);
            }

            throw xUnitAssert(callPrefix + "Throws", std::move(lineInfo))
                .Expected(typeid(TException).name())
                .Actual("No exception.")
                .AppendUserMessage(msg);
        }
    };

    // partial specialization for catching std::exception
    template<typename TFunc>
    struct ThrowsImpl<std::exception, TFunc>
    {
        static std::exception impl(const std::string &callPrefix, TFunc &&fn, const std::string &msg, LineInfo &&lineInfo = LineInfo())
        {
            try
            {
                fn();
            }
            catch (const std::exception &e)
            {
                return e;
            }
            catch (...)
            {
                throw xUnitAssert(callPrefix + "Throws", std::move(lineInfo))
                    .Expected(typeid(std::exception).name())
                    .Actual("Crash: unknown exception.")
                    .AppendUserMessage(msg);
            }

            throw xUnitAssert(callPrefix + "Throws", std::move(lineInfo))
                .Expected(typeid(std::exception).name())
                .Actual("No exception.")
                .AppendUserMessage(msg);
        }
    };

public:
    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn, const std::string &msg, LineInfo &&lineInfo = LineInfo()) const
    {
        return ThrowsImpl<TException, TFunc>::impl(callPrefix, std::forward<TFunc>(fn), msg, std::move(lineInfo));
    }

    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn, LineInfo &&lineInfo = LineInfo()) const
    {
        return Throws<TException>(std::forward<TFunc>(fn), "", std::move(lineInfo));
    }
} Assert
#if !defined(_MSC_VER) // !!!VS remove the #if/#endif when VS can compile this code
    = {}    // constant instance of class requires user-defined default constructor, or initializer list
 #endif
 ;

}

#endif
