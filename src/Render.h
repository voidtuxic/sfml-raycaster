//
// Created by tuxic on 21/12/2023.
//

#ifndef RENDER_H
#define RENDER_H
#include <cmath>
#include "Config.h"

namespace wolf {
    struct CameraData {
        CameraData(const glm::vec2 &position, const glm::vec2 &direction,
            const glm::vec2 &plane)
            : position(position),
              direction(direction),
              plane(plane) {
        }
        glm::vec2 position;
        glm::vec2 direction;
        glm::vec2 plane;
        float positionZ = 0;
        float pitch = 0;
    };

    struct RenderData {
        sf::Uint8 *buffer;
        std::vector<sf::Image *> textures;

        void clearBuffer() const {
            for (int x = 0; x < RENDER_WIDTH; x++) {
                for (int y = 0; y < RENDER_HEIGHT; y++) {
                    setColor(x, y, CLEAR_COLOR);
                }
            }
        }

        void setColor(const int x, const int y, const sf::Color color) const {
            buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS] = color.r;
            buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 1] = color.g;
            buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 2] = color.b;
            buffer[y * RENDER_STRIDE + x * RENDER_COMPONENTS + 3] = color.a;
        }
    };

    struct RaycastData {
        glm::vec2 rayDirection;
        glm::ivec2 mapPosition;
        float wallDistance{};
        float wallX{};
        int side{};
        int lineHeight{};
        int drawStart{};
        int drawEnd{};
        int textureId{};
        int textureX{};
        float step{};
        float texturePosition{};
        glm::vec2 floorWall;
        int wallHeight = 3;

        void populateTextureParameters(const glm::vec2 &position, const float &positionZ, const float &pitch) {
            textureId = worldMap[mapPosition.x][mapPosition.y] - 1;

            //calculate value of wallX
            if (side % 2 == 0) wallX = position.y + wallDistance * rayDirection.y;
            else wallX = position.x + wallDistance * rayDirection.x;
            wallX -= std::floor(wallX);

            textureX = static_cast<int>(wallX * static_cast<float>(TEX_WIDTH));
            if (side % 2 == 0 && rayDirection.x > 0) textureX = TEX_WIDTH - textureX - 1;
            if (side % 2 == 1 && rayDirection.y < 0) textureX = TEX_WIDTH - textureX - 1;
            step = 1.0 * TEX_HEIGHT / lineHeight;
            texturePosition = (drawStart - pitch - positionZ - RENDER_HEIGHT / 2 + lineHeight / 2) * step;
        }
    };

    inline void applyFog(const float distance, sf::Color &color) {
        float delta = std::clamp(distance / FOG_DISTANCE, 0.0f, 1.0f);
        delta *= delta * delta;
        color.r = static_cast<sf::Uint8>(std::lerp(color.r, CLEAR_COLOR.r, static_cast<sf::Uint8>(delta)));
        color.g = static_cast<sf::Uint8>(std::lerp(color.g, CLEAR_COLOR.g, delta));
        color.b = static_cast<sf::Uint8>(std::lerp(color.b, CLEAR_COLOR.b, delta));
    }

    inline void drawColumn(const int x, RaycastData &raycast, const RenderData *renderData) {
        if (raycast.wallDistance > FOG_DISTANCE
            || raycast.textureId >= renderData->textures.size()) return;
        for (int y = raycast.drawStart; y <= raycast.drawEnd; ++y) {
            // Cast the texture coordinate to integer, and mask with (TEX_HEIGHT - 1) in case of overflow
            const int texY = static_cast<int>(raycast.texturePosition) & (TEX_HEIGHT - 1);
            raycast.texturePosition += raycast.step;
            auto color = renderData->textures[raycast.textureId]->getPixel(raycast.textureX, texY);
            applyFog(raycast.wallDistance, color);
            renderData->setColor(x, y, color);
        }
    }

    inline void drawFloorWall(const int x, RaycastData &raycast,
                              const CameraData *camera, const RenderData *renderData) {
        if (raycast.drawEnd < 0) raycast.drawEnd = RENDER_HEIGHT; //becomes < 0 when the integer overflows

        // ceiling
        for (int y = raycast.drawStart - 1; y >= 0; y--) {
            constexpr float distPlayer = 0.0;

            const float ceilingHeight = 2.0 * (raycast.wallHeight - 1) * RENDER_HEIGHT;
            const float currentDist = (RENDER_HEIGHT - 2.0f * camera->positionZ + ceilingHeight)
                / (RENDER_HEIGHT - 2.0f * (y - camera->pitch));
            //you could make a small lookup table for this instead

            const float weight = (currentDist - distPlayer) / (raycast.wallDistance - distPlayer);

            const float currentFloorX = weight * raycast.floorWall.x + (1.0f - weight) * camera->position.x;
            const float currentFloorY = weight * raycast.floorWall.y + (1.0f - weight) * camera->position.y;

            const int floorTexX = std::clamp(static_cast<int>(currentFloorX * TEX_WIDTH) % TEX_WIDTH, 0, TEX_WIDTH);
            const int floorTexY = std::clamp(static_cast<int>(currentFloorY * TEX_HEIGHT) % TEX_HEIGHT, 0, TEX_HEIGHT);

            auto color = renderData->textures[CEILING_TEXTURE]->getPixel(floorTexX, floorTexY);
            applyFog(currentDist, color);
            renderData->setColor(x, y, color);
        }

        // floor
        for (int y = raycast.drawEnd + 1; y < RENDER_HEIGHT; y++) {
            constexpr float distPlayer = 0.0;
            const float currentDist = (RENDER_HEIGHT + 2.0f * camera->positionZ) / (2.0f * (y - camera->pitch) - RENDER_HEIGHT);
            //you could make a small lookup table for this instead

            const float weight = (currentDist - distPlayer) / (raycast.wallDistance - distPlayer);

            const float currentFloorX = weight * raycast.floorWall.x + (1.0f - weight) * camera->position.x;
            const float currentFloorY = weight * raycast.floorWall.y + (1.0f - weight) * camera->position.y;

            const int floorTexX = std::clamp(static_cast<int>(currentFloorX * TEX_WIDTH) % TEX_WIDTH, 0, TEX_WIDTH);
            const int floorTexY = std::clamp(static_cast<int>(currentFloorY * TEX_HEIGHT) % TEX_HEIGHT, 0, TEX_HEIGHT);

            auto color = renderData->textures[FLOOR_TEXTURE]->getPixel(floorTexX, floorTexY);
            applyFog(currentDist, color);
            renderData->setColor(x, y, color);
        }
    }
}

#endif //RENDER_H
