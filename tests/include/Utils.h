#pragma once
/**
 * @file Utils.h
 * @author Alan Abraham P Kochumon
 * @date Created on: May 09, 2026.
 *
 * @brief Utilities and helper for testing.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

struct alignas(16) Vec4
{
    float x, y, z, w;
    Vec4(const float v0, const float v1, const float v2, const float v3): x(v0), y(v1), z(v2), w(v3) {}
    Vec4(const int v0, const int v1, const int v2, const int v3)
        : x(static_cast<float>(v0)), y(static_cast<float>(v1)), z(static_cast<float>(v2)), w(static_cast<float>(v3))
    {}

    
    void set(const float v0, const float v1, const float v2, const float v3)
    {
        this->x = v0;
        this->y = v1;
        this->z = v2;
        this->w = v3;
    }
};
