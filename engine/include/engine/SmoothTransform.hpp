#ifndef SMOOTH_TRANSFORM_HPP
#define SMOOTH_TRANSFORM_HPP

#include "Transform.hpp"
#include "Clock.hpp"

class SmoothTransform {
public:
    SmoothTransform(Transform from, Transform to, double duration)
    : mFrom{from}, mTo{to}, mDuration{duration}
    {}

    void reset() {
        mTime = 0;
    }

    void update(double dt) {
        mTime += dt;

        if (mTime >= mDuration) {
            mTime = mDuration;
        } else if (mTime <= 0) {
            mTime = 0;
        }
    }

    Transform current() const {
        return lerp(mFrom, mTo, mTime / mDuration);
    }

private:
    Transform mFrom;
    Transform mTo;
    double mDuration;
    double mTime{0};
};


#endif
