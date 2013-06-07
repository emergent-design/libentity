#ifndef XUNITMACROS_H_
#define XUNITMACROS_H_

#include <memory>
#include <tuple>
#include <vector>
#include "Attributes.h"
#include "LineInfo.h"
#include "TestCollection.h"
#include "TestEventRecorder.h"
#include "Suite.h"
#include "xUnitCheck.h"
#include "xUnitLog.h"
#include "xUnitMacroHelpers.h"
#include "xUnitWarn.h"

// !!!VS initializer lists can make this faster...
#define ATTRIBUTES(...) \
    namespace XU_UNIQUE_ATT_NS { \
        namespace xUnitAttributes { \
            inline xUnitpp::AttributeCollection Attributes() \
            { \
                xUnitpp::AttributeCollection attributes; \
                XU_ATTRIBUTES(__VA_ARGS__) \
                attributes.sort(); \
                return attributes; \
            } \
        } \
    } \
    namespace XU_UNIQUE_ATT_NS

#define SKIP(reason) \
    namespace XU_UNIQUE_ATT_NS { \
        namespace xUnitAttributes { \
            inline xUnitpp::AttributeCollection Attributes() \
            { \
                xUnitpp::AttributeCollection attributes; \
                XU_ATTRIBUTES(("Skip", reason)) \
                attributes.sort(); \
                return attributes; \
            } \
        } \
    } \
    namespace XU_UNIQUE_ATT_NS

#define SUITE(SuiteName) \
    namespace { \
        namespace xUnitSuite { \
            inline const std::string &Name() \
            { \
                static std::string name = SuiteName; \
                return name; \
            } \
        } \
    } \
    namespace

namespace xUnitpp { struct NoFixture {}; }

#define TIMED_FACT_FIXTURE(FactDetails, FixtureType, timeout) \
    namespace XU_UNIQUE_NS { \
        using xUnitpp::Assert; \
        XU_TEST_EVENTS \
        class XU_UNIQUE_FIXTURE : public FixtureType \
        { \
            /* !!!VS fix when '= delete' is supported */ \
            XU_UNIQUE_FIXTURE &operator =(XU_UNIQUE_FIXTURE) /* = delete */; \
        public: \
            XU_UNIQUE_FIXTURE() \
                : Check(*detail::pCheck) \
                , Warn(*detail::pWarn) \
                , Log(*detail::pLog) \
                { } \
            void XU_UNIQUE_TEST(); \
            const xUnitpp::Check &Check; \
            const xUnitpp::Warn &Warn; \
            const xUnitpp::Log &Log; \
        }; \
        void XU_UNIQUE_RUNNER() { XU_UNIQUE_FIXTURE().XU_UNIQUE_TEST(); } \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), \
            &XU_UNIQUE_RUNNER, std::string(FactDetails), xUnitSuite::Name(), \
            xUnitAttributes::Attributes(), timeout, std::string(__FILE__), __LINE__, std::move(eventRecorders)); \
    } \
    void XU_UNIQUE_NS :: XU_UNIQUE_FIXTURE :: XU_UNIQUE_TEST()

#define UNTIMED_FACT_FIXTURE(FactDetails, FixtureType) TIMED_FACT_FIXTURE(FactDetails, FixtureType, 0)

#define FACT_FIXTURE(FactDetails, FixtureType) TIMED_FACT_FIXTURE(FactDetails, FixtureType, -1)

#define TIMED_FACT(FactDetails, timeout) TIMED_FACT_FIXTURE(FactDetails, xUnitpp::NoFixture, timeout)

#define UNTIMED_FACT(FactDetails) TIMED_FACT_FIXTURE(FactDetails, xUnitpp::NoFixture, 0)

#define FACT(FactDetails) TIMED_FACT_FIXTURE(FactDetails, xUnitpp::NoFixture, -1)

#define TIMED_DATA_THEORY(TheoryDetails, params, DataProvider, timeout) \
    namespace XU_UNIQUE_NS { \
        using xUnitpp::Assert; \
        XU_TEST_EVENTS \
        const xUnitpp::Check &Check = *detail::pCheck; \
        const xUnitpp::Warn &Warn = *detail::pWarn; \
        const xUnitpp::Log &Log = *detail::pLog; \
        void XU_UNIQUE_TEST params; \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), \
            &XU_UNIQUE_TEST, DataProvider, std::string(TheoryDetails), xUnitSuite::Name(), std::string(#params), \
            xUnitAttributes::Attributes(), timeout, std::string(__FILE__), __LINE__, eventRecorders); \
    } \
    void XU_UNIQUE_NS :: XU_UNIQUE_TEST params

#define UNTIMED_DATA_THEORY(TheoryDetails, params, DataProvider, timeout) TIMED_DATA_THEORY(TheoryDetails, params, DataProvider, 0)

#define DATA_THEORY(TheoryDetails, params, DataProvider) TIMED_DATA_THEORY(TheoryDetails, params, DataProvider, -1)

#define TIMED_THEORY(TheoryDetails, params, timeout, ...) \
    namespace XU_UNIQUE_NS { \
        using xUnitpp::Assert; \
        XU_TEST_EVENTS \
        const xUnitpp::Check &Check = *detail::pCheck; \
        const xUnitpp::Warn &Warn = *detail::pWarn; \
        const xUnitpp::Log &Log = *detail::pLog; \
        void XU_UNIQUE_TEST params; \
        decltype(CAR(__VA_ARGS__)) args[] = { __VA_ARGS__ }; \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), \
            &XU_UNIQUE_TEST, xUnitpp::TheoryData(PP_NARGS(__VA_ARGS__), args), std::string(TheoryDetails), \
            xUnitSuite::Name(), std::string(#params), xUnitAttributes::Attributes(), timeout, std::string(__FILE__), __LINE__, eventRecorders); \
    } \
    void XU_UNIQUE_NS :: XU_UNIQUE_TEST params

#define UNTIMED_THEORY(TheoryDetails, params, ...) TIMED_THEORY(TheoryDetails, params, 0, __VA_ARGS__)

#define THEORY(TheoryDetails, params, ...) TIMED_THEORY(TheoryDetails, params, -1, __VA_ARGS__)

#define LI xUnitpp::LineInfo(std::string(__FILE__), __LINE__)

#endif