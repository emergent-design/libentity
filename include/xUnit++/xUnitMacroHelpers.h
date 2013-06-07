#ifndef XUNITMACROHELPERS_H_
#define XUNITMACROHELPERS_H_

#define XU_CAT2(a,b) a ## b
#define XU_CAT(a,b) XU_CAT2(a,b)

#define XU_UNIQUE_ATT_NS XU_CAT(TestAtt_, __LINE__)
#define XU_UNIQUE_NS XU_CAT(TestNs_, __LINE__)
#define XU_UNIQUE_FIXTURE XU_CAT(TestFixture_, __LINE__)
#define XU_UNIQUE_TEST XU_CAT(TestFn_, __LINE__)
#define XU_UNIQUE_RUNNER XU_CAT(TestRunner_, __LINE__)

// !!!VS fix when initializer lists are supported
#define XU_TEST_EVENTS \
namespace detail \
{ \
    std::shared_ptr<xUnitpp::TestEventRecorder> eventRecorders[] = { \
        /* check */ std::make_shared<xUnitpp::TestEventRecorder>(), \
        /* warn  */ std::make_shared<xUnitpp::TestEventRecorder>(), \
        /* log   */ std::make_shared<xUnitpp::TestEventRecorder>(), \
    }; \
    auto pCheck = std::make_shared<xUnitpp::Check>(*eventRecorders[0]); \
    auto pWarn = std::make_shared<xUnitpp::Warn>(*eventRecorders[1]); \
    auto pLog = std::make_shared<xUnitpp::Log>(*eventRecorders[2]); \
} \
std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> eventRecorders(std::begin(detail::eventRecorders), std::end(detail::eventRecorders)); \

// with thanks for various sources, but I got it from
// http://stackoverflow.com/questions/2308243/macro-returning-the-number-of-arguments-it-is-given-in-c

// Removed lines to remove warnings (Dan)

// ...and a little VS help from
// http://stackoverflow.com/questions/5530505/variadic-macro-argument-count-macro-fails-in-vs2010-but-works-on-ideone

#define EXPAND(x) x

#define PP_NARGS(...) \
    EXPAND(_xPP_NARGS_IMPL(__VA_ARGS__,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

#define _xPP_NARGS_IMPL(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,N,...) N

#define CAR(x, ...) x
#define CDR(x, ...) __VA_ARGS__

// attribute expansion helpers
#define XU_A_1(a) \
    attributes.insert(std::make_pair a);
#define XU_A_2(a, b) \
    XU_A_1(a) \
    XU_A_1(b)
#define XU_A_3(a, b, c) \
    XU_A_1(a) \
    XU_A_2(b, c)
#define XU_A_4(a, b, c, d) \
    XU_A_1(a) \
    XU_A_3(b, c, d)
#define XU_A_5(a, b, c, d, e) \
    XU_A_1(a) \
    XU_A_4(b, c, d, e)
#define XU_A_6(a, b, c, d, e, f) \
    XU_A_1(a) \
    XU_A_5(b, c, d, e, f)
#define XU_A_7(a, b, c, d, e, f, g) \
    XU_A_1(a) \
    XU_A_6(b, c, d, e, f, g)
#define XU_A_8(a, b, c, d, e, f, g, h) \
    XU_A_1(a) \
    XU_A_7(b, c, d, e, f, g, h)
#define XU_ATT(M, ...) M(__VA_ARGS__)
#define XU_ATTRIBUTES(...) XU_ATT(XU_CAT(XU_A_, PP_NARGS(__VA_ARGS__)), __VA_ARGS__)

#endif
