//
// Created by tuxic on 21/12/2023.
//

#include "App.h"

#include <cmath>

App::App() {
    window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Raycaster");
    window->setVerticalSyncEnabled(true);

    buffer = new sf::Image();
    buffer->create(RENDER_WIDTH, RENDER_HEIGHT, sf::Color::Black);

    texture = new sf::Texture();
    texture->loadFromImage(*buffer);

    bufferRect = new sf::RectangleShape(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bufferRect->setTexture(texture, true);
}

App::~App() {
    for (const auto image: textures) {
        free(image);
    }
    textures.clear();
    free(bufferRect);
    free(texture);
    free(buffer);
    free(window);
}

int App::run() {
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
    textures[3]->loadFromFile("textures/wool_colored_black.png");
    textures[4]->loadFromFile("textures/wool_colored_blue.png");
    textures[5]->loadFromFile("textures/wool_colored_brown.png");
    textures[6]->loadFromFile("textures/wool_colored_gray.png");
    textures[7]->loadFromFile("textures/wool_colored_red.png");

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
    const double rotSpeed = frameTime * 3.0; //the constant value is in radians/second

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) {
        if (worldMap[static_cast<int>(posX + dirX * moveSpeed * WALL_DISTANCE)][static_cast<int>(posY)] == false)
            posX += dirX * moveSpeed;
        if (worldMap[static_cast<int>(posX)][static_cast<int>(posY + dirY * moveSpeed * WALL_DISTANCE)] == false)
            posY += dirY * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
        if (worldMap[static_cast<int>(posX - dirX * moveSpeed * WALL_DISTANCE)][static_cast<int>(posY)] == false)
            posX -= dirX * moveSpeed;
        if (worldMap[static_cast<int>(posX)][static_cast<int>(posY - dirY * moveSpeed * WALL_DISTANCE)] == false)
            posY -= dirY * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
        //both camera direction and camera plane must be rotated
        const double oldDirX = dirX;
        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
        const double oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
        //both camera direction and camera plane must be rotated
        const double oldDirX = dirX;
        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
        const double oldPlaneX = planeX;
        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    }

    sf::Event event{};
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }
}

void App::clearBuffer(const int x) const {
    for (int y = 0; y < RENDER_HEIGHT; y++) {
        buffer->setPixel(x, y, sf::Color::Black);
    }
}

void App::calculateStep(const sf::Vector2<double> &rayDir, const sf::Vector2<double> &deltaDist,
                        const sf::Vector2i map, sf::Vector2<double> &sideDist,
                        sf::Vector2i &step) const {
    //what direction to step in x or y-direction (either +1 or -1)
    if (rayDir.x < 0) {
        step.x = -1;
        sideDist.x = (posX - map.x) * deltaDist.x;
    } else {
        step.x = 1;
        sideDist.x = (map.x + 1.0 - posX) * deltaDist.x;
    }
    if (rayDir.y < 0) {
        step.y = -1;
        sideDist.y = (posY - map.y) * deltaDist.y;
    } else {
        step.y = 1;
        sideDist.y = (map.y + 1.0 - posY) * deltaDist.y;
    }
}

void App::performDDA(int &side, const sf::Vector2<double> &deltaDist, const sf::Vector2i &step, sf::Vector2i map,
                     sf::Vector2<double> &sideDist) {
    int hit = 0; //was there a wall hit?
    while (hit == 0) {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDist.x < sideDist.y) {
            sideDist.x += deltaDist.x;
            map.x += step.x;
            side = 0;
        } else {
            sideDist.y += deltaDist.y;
            map.y += step.y;
            side = 1;
        }
        //Check if ray has hit a wall
        if (worldMap[map.x][map.y] > 0) hit = 1;
    }
}

