#pragma once

#include <unordered_map>
#include <vector>

#include "luse.h"

namespace rpplugins {

class JitterType: public std::unordered_map<std::string, std::vector<LVecBase2>>
{
public:
    JitterType(void);

private:
    /** Low discrepancy halton sequence with a given prime */
    float halton_seq(int prime, int index=1);
};

extern const JitterType JITTERS;

}    // namespace rpplugins
