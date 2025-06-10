#include "AnimatedExplosion.h"

AnimatedExplosion::AnimatedExplosion(const std::vector<sf::Texture*>& frameTextures, sf::Vector2f pos, float tileSize)
    : frames(frameTextures), position(pos), tileSize(tileSize), timer(0.f), currentFrame(0), finished(false)
{
    totalFrames = static_cast<int>(frames.size());
    frameTime = 0.1f;

    if (!frames.empty()) {
        sprite.setTexture(*frames[0]);
    }

    sprite.setPosition(position);

    // Масштабируем, если размер текстуры не совпадает с размером тайла
    sf::Vector2u texSize = sprite.getTexture()->getSize();
    float scaleX = tileSize / static_cast<float>(texSize.x);
    float scaleY = tileSize / static_cast<float>(texSize.y);
    sprite.setScale(scaleX, scaleY);
}

void AnimatedExplosion::update(float dt) {
    if (finished) return;

    timer += dt;
    if (timer >= frameTime) {
        timer = 0.f;
        ++currentFrame;
        if (currentFrame >= totalFrames) {
            finished = true;
        } else {
            sprite.setTexture(*frames[currentFrame]);
        }
    }
}

void AnimatedExplosion::draw(sf::RenderWindow& window) {
    if (!finished)
        window.draw(sprite);
}

sf::Vector2i AnimatedExplosion::getTilePosition() const {
    return sf::Vector2i(
        static_cast<int>(position.x / tileSize),
        static_cast<int>(position.y / tileSize)
    );
}

bool AnimatedExplosion::isFinished() const {
    return finished;
}
