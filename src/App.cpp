//
// Created by tuxic on 21/12/2023.
//

#include <cmath>
#include "App.h"
#include "Config.h"
#include "Floor.h"
#include "Math.h"
#include "Render.h"
#include "Wall.h"


App::App() {
    window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Raycaster");
    window->setVerticalSyncEnabled(true);
    window->setMouseCursorGrabbed(true);
    window->setMouseCursorVisible(false);

    buffer = new sf::Uint8[RENDER_WIDTH * RENDER_HEIGHT * RENDER_COMPONENTS];

    texture = new sf::Texture();
    texture->create(RENDER_WIDTH, RENDER_HEIGHT);

    bufferRect = new sf::RectangleShape(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bufferRect->setTexture(texture, true);

    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures.push_back(new sf::Image());
    textures[0]->loadFromFile("textures/wool_colored_yellow.png");
    textures[1]->loadFromFile("textures/wool_colored_silver.png");
    textures[2]->loadFromFile("textures/wool_colored_orange.png");
    textures[3]->loadFromFile("textures/wool_colored_purple.png");
    textures[4]->loadFromFile("textures/wool_colored_magenta.png");
    textures[5]->loadFromFile("textures/glass_blue.png");
    textures[6]->loadFromFile("textures/wool_colored_gray.png");
    textures[7]->loadFromFile("textures/wool_colored_red.png");
    textures[8]->loadFromFile("textures/stone_andesite_smooth.png");
    textures[9]->loadFromFile("textures/pumpkin_top.png");
}

App::~App() {
    for (const auto image: textures) {
        free(image);
    }
    textures.clear();
    free(bufferRect);
    free(texture);
    free(window);
}

int App::run() {
    while (window->isOpen()) {
        handleInput();
        render();
    }

    return EXIT_SUCCESS;
}

void App::handleInput() {
    //timing for input and FPS counter
    const sf::Time dt = deltaClock.restart();
    const double frameTime = dt.asSeconds();

    //speed modifiers
    const double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
    sf::Vector2<double> movement;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) {
        movement.y += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
        movement.y -= 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
        movement.x += 1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
        movement.x -= 1;
    }

    if(movement.x != 0 || movement.y != 0) {
        normalize(movement);
        if (worldMap
            [static_cast<int>(position.x + direction.x * moveSpeed * WALL_DISTANCE * movement.y)]
            [static_cast<int>(position.y)] == false)
            position.x += direction.x * moveSpeed * movement.y;
        if (worldMap
            [static_cast<int>(position.x)]
            [static_cast<int>(position.y + direction.y * moveSpeed * WALL_DISTANCE * movement.y)] == false)
            position.y += direction.y * moveSpeed * movement.y;
        if (worldMap
            [static_cast<int>(position.x + direction.y * moveSpeed * WALL_DISTANCE * movement.x)]
            [static_cast<int>(position.y)] == false)
            position.x += direction.y * moveSpeed * movement.x;
        if (worldMap
            [static_cast<int>(position.x)]
            [static_cast<int>(position.y - direction.x * moveSpeed * WALL_DISTANCE * movement.x)] == false)
            position.y -= direction.x * moveSpeed * movement.x;
    }

    auto mousePosition = sf::Mouse::getPosition(*window);
    if(const auto mouseDelta = mousePosition - previousMousePosition; abs(mouseDelta.x) > 0) {
        //both camera direction and camera plane must be rotated
        const double oldDirX = direction.x;
        const double rotSpeed = frameTime * -mouseDelta.x * MOUSE_SENSITIVITY; //the constant value is in radians/second
        direction.x = direction.x * cos(rotSpeed) - direction.y * sin(rotSpeed);
        direction.y = oldDirX * sin(rotSpeed) + direction.y * cos(rotSpeed);
        const double oldPlaneX = plane.x;
        plane.x = plane.x * cos(rotSpeed) - plane.y * sin(rotSpeed);
        plane.y = oldPlaneX * sin(rotSpeed) + plane.y * cos(rotSpeed);
    }
    previousMousePosition = mousePosition;
    if(mousePosition.x <= 1) {
        mousePosition.x += WINDOW_WIDTH;
        previousMousePosition.x += WINDOW_WIDTH;
        sf::Mouse::setPosition(mousePosition, *window);
    }
    else if(mousePosition.x >= WINDOW_WIDTH - 1) {
        mousePosition.x -= WINDOW_WIDTH;
        previousMousePosition.x -= WINDOW_WIDTH;
        sf::Mouse::setPosition(mousePosition, *window);
    }

    sf::Event event{};
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }
}

void App::render() const {
    clearBuffer(buffer);

    for (int x = 0; x < RENDER_WIDTH; x++) {
        sf::Vector2<double> rayDir;
        sf::Vector2i map;
        double perpWallDist, wallX;
        int side, lineHeight, drawStart, drawEnd;
        calculateWall(worldMap, position, direction, plane, x, rayDir, map, perpWallDist,
            side, lineHeight, drawStart, drawEnd);

        int texNum, texX;
        double step, texPos;
        getTextureParameters(position, rayDir, map, perpWallDist, side, lineHeight, drawStart,
            texNum, texX, step, texPos, wallX);
        drawColumn(x, drawStart, drawEnd, texNum, texX, step, texPos, perpWallDist, buffer, textures);

        sf::Vector2<double> floorWall; //x, y position of the floor texel at the bottom of the wall
        calculateFloorWall(rayDir, map, wallX, side, floorWall);
        drawFloorWall(position, x, perpWallDist, drawEnd, floorWall, buffer, textures);
    }

    texture->update(buffer);
    window->clear(sf::Color::Black);
    window->draw(*bufferRect);
    window->display();
}
