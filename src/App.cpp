//
// Created by tuxic on 21/12/2023.
//

#include <cmath>
#include <format>
#include "App.h"
#include "Floor.h"
#include "Math.h"
#include "Wall.h"


App::App() {
    camera = new wolf::CameraData(
        sf::Vector2<double>(6.5, 4),
        sf::Vector2<double>(-1, 0),
        sf::Vector2<double>(0, 0.9));

    window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Raycaster");
    window->setVerticalSyncEnabled(true);
    window->setMouseCursorGrabbed(true);
    window->setMouseCursorVisible(false);

    texture = new sf::Texture();
    texture->create(RENDER_WIDTH, RENDER_HEIGHT);

    bufferRect = new sf::RectangleShape(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bufferRect->setTexture(texture, true);

    renderData = new wolf::RenderData;
    renderData->buffer = new sf::Uint8[RENDER_WIDTH * RENDER_HEIGHT * RENDER_COMPONENTS];
    loadTexture("textures/wool_colored_yellow.png");
    loadTexture("textures/wool_colored_silver.png");
    loadTexture("textures/wool_colored_orange.png");
    loadTexture("textures/wool_colored_purple.png");
    loadTexture("textures/wool_colored_magenta.png");
    loadTexture("textures/glass_blue.png");
    loadTexture("textures/wool_colored_gray.png");
    loadTexture("textures/wool_colored_red.png");
    loadTexture("textures/stone_andesite_smooth.png");
    loadTexture("textures/pumpkin_top.png");

    debugFont.loadFromFile("fonts/VT323-Regular.ttf");
    debugText = new sf::Text;
    debugText->setFont(debugFont);
    debugText->setCharacterSize(64);
    debugText->setPosition(10,0);
    debugText->setFillColor(sf::Color::Green);
}

App::~App() {
    for (const auto image: renderData->textures) {
        free(image);
    }
    renderData->textures.clear();
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

void App::loadTexture(const std::string &filename) const {
    renderData->textures.push_back(new sf::Image());
    renderData->textures[renderData->textures.size() - 1]->loadFromFile(filename);
}

void App::handleKeyboard(const double frameTime, const double totalTime) const {
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
            [static_cast<int>(camera->position.x + camera->direction.x * moveSpeed * WALL_DISTANCE * movement.y)]
            [static_cast<int>(camera->position.y)] == false)
            camera->position.x += camera->direction.x * moveSpeed * movement.y;
        if (worldMap
            [static_cast<int>(camera->position.x)]
            [static_cast<int>(camera->position.y + camera->direction.y * moveSpeed * WALL_DISTANCE * movement.y)] == false)
            camera->position.y += camera->direction.y * moveSpeed * movement.y;
        if (worldMap
            [static_cast<int>(camera->position.x + camera->direction.y * moveSpeed * WALL_DISTANCE * movement.x)]
            [static_cast<int>(camera->position.y)] == false)
            camera->position.x += camera->direction.y * moveSpeed * movement.x;
        if (worldMap
            [static_cast<int>(camera->position.x)]
            [static_cast<int>(camera->position.y - camera->direction.x * moveSpeed * WALL_DISTANCE * movement.x)] == false)
            camera->position.y -= camera->direction.x * moveSpeed * movement.x;

        camera->positionZ = std::cos(totalTime * 16) * 8;
    }
    else {
        camera->positionZ = 0;
    }
}

void App::handleMouse(const double frameTime) {
    auto mousePosition = sf::Mouse::getPosition(*window);
    const auto mouseDelta = mousePosition - previousMousePosition;

    if(abs(mouseDelta.x) > 0) {
        //both camera direction and camera plane must be rotated
        const double oldDirX = camera->direction.x;
        const double rotSpeed = frameTime * -mouseDelta.x * MOUSE_SENSITIVITY_HORIZONTAL; //the constant value is in radians/second
        camera->direction.x = camera->direction.x * cos(rotSpeed) - camera->direction.y * sin(rotSpeed);
        camera->direction.y = oldDirX * sin(rotSpeed) + camera->direction.y * cos(rotSpeed);
        const double oldPlaneX = camera->plane.x;
        camera->plane.x = camera->plane.x * cos(rotSpeed) - camera->plane.y * sin(rotSpeed);
        camera->plane.y = oldPlaneX * sin(rotSpeed) + camera->plane.y * cos(rotSpeed);
    }

    if(abs(mouseDelta.y) > 0) {
        const double rotSpeed = frameTime * -mouseDelta.y * MOUSE_SENSITIVITY_VERTICAL; //the constant value is in radians/second
        camera->pitch = std::clamp(camera->pitch + rotSpeed, -MOUSE_VERTICAL_PITCH, MOUSE_VERTICAL_PITCH);
    }

    previousMousePosition = mousePosition;
    if(mousePosition.x <= 1) {
        mousePosition.x += WINDOW_WIDTH;
        previousMousePosition.x += WINDOW_WIDTH;
    }
    else if(mousePosition.x >= WINDOW_WIDTH - 1) {
        mousePosition.x -= WINDOW_WIDTH;
        previousMousePosition.x -= WINDOW_WIDTH;
    }
    if(mousePosition.y <= 1) {
        mousePosition.y += WINDOW_HEIGHT;
        previousMousePosition.y += WINDOW_HEIGHT;
    }
    else if(mousePosition.y >= WINDOW_HEIGHT - 1) {
        mousePosition.y -= WINDOW_HEIGHT;
        previousMousePosition.y -= WINDOW_HEIGHT;
    }
    sf::Mouse::setPosition(mousePosition, *window);
}

void App::handleInput() {
    //timing for input and FPS counter
    const sf::Time dt = deltaClock.restart();
    const double frameTime = dt.asSeconds();
    const double totalTime = clock.getElapsedTime().asSeconds();

    handleKeyboard(frameTime, totalTime);
    handleMouse(frameTime);

    sf::Event event{};
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }

    int elapsedMS = dt.asMilliseconds();
    debugText->setString(std::format("FPS: {}\nFrame time: {}ms\nRender time: {}ms", 1000 / elapsedMS, elapsedMS,
                                     renderTimeMS));
}

void App::render() {
    renderDeltaClock.restart();
    renderData->clearBuffer();

    for (int x = 0; x < RENDER_WIDTH; x++) {
        wolf::RaycastData raycast;
        calculateWall(x, raycast, worldMap, camera);
        raycast.populateTextureParameters(camera->position, camera->positionZ / raycast.wallDistance, camera->pitch);
        drawColumn(x, raycast, renderData);

        calculateFloorWall(raycast);
        drawFloorWall(x, raycast, camera, renderData);
    }

    const sf::Time dt = renderDeltaClock.restart();
    renderTimeMS = dt.asMilliseconds();

    texture->update(renderData->buffer);
    window->clear(sf::Color::Black);
    window->draw(*bufferRect);
    window->draw(*debugText);

    window->display();
}
