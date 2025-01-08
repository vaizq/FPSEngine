#pragma once

#include <chrono>


template <typename Clock = std::chrono::steady_clock>
class DeltaTimer
{
public:
    using Duration = std::chrono::duration<float>;
    Duration tick()
    {
        if (!mInitialized)
        {
            mLastTick = mClock.now();
            mInitialized = true;
            return Duration(0.0f); 
        }

        const auto now = mClock.now();
        const auto dt = std::chrono::duration_cast<Duration>(now - mLastTick);
        mLastTick = now;
        return dt;
    }
private:
    Clock mClock;
    typename Clock::time_point mLastTick;
    bool mInitialized{false};
};
