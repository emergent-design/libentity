#ifndef LINEINFO_H_
#define LINEINFO_H_

#include <string>

namespace xUnitpp
{

struct LineInfo
{
    LineInfo();
    LineInfo(std::string &&file, int line);

    std::string file;
    int line;

    friend std::string to_string(const LineInfo &lineInfo);
};

}

#endif
