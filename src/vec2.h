#pragma once
#include <iostream>
#include <cmath>

template<class T>
class vec2
{
public: // Structors
    vec2() : x_(0), y_(0) {}
    vec2(T val) : x_(val), y_(val) {}
    vec2(T x, T y) : x_(x), y_(y) {}

public: // Accessors
    auto x() -> T& { return x_; }
    auto x() const -> T { return x_; }
    auto y() -> T& { return y_; }
    auto y() const -> T { return y_; }

public: // Interface
    auto length_squared() const -> T;
    auto length() const -> T;

    auto normalize() -> void;
    auto normalized() const -> vec2;
    static auto distance(const vec2<T>& first, const vec2<T>& second) -> T;
    static auto distance_squared(const vec2<T>& first, const vec2<T>& second) -> T;

public: // Operators
    // multiplication
    auto operator*=(T value) -> vec2<T>&;

    template<class S>
    friend auto operator*(const vec2<S>& first, const vec2<S>& second) -> S;

    template<class S>
    friend auto operator*(S first, const vec2<S>& second) -> vec2<S>;

    template<class S>
    friend auto operator*(const vec2<S>& first, S second) -> vec2<S>;

    // division
    auto operator/=(T value) -> vec2<T>&;

    template<class S>
    friend auto operator/(const vec2<S>& first, S second) -> vec2<S>;

    // addition 
    auto operator+=(const vec2<T>& other) -> vec2<T>&;

    template<class S>
    friend auto operator+(const vec2<S>& first, const vec2<S>& second) -> vec2<S>;

    // subtraction
    auto operator-=(const vec2<T>& other) -> vec2<T>&;

    template<class S>
    friend auto operator-(const vec2<S>& first, const vec2<S>& second) -> vec2<S>;

    // print output
    template<class S>
    friend auto operator<<(std::ostream& stream, const vec2<S>& vec) -> std::ostream&;

private: // Variables
    T x_;
    T y_;
};

///
///
template<class T>
T vec2<T>::length_squared() const
{
    return *this * *this;
}

///
///
template<class T>
T vec2<T>::length() const
{
    return std::sqrt(length_squared());
}

///
///
template<class T>
void vec2<T>::normalize()
{
    T normalization_factor = static_cast<T>(1) / length();
    x_ *= normalization_factor;
    y_ *= normalization_factor;
}

///
///
template<class T>
vec2<T> vec2<T>::normalized() const
{
    T normalization_factor = static_cast<T>(1) / length();
    return vec2<T>(x_, y_) * normalization_factor;
}

///
///
template<class T>
T vec2<T>::distance_squared(const vec2<T>& first, const vec2<T>& second)
{
    auto difference = second - first;
    return difference * difference;
}

///
///
template<class T>
T vec2<T>::distance(const vec2<T>& first, const vec2<T>& second)
{
    return std::sqrt(distance_squared(first, second));
}

// multiplication

///
///
template<class T>
T operator*(const vec2<T>& first, const vec2<T>& second)
{
    return first.x_ * second.x_ + first.y_ * second.y_;
}

///
///
template<class T>
vec2<T> operator*(T first, const vec2<T>& second)
{
    return vec2<T>(first * second.x_, first * second.y_);
}

///
///
template<class T>
vec2<T> operator*(const vec2<T>& first, T second)
{
    return second * first;
}

///
///
template<class T>
vec2<T>& vec2<T>::operator*=(T value)
{
    x_ *= value;
    y_ *= value;
    return *this;
}

// division

///
///
template<class T>
vec2<T> operator/(const vec2<T>& first, T second)
{
    return decltype(second){1} / second * first;
}

///
///
template<class T>
vec2<T>& vec2<T>::operator/=(T value)
{
    x_ /= value;
    y_ /= value;
    return *this;
}

// addition & subtraction

///
///
template<class T>
vec2<T> operator+(const vec2<T>& first, const vec2<T>& second)
{
    return vec2<T>(first.x_ + second.x_, first.y_ + second.y_);
}

///
///
template<class T>
vec2<T> operator-(const vec2<T>& first, const vec2<T>& second)
{
    return vec2<T>(first.x_ - second.x_, first.y_ - second.y_);
}

///
///
template<class T>
vec2<T>& vec2<T>::operator-=(const vec2<T>& vec)
{
    x_ -= vec.x_;
    y_ -= vec.y_;
    return *this;
}

///
///
template<class T>
vec2<T>& vec2<T>::operator+=(const vec2<T>& vec)
{
    x_ += vec.x_;
    y_ += vec.y_;
    return *this;
}

// print output

///
///
template<class T>
std::ostream& operator<<(std::ostream& stream, const vec2<T>& vec)
{
    return stream << vec.x() << ", " << vec.y();
}