#pragma once

#include <chrono>

namespace Needle {
    class Timer final {
    public:
        using Clock = std::chrono::high_resolution_clock;
        using Duration = Clock::duration;
        using TimePoint = Clock::time_point;
        using Precision = float;

        Timer() : mDeltaTime(0),
                  mPrev(Clock::now()),
                  mCurr(Clock::now()) {}

        auto deltaTime() const { return mDeltaTime; }

        void reset() {
            mDeltaTime = 0;
            mPrev = mCurr = Clock::now();
        }

        void tick() {
            mCurr = Clock::now();
            mDeltaTime = DurationToSecond(mCurr - mPrev);
            mPrev = mCurr;
        }

    private:
        Precision mDeltaTime;
        TimePoint mPrev, mCurr;

        static Precision DurationToSecond(Duration&& _duration) {
            const auto fullSteps = _duration.count() / Duration::period::den,
                       restSteps = _duration.count() - fullSteps * Duration::period::den;
            return (fullSteps + static_cast<Precision>(restSteps) / Duration::period::den) * Duration::period::num;
        }
    };
}