void App::calculate(const int x, sf::Vector2<double> &rayDir, sf::Vector2i map, double &perpWallDist,
                    int &side,
                    int &lineHeight, int &drawStart, int &drawEnd) const {
    //calculate ray position and direction
    const double cameraX = 2 * x / static_cast<double>(RENDER_WIDTH) - 1; //x-coordinate in camera space
    rayDir = sf::Vector2(dirX + planeX * cameraX, dirY + planeY * cameraX);
    map = sf::Vector2i(static_cast<int>(posX), static_cast<int>(posY));

    //length of ray from current position to next x or y-side
    sf::Vector2<double> sideDist;
    sf::Vector2i step;

    //length of ray from one x or y-side to next x or y-side
    const sf::Vector2<double> deltaDist = sf::Vector2<double>(
        (rayDir.x == 0) ? 1e30 : std::abs(1 / rayDir.x),
        (rayDir.y == 0) ? 1e30 : std::abs(1 / rayDir.y));

    //calculate step and initial sideDist
    calculateStep(rayDir, deltaDist, map, sideDist, step);

    //perform DDA
    performDDA(side, deltaDist, step, map, sideDist);

    //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
    if (side == 0) perpWallDist = (sideDist.x - deltaDist.x);
    else perpWallDist = (sideDist.y - deltaDist.y);
    lineHeight = static_cast<int>(RENDER_HEIGHT / perpWallDist);

    drawStart = -lineHeight / 2 + RENDER_HEIGHT / 2;
    if (drawStart < 0) drawStart = 0;
    drawEnd = lineHeight / 2 + RENDER_HEIGHT / 2;
    if (drawEnd >= RENDER_HEIGHT) drawEnd = RENDER_HEIGHT - 1;
}

void App::getTextureParameters(const sf::Vector2<double> &rayDir, const sf::Vector2i map,
                               const double perpWallDist, const int side, const int lineHeight, const int drawStart,
                               int &texNum, int &texX, double &step, double &texPos) const {
    texNum = worldMap[map.x][map.y] - 1;

    //calculate value of wallX
    double wallX; //where exactly the wall was hit
    if (side == 0) wallX = posY + perpWallDist * rayDir.y;
    else wallX = posX + perpWallDist * rayDir.x;
    wallX -= floor((wallX));

    texX = static_cast<int>(wallX * static_cast<double>(TEX_WIDTH));
    if (side == 0 && rayDir.x > 0) texX = TEX_WIDTH - texX - 1;
    if (side == 1 && rayDir.y < 0) texX = TEX_WIDTH - texX - 1;
    step = 1.0 * TEX_HEIGHT / lineHeight;
    texPos = (drawStart - RENDER_HEIGHT / 2 + lineHeight / 2) * step;
}

void App::drawColumn(const int x, const int side, const int drawStart, const int drawEnd, const int texNum,
                     const int texX, const double step, double texPos) const {
    for (int y = drawStart; y < drawEnd; ++y) {
        // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
        const int texY = static_cast<int>(texPos) & (TEX_HEIGHT - 1);
        texPos += step;
        auto color = textures[texNum]->getPixel(texX, texY);
        // make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
        if (side == 1) color.a /= 2;
        buffer->setPixel(x, y, color);
    }
}

void App::render() const {
    for (int x = 0; x < RENDER_WIDTH; x++) {
        clearBuffer(x);

        sf::Vector2<double> rayDir;
        const sf::Vector2i map;
        double perpWallDist;
        int side, lineHeight, drawStart, drawEnd;

        calculate(x, rayDir, map, perpWallDist,
                  side, lineHeight, drawStart, drawEnd);

        int texNum, texX;
        double step, texPos;
        getTextureParameters(rayDir, map, perpWallDist,
                             side, lineHeight, drawStart, texNum, texX, step, texPos);
        //draw the pixels of the stripe as a vertical line
        drawColumn(x, side, drawStart, drawEnd, texNum, texX, step, texPos);
    }

    texture->update(*buffer);
    window->clear(sf::Color::Black);
    window->draw(*bufferRect);
    window->display();
}
