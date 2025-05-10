#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
class Entity
{
public:
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual sf::Vector2i getTilePosition() const = 0;
	virtual ~Entity() {}
};

