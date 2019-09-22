#pragma once

#include <random>
#include <ctime>

namespace Needle {
    class Rng final {
    public:
        Rng(const float _min = 0, const float _max = 1) : mRng(time(nullptr)),
                                                          mDist(_min, _max) {}

        float operator()() { return mDist(mRng); }

    private:
        std::mt19937 mRng;
        std::uniform_real_distribution<float> mDist;
    };
}
