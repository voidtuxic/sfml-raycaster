//
// Created by tuxic on 21/12/2023.
//

#include "App.h"

#include <cmath>

App::App() {
    window = new sf::RenderWindow(sf::VideoMode(screenWidth, screenHeight), "Raycaster");
    window->setVerticalSyncEnabled(true);

    buffer = new sf::Image();
    buffer->create(screenWidth, screenHeight, sf::Color::Black);

    texture = new sf::Texture();
    texture->loadFromImage(*buffer);

    bufferSprite = new sf::Sprite();
    bufferSprite->setTexture(*texture, true);
}

App::~App() {
    for (const auto image: textures) {
        free(image);
    }
    textures.clear();
    free(bufferSprite);
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
    textures[0]->loadFromFile("textures/eagle.png");
    textures[1]->loadFromFile("textures/redbrick.png");
    textures[2]->loadFromFile("textures/purplestone.png");
    textures[3]->loadFromFile("textures/greystone.png");
    textures[4]->loadFromFile("textures/bluestone.png");
    textures[5]->loadFromFile("textures/mossy.png");
    textures[6]->loadFromFile("textures/wood.png");
    textures[7]->loadFromFile("textures/colorstone.png");

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
        if (worldMap[static_cast<int>(posX + dirX * moveSpeed * wallDistance)][static_cast<int>(posY)] == false)
            posX += dirX * moveSpeed;
        if (worldMap[static_cast<int>(posX)][static_cast<int>(posY + dirY * moveSpeed * wallDistance)] == false)
            posY += dirY * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
        if (worldMap[static_cast<int>(posX - dirX * moveSpeed * wallDistance)][static_cast<int>(posY)] == false)
            posX -= dirX * moveSpeed;
        if (worldMap[static_cast<int>(posX)][static_cast<int>(posY - dirY * moveSpeed * wallDistance)] == false)
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
    for (int y = 0; y < screenHeight; y++) {
        buffer->setPixel(x, y, sf::Color::Black);
    }
}

void App::calculateStep(const double &rayDirX, const double &rayDirY, const int &mapX, const int &mapY,
                        double &sideDistX, double &sideDistY, const double deltaDistX, const double deltaDistY,
                        int &stepX, int &stepY) const {
    //what direction to step in x or y-direction (either +1 or -1)
    if (rayDirX < 0) {
        stepX = -1;
        sideDistX = (posX - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if (rayDirY < 0) {
        stepY = -1;
        sideDistY = (posY - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - posY) * deltaDistY;
    }
}

void App::performDDA(int &mapX, int &mapY, int &side, double &sideDistX, double &sideDistY, const double deltaDistX,
                     const double deltaDistY, const int stepX, const int stepY) {
    int hit = 0; //was there a wall hit?
    while (hit == 0) {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }
        //Check if ray has hit a wall
        if (worldMap[mapX][mapY] > 0) hit = 1;
    }
}

void App::calculate(const int x, double &rayDirX, double &rayDirY, int &mapX, int &mapY, double &perpWallDist,
                    int &side,
                    int &lineHeight, int &drawStart, int &drawEnd) const {
    //calculate ray position and direction
    const double cameraX = 2 * x / static_cast<double>(screenWidth) - 1; //x-coordinate in camera space
    rayDirX = dirX + planeX * cameraX;
    rayDirY = dirY + planeY * cameraX;
    mapX = static_cast<int>(posX);
    mapY = static_cast<int>(posY);

    //length of ray from current position to next x or y-side
    double sideDistX, sideDistY;
    int stepX, stepY;

    //length of ray from one x or y-side to next x or y-side
    double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
    double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

    //calculate step and initial sideDist
    calculateStep(rayDirX, rayDirY, mapX, mapY, sideDistX, sideDistY, deltaDistX, deltaDistY, stepX, stepY);

    //perform DDA
    performDDA(mapX, mapY, side, sideDistX, sideDistY, deltaDistX, deltaDistY, stepX, stepY);

    //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
    if (side == 0) perpWallDist = (sideDistX - deltaDistX);
    else perpWallDist = (sideDistY - deltaDistY);
    lineHeight = static_cast<int>(screenHeight / perpWallDist);

    drawStart = -lineHeight / 2 + screenHeight / 2;
    if (drawStart < 0) drawStart = 0;
    drawEnd = lineHeight / 2 + screenHeight / 2;
    if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;
}

void App::getTextureParameters(const double rayDirX, const double rayDirY, const int mapX, const int mapY,
                               const double perpWallDist, const int side, const int lineHeight, const int drawStart,
                               int &texNum, int &texX, double &step, double &texPos) const {
    texNum = worldMap[mapX][mapY] - 1;

    //calculate value of wallX
    double wallX; //where exactly the wall was hit
    if (side == 0) wallX = posY + perpWallDist * rayDirY;
    else wallX = posX + perpWallDist * rayDirX;
    wallX -= floor((wallX));

    texX = static_cast<int>(wallX * static_cast<double>(texWidth));
    if (side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
    if (side == 1 && rayDirY < 0) texX = texWidth - texX - 1;
    step = 1.0 * texHeight / lineHeight;
    texPos = (drawStart - screenHeight / 2 + lineHeight / 2) * step;
}

void App::drawColumn(const int x, const int side, const int drawStart, const int drawEnd, const int texNum,
                     const int texX, const double step, double texPos) const {
    for (int y = drawStart; y < drawEnd; ++y) {
        // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
        const int texY = static_cast<int>(texPos) & (texHeight - 1);
        texPos += step;
        auto color = textures[texNum]->getPixel(texX, texY);
        // make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
        if (side == 1) color.a /= 2;
        buffer->setPixel(x, y, color);
    }
}

void App::render() const {
    for (int x = 0; x < screenWidth; x++) {
        clearBuffer(x);

        double rayDirX, rayDirY;
        int mapX, mapY;
        double perpWallDist;
        int side, lineHeight, drawStart, drawEnd;

        calculate(x, rayDirX, rayDirY, mapX, mapY, perpWallDist,
                  side, lineHeight, drawStart, drawEnd);

        int texNum, texX;
        double step, texPos;
        getTextureParameters(rayDirX, rayDirY, mapX, mapY, perpWallDist,
                             side, lineHeight, drawStart, texNum, texX, step, texPos);
        //draw the pixels of the stripe as a vertical line
        drawColumn(x, side, drawStart, drawEnd, texNum, texX, step, texPos);
    }

    texture->update(*buffer);
    window->clear(sf::Color::Black);
    window->draw(*bufferSprite);
    window->display();
}
