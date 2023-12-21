//
// Created by tuxic on 21/12/23.
//

#ifndef MATH_H
#define MATH_H
#include <cmath>
#include "SFML/System/Vector2.hpp"

inline void normalize(sf::Vector2<double> &v) {
    const auto length = std::sqrt(v.x * v.x + v.y * v.y);
    v /= length;
}

#endif //MATH_H
