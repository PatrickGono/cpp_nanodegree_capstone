#pragma once
#include <iostream>
#include <cmath>

template<class T>
class vec2
{
public:
    vec2() : x_(0), y_(0) {}
    vec2(T x, T y) : x_(x), y_(y) {}

    T& x() { return x_; }
    T x() const { return x_; }
    T& y() { return y_; }
    T y() const { return y_; }

    T length_squared() const;
    T length() const;

    void normalize();
    vec2 normalized() const;
    static T distance(const vec2<T>& first, const vec2<T>& second);

    // multiplication
    vec2<T>& operator*=(T value);

    template<class S>
    friend S operator*(const vec2<S>& first, const vec2<S>& second);

    template<class S>
    friend vec2<S> operator*(S first, const vec2<S>& second);

    template<class S>
    friend vec2<S> operator*(const vec2<S>& first, S second);

    // division
    vec2<T>& operator/=(T value);

    template<class S>
    friend vec2<S> operator/(const vec2<S>& first, S second);

    // addition 
    vec2<T>& operator+=(const vec2<T>& other);

    template<class S>
    friend vec2<S> operator+(const vec2<S>& first, const vec2<S>& second);

    // subtraction
    vec2<T>& operator-=(const vec2<T>& other);
    vec2<T> operator-(const vec2<T>& vec);

    template<class S>
    friend vec2<S> operator-(const vec2<S>& first, const vec2<S>& second);

    // print output
    template<class S>
    friend std::ostream& operator<<(std::ostream& stream, const vec2<S>& vec);

private:
    T x_;
    T y_;
};

// multiplication
template<class T>
T operator*(const vec2<T>& first, const vec2<T>& second)
{
    return first.x_ * second.x_ + first.y_ * second.y_;
}

template<class T>
vec2<T> operator*(T first, const vec2<T>& second)
{
    return vec2<T>(first * second.x_, first * second.y_);
}

template<class T>
vec2<T> operator*(const vec2<T>& first, T second)
{
    return second * first;
}

template<class T>
vec2<T>& vec2<T>::operator*=(T value)
{
    x_ *= value;
    y_ *= value;
    return *this;
}

// division
template<class T>
vec2<T> operator/(const vec2<T>& first, T second)
{
    return decltype(second){1} / second * first;
}

template<class T>
vec2<T>& vec2<T>::operator/=(T value)
{
    x_ /= value;
    y_ /= value;
    return *this;
}

// addition & subtraction
template<class T>
vec2<T> operator+(const vec2<T>& first, const vec2<T>& second)
{
    return vec2<T>(first.x_ + second.x_, first.y_ + second.y_);
}

template<class T>
vec2<T> operator-(const vec2<T>& first, const vec2<T>& second)
{
    return vec2<T>(first.x_ - second.x_, first.y_ - second.y_);
}

template<class T>
vec2<T> vec2<T>::operator-(const vec2<T>& vec)
{
    return vec2<T>(-vec.x_, -vec.y_);   
}

template<class T>
vec2<T>& vec2<T>::operator-=(const vec2<T>& vec)
{
    x_ -= vec.x_;
    y_ -= vec.y_;
    return *this;
}

template<class T>
vec2<T>& vec2<T>::operator+=(const vec2<T>& vec)
{
    x_ += vec.x_;
    y_ += vec.y_;
    return *this;
}

// print output
template<class T>
std::ostream& operator<<(std::ostream& stream, const vec2<T>& vec)
{
    return stream << vec.x() << ", " << vec.y();
}

// methods
template<class T>
T vec2<T>::length_squared() const
{
    return *this * *this; // *this** this;
}

template<class T>
T vec2<T>::length() const
{
    return std::sqrt(length_squared());
}

template<class T>
void vec2<T>::normalize()
{
    T normalization_factor = static_cast<T>(1) / length();
    x_ *= normalization_factor;
    y_ *= normalization_factor;
}

template<class T>
vec2<T> vec2<T>::normalized() const
{
    T normalization_factor = static_cast<T>(1) / length();
    return vec2<T>(x_, y_) * normalization_factor;
}