#include "lc_bool.h"
#include <assert.h>
#include <iostream>
#include <string>

inline void Assert(bool cond, const std::string& msg)
{
    if (!cond)
    {
        std::cout << "Assertion failed: " << msg << std::endl;
        assert(false);
    }

    std::cout << "Assertion passed: " << msg << std::endl;
}
