//
// Created by tuxic on 21/12/2023.
//

#ifndef CONFIG_H
#define CONFIG_H

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

#define TEX_WIDTH 128
#define TEX_HEIGHT 128

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_CENTER glm::vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2)

#define RENDER_SCALE 4
#define RENDER_WIDTH 320// (WINDOW_WIDTH / RENDER_SCALE)
#define RENDER_HEIGHT 200// (WINDOW_HEIGHT / RENDER_SCALE)
#define RENDER_COMPONENTS 4
#define RENDER_STRIDE (RENDER_WIDTH * RENDER_COMPONENTS)
#define CLEAR_COLOR sf::Color(65, 157, 160)

#define WALL_DISTANCE 8
#define FOG_DISTANCE 10

#define FLOOR_TEXTURE 8
#define CEILING_TEXTURE 9

#define MOUSE_SENSITIVITY_HORIZONTAL 0.5f
#define MOUSE_SENSITIVITY_VERTICAL 100
#define MOUSE_VERTICAL_PITCH 100.0f

inline int worldMap[MAP_WIDTH][MAP_HEIGHT] =
{
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

#endif //CONFIG_H
