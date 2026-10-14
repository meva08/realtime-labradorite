#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec
{
    double data[n] = {0};
    double& operator[](const int i)       { assert(i>=0 && i<n); return data[i]; }
    double  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
    vec<n> operator/(double s) const {
        vec<n> ret;
        for (int i=n; i--; ) ret[i] = data[i] / s;
        return ret;
    }
};

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i)       { assert(i>=0 && i<2); return i ? y : x; }
    double  operator[](const int i) const { assert(i>=0 && i<2); return i ? y : x; }
    float norm() const { return std::sqrt(x*x + y*y); }
    float Dot(const vec<2>& v) const { return x*v.x + y*v.y; }

    vec<2> operator+(const vec<2>& v) const { return {x + v.x, y + v.y}; }
    vec<2> operator-(const vec<2>& v) const { return {x - v.x, y - v.y}; }
    vec<2> operator*(double s) const { return {x * s, y * s}; }
    vec<2> operator/(double s) const { return {x / s, y / s}; }
};

typedef vec<2> vec2;

template<> struct vec<3>
{
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
    double  operator[](const int i) const { assert(i>=0 && i<3); return i ? (1==i ? y : z) : x; }
    float norm() const { return std::sqrt(x*x + y*y + z*z); }
    float Dot(const vec<3>& v) const { return x*v.x + y*v.y + z*v.z; }
    float Mag(const vec<3>& v) const { return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z); }
    vec<3> Cross(const vec<3>& v) const { return {x*v.y - y*v.x, y*v.z - z*v.y, z*v.x - x*v.z}; }

    vec<3> operator+(const vec<3>& v) const { return {x + v.x, y + v.y, z + v.z}; }
    vec<3> operator-(const vec<3>& v) const { return {x - v.x, y - v.y, z - v.z}; }
    vec<3> operator*(double s) const { return {x * s, y * s, z * s}; }
    vec<3> operator/(double s) const { return {x / s, y / s, z / s}; }
};

typedef vec<3> vec3;