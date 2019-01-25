#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <map>


#define WINDOW_X_SIZE_ VideoMode::getDesktopMode().width
#define WINDOW_Y_SIZE_ VideoMode::getDesktopMode().height


class DrawCursor
{
protected:
	bool m_visible;
	int m_thickness;
	sf::Color m_color;

	sf::Vector2f m_coord;
	float m_rotation;

	sf::Vector2f m_originCoord;
	float m_originRotation;

public:
	static float simplifyAngle(float angle);

	DrawCursor();


	bool isVisible() const;
	int getThickness() const;
	sf::Color getColor() const;

	sf::Vector2f getCoord() const;
	float getRotation() const;

	float getOriginRot() const;
	sf::Vector2f getOriginCoord() const;


	void setVisible(bool visible);
	void setThickness(int thickness);
	void setColor(sf::Color color);

	void setCoord(sf::Vector2f coord);
	void setCoordX(float xAxis);
	void setCoordY(float yAxis);
	void setRotation(float rotation);


	void setOrigin(sf::Vector2f coord, float rotation);
	void applyOrigin();


	void drawCursor(sf::RenderWindow& window) const;
};

