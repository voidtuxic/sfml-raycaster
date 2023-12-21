//
// Created by tuxic on 21/12/2023.
//

#ifndef RENDER_H
#define RENDER_H
#include "Config.h"

inline void setColor(sf::Uint8* buffer, const int x, const int y, const sf::Color color) {
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS] = color.r;
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 1] = color.g;
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 2] = color.b;
    buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 3] = color.a;
}

inline void clearBuffer(sf::Uint8* buffer) {
    for (int x = 0; x < RENDER_WIDTH; x++) {
        for (int y = 0; y < RENDER_HEIGHT; y++) {
            setColor(buffer, x, y, CLEAR_COLOR);
        }
    }
}

inline void applyFog(const double distance, sf::Color &color) {
    double delta = std::clamp(distance / FOG_DISTANCE, 0.0, 1.0);
    delta *= delta;
    color.r = std::lerp(color.r, CLEAR_COLOR.r, delta);
    color.g = std::lerp(color.g, CLEAR_COLOR.g, delta);
    color.b = std::lerp(color.b, CLEAR_COLOR.b, delta);
}

#endif //RENDER_H
