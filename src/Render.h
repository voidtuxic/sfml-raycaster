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

inline void getTextureParameters(const sf::Vector2<double> &position, const sf::Vector2<double> &rayDir,
                                 const sf::Vector2i &map, const double perpWallDist, const int side,
                                 const int lineHeight, const int drawStart, int &texNum, int &texX, double &step,
                                 double &texPos) {
    texNum = worldMap[map.x][map.y] - 1;

    //calculate value of wallX
    double wallX; //where exactly the wall was hit
    if (side % 2 == 0) wallX = position.y + perpWallDist * rayDir.y;
    else wallX = position.x + perpWallDist * rayDir.x;
    wallX -= floor((wallX));

    texX = static_cast<int>(wallX * static_cast<double>(TEX_WIDTH));
    if (side % 2 == 0 && rayDir.x > 0) texX = TEX_WIDTH - texX - 1;
    if (side % 2 == 1 && rayDir.y < 0) texX = TEX_WIDTH - texX - 1;
    step = 1.0 * TEX_HEIGHT / lineHeight;
    texPos = (drawStart - RENDER_HEIGHT / 2 + lineHeight / 2) * step;
}

inline void drawColumn(const int x, const int drawStart, const int drawEnd, const int texNum, const int texX,
                       const double step, double texPos, const double perpWallDist, sf::Uint8 *buffer,
                       const std::vector<sf::Image *> &textures) {
    if(perpWallDist > FOG_DISTANCE) return;
    for (int y = drawStart; y < drawEnd; ++y) {
        // Cast the texture coordinate to integer, and mask with (TEX_HEIGHT - 1) in case of overflow
        const int texY = static_cast<int>(texPos) & (TEX_HEIGHT - 1);
        texPos += step;
        auto color = textures[texNum]->getPixel(texX, texY);
        applyFog(perpWallDist, color);
        setColor(buffer, x, y, color);
    }
}

inline void drawFloorAndCeiling(const int y, const sf::Vector2<double> floorStep, sf::Vector2<double> floor,
                                const double &rowDistance, sf::Uint8 *buffer,
                                const std::vector<sf::Image *> &textures) {
    if(rowDistance > FOG_DISTANCE) return;
    for(int x = 0; x < RENDER_WIDTH; ++x)
    {
        // the cell coord is simply got from the integer parts of floorX and floorY
        const auto cell = sf::Vector2i(static_cast<int>(floor.x), static_cast<int>(floor.y));

        // get the texture coordinate from the fractional part
        const int tx = static_cast<int>((TEX_WIDTH * (floor.x - cell.x))) & (TEX_WIDTH - 1);
        const int ty = static_cast<int>((TEX_HEIGHT * (floor.y - cell.y))) & (TEX_HEIGHT - 1);

        floor.x += floorStep.x;
        floor.y += floorStep.y;

        // floor
        auto color = textures[FLOOR_TEXTURE]->getPixel(tx, ty);
        applyFog(rowDistance, color);
        setColor(buffer, x, y, color);

        // //ceiling (symmetrical, at screenHeight - y - 1 instead of y)
        // color = textures[CEILING_TEXTURE]->getPixel(tx, ty);
        // applyFog(rowDistance, color);
        // setColor(x, RENDER_HEIGHT - y - 1, color);
    }
}

#endif //RENDER_H
