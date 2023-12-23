//
// Created by tuxic on 21/12/23.
//

#ifndef MATH_H
#define MATH_H
#include <cmath>
#include "SFML/System/Vector2.hpp"

inline void normalize(glm::vec2 &v) {
    const auto length = std::sqrt(v.x * v.x + v.y * v.y);
    v /= length;
}

inline glm::ivec2 ToGLM(const sf::Vector2i vec) {
    return {vec.x, vec.y};
}

inline sf::Vector2i FromGLM(const glm::ivec2 vec) {
    return {vec.x, vec.y};
}

#endif //MATH_H
