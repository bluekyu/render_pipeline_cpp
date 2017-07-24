// Jitters from
// https://github.com/playdeadgames/temporal/blob/master/Assets/Scripts/FrustumJitter.cs

#include "jitters.hpp"

namespace rpplugins {

const JitterType JITTERS;

JitterType::JitterType(void)
{
    insert({

        { "still", {
            LVecBase2(0.0, 0.0)
        }},

        { "uniform2", {
            LVecBase2(-0.25, -0.25), // ll
            LVecBase2(0.25,  0.25),  // ur
        }},

        { "uniform4", {
            LVecBase2(-0.25, -0.25), // ll
            LVecBase2(0.25, -0.25),  // lr
            LVecBase2(0.25,  0.25),  // ur
            LVecBase2(-0.25,  0.25), // ul
        }},

        { "uniform4_helix", {
            LVecBase2(-0.25, -0.25),    // ll | 3  1 |
            LVecBase2(0.25,  0.25),     // ur |  \/| |
            LVecBase2(0.25, -0.25),     // lr |  /\| |
            LVecBase2(-0.25,  0.25),    // ul | 0  2 |
        }},

        { "uniform4_double_helix", {
            LVecBase2(-0.25, -0.25),    // ll  | 3  1 |
            LVecBase2(0.25,  0.25),     // ur  |  \/| |
            LVecBase2(0.25, -0.25),     // lr  |  /\| |
            LVecBase2(-0.25,  0.25),    // ul  | 0  2 |
            LVecBase2(-0.25, -0.25),    // ll  | 6--7 |
            LVecBase2(0.25, -0.25),     // lr  |  \   |
            LVecBase2(-0.25,  0.25),    // ul  |   \  |
            LVecBase2(0.25,  0.25),     // ur  | 4--5 |
        }},

        { "skew_butterly", {
            LVecBase2(-0.250, -0.250),
            LVecBase2(0.250,  0.250),
            LVecBase2(0.125, -0.125),
            LVecBase2(-0.125,  0.125),
        }},

        { "rotated4", {
            LVecBase2(-0.125, -0.375),  // ll
            LVecBase2(0.375, -0.125),   // lr
            LVecBase2(0.125,  0.375),   // ur
            LVecBase2(-0.375,  0.125),  // ul
        }},

        { "rotated4_helix", {
            LVecBase2(-0.125, -0.375),  // ll | 3  1 |
            LVecBase2(0.125,  0.375),   // ur |  \/| |
            LVecBase2(0.375, -0.125),   // lr |  /\| |
            LVecBase2(-0.375,  0.125),  // ul | 0  2 |
        }},

        { "rotated4_helix2", {
            LVecBase2(-0.125, -0.375),  // ll | 2--1 |
            LVecBase2(0.125,  0.375),   // ur |  \/  |
            LVecBase2(-0.375,  0.125),  // ul |  /\  |
            LVecBase2(0.375, -0.125),   // lr | 0  3 |
        }},

        { "poisson10", {
            LVecBase2(-0.16795960 * 0.25,  0.65544910 * 0.25),
            LVecBase2(-0.69096030 * 0.25,  0.59015970 * 0.25),
            LVecBase2(0.49843820 * 0.25,  0.83099720 * 0.25),
            LVecBase2(0.17230150 * 0.25, -0.03882703 * 0.25),
            LVecBase2(-0.60772670 * 0.25, -0.06013587 * 0.25),
            LVecBase2(0.65606390 * 0.25,  0.24007600 * 0.25),
            LVecBase2(0.80348370 * 0.25, -0.48096900 * 0.25),
            LVecBase2(0.33436540 * 0.25, -0.73007030 * 0.25),
            LVecBase2(-0.47839520 * 0.25, -0.56005300 * 0.25),
            LVecBase2(-0.12388120 * 0.25, -0.96633990 * 0.25),
        }},

        { "pentagram", {
            LVecBase2(0.000000 * 0.5,  0.525731 * 0.5),     // head
            LVecBase2(-0.309017 * 0.5, -0.425325 * 0.5),    // lleg
            LVecBase2(0.500000 * 0.5,  0.162460 * 0.5),     // rarm
            LVecBase2(-0.500000 * 0.5,  0.162460 * 0.5),    // larm
            LVecBase2(0.309017 * 0.5, -0.425325 * 0.5),     // rleg
        }}

    });

    // Initialize halton sequences
    for (int seq_num : {8, 16, 32, 256})
    {
        std::vector<LVecBase2> sequence;
        for (int seq_iter=0; seq_iter < seq_num; ++seq_iter)
        {
            LVecBase2 point(halton_seq(2, seq_iter + 1) - 0.5, halton_seq(3, seq_iter + 1) - 0.5);
            sequence.push_back(point);
        }
#if !defined(_MSC_VER) || _MSC_VER >= 1900
        insert_or_assign(std::string("halton") + std::to_string(seq_num), std::move(sequence));
#else
        this->operator[](std::string("halton") + std::to_string(seq_num)) = std::move(sequence);
#endif
    }
}

/** Low discrepancy halton sequence with a given prime */
float JitterType::halton_seq(int prime, int index)
{
    float r = 0.0f;
    float f = 1.0f;
    int i = index;

    while (i > 0)
    {
        f /= prime;
        r += f * (i % prime);
        i = int(i / float(prime));
    }
    return r;
}

}    // namespace rpplugins
