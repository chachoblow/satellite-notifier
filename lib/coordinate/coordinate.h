#pragma once

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
struct Coordinate
{
    T x;
    T y;
};