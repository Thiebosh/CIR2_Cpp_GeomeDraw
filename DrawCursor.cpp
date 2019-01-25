#include "DrawCursor.h"

using namespace std;
using namespace sf;





float DrawCursor::simplifyAngle(float angle) {
	float reducedAngle = ((int)angle) % 360 + (angle - ((int)angle));
	return reducedAngle > 0 ? reducedAngle : reducedAngle + 360;
}



DrawCursor::DrawCursor(): m_visible(true), m_thickness(2), m_color(Color::Yellow), m_coord(WINDOW_X_SIZE_ / 2, WINDOW_Y_SIZE_ / 2), m_originCoord(m_coord), m_rotation(0), m_originRotation(-90) {}



bool DrawCursor::isVisible() const		{ return m_visible; }
int DrawCursor::getThickness() const	{ return m_thickness; }
Color DrawCursor::getColor() const		{ return m_color; }

Vector2f DrawCursor::getCoord() const	{ return m_coord; }
float DrawCursor::getRotation() const	{ return m_rotation; }

float DrawCursor::getOriginRot() const		{ return m_originRotation; }
Vector2f DrawCursor::getOriginCoord() const { return m_originCoord; }



void DrawCursor::setVisible(bool visible)		{ m_visible = visible; }
void DrawCursor::setThickness(int thickness)	{ m_thickness = thickness > 0 ? thickness : 1; }
void DrawCursor::setColor(Color color)			{ m_color = color; }

void DrawCursor::setCoord(Vector2f coord)		{ m_coord = coord; }
void DrawCursor::setCoordX(float xAxis)			{ m_coord = Vector2f(xAxis, m_coord.y); }
void DrawCursor::setCoordY(float yAxis)			{ m_coord = Vector2f(m_coord.x, yAxis); }
void DrawCursor::setRotation(float rotation)	{ m_rotation = DrawCursor::simplifyAngle(rotation); }



void DrawCursor::setOrigin(sf::Vector2f coord, float rotation) {
	m_originCoord = coord;
	m_originRotation = rotation;
}
void DrawCursor::applyOrigin() {
	m_coord = m_originCoord;
	m_rotation = m_originRotation;
}



void DrawCursor::drawCursor(RenderWindow& window) const {
	//définit la forme du curseur et la met au bout du tracé
	if (m_visible) {
		ConvexShape cursorView;
		cursorView.setPointCount(4);
		cursorView.setFillColor(m_color);

		cursorView.setPoint(0, Vector2f(0, 0));
		cursorView.setPoint(2, Vector2f(-12, 0));
		cursorView.setPoint(1, Vector2f(-4-12, 6));
		cursorView.setPoint(3, Vector2f(-4-12, -6));

		cursorView.setPosition(m_coord);
		cursorView.setRotation(m_rotation);

		window.draw(cursorView);
	}

	window.display();//affiche la frame dans tous les cas
}