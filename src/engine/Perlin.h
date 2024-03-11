//
// Created by vaige on 8.3.2024.
//

#ifndef FPSFROMSCRATCH_PERLIN_H
#define FPSFROMSCRATCH_PERLIN_H

#include <glm/glm.hpp>
#include <unordered_map>
#include <array>
#include <vector>
#include <random>



class Perlin2D
{
public:
    Perlin2D(int width, int height, int seed = 1)
    :   mWidth{width},
        mHeight{height},
        mGradientCache((width + 1) * (height + 1)),
        mCacheStatus((width + 1) * (height + 1)),
        mRndEngine(mRndDevice()),
        mRealDist(-1.f, 1.f)
    {
        // Build gradient cache
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                randomGridGradient(x, y);
            }
        }
    }

    float operator()(const glm::vec2& point)
    {
        int x0 = std::floor(point.x);
        int x1 = x0 + 1;
        int y0 = std::floor(point.y);
        int y1 = y0 + 1;

        float sx = point.x - static_cast<float>(x0);
        float sy = point.y - static_cast<float>(y0);


        float n0 = dotGridGradient(x0, y0, point);
        float n1 = dotGridGradient(x1, y0, point);
        float ix0 = interpolate(n0, n1, sx);

        n0 = dotGridGradient(x0, y1, point);
        n1 = dotGridGradient(x1, y1, point);
        float ix1 = interpolate(n0, n1, sx);

        return interpolate(ix0, ix1, sy);
    }

private:
    // w should be in range [0, 1]
    static float interpolate(float a, float b, float w) {
        return (b - a) * w + a;
    }

    glm::vec2 randomGridGradient(int xi, int yi)
    {
        int index = yi * mWidth + xi;
        if (!mCacheStatus[index]) {
            mGradientCache[index] = glm::normalize(glm::vec2{mRealDist(mRndEngine), mRealDist(mRndEngine)});
            mCacheStatus[index] = true;
        }

        return mGradientCache[index];
    }

    float dotGridGradient(int xi, int yi, const glm::vec2& point)
    {
        glm::vec2 gradient = randomGridGradient(xi, yi);
        const glm::vec2 dist(point.x - static_cast<float>(xi), point.y - static_cast<float>(yi));
        return glm::dot(gradient, dist);
    }

    const int mWidth;
    const int mHeight;
    std::vector<glm::vec2> mGradientCache;
    std::vector<bool> mCacheStatus;
    std::random_device mRndDevice{};
    std::default_random_engine mRndEngine;
    std::uniform_real_distribution<float> mRealDist;
};

// Cell number = (y + 1) * x



#endif //FPSFROMSCRATCH_PERLIN_H
