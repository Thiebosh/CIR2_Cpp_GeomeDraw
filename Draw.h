#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include "DrawIO.h"

class Draw;//dépendance du pointeur de fonction


typedef std::vector <sf::Vertex>(*functionPointer)(Draw&, LineDetail &/*, ...*/);//definit type des fonctions pour les stocker en tableau : (*(tab["clé"]))(arguments);
//variadic function c++

class Draw {
protected:
	DrawCursor m_cursor;
	std::map <std::string, std::map <std::string, functionPointer>> m_functionCall;

public:
	Draw();

	DrawCursor getCursor() const;

	void drawAll(sf::RenderWindow &window, std::vector <LineDetail> const &userInput);

	void cursorOrigin(LineDetail &lineInput);
	void cursorMove(LineDetail &lineInput);
	void cursorStyle(LineDetail &lineInput);
	std::vector <sf::Vertex> drawLine(sf::Color const &color, float const &length, float const &rotation, int repetition, int const &thickness);
	std::vector <sf::Vertex> drawSpiral(sf::Color const &color, float radius, float const &revolution, float const &variation, int repetition, bool const &wrap);
	std::vector <sf::Vertex> drawMosaic(sf::Color const &color, float radius, float const &revolution, float const &variation, int repetition, bool const &wrap);
	std::vector <sf::Vertex> drawFractal(LineDetail &lineInput);

	void flake(std::vector <sf::Vertex> &assembly, int const &iteration, float const &length);
	void squareFlake(std::vector <sf::Vertex> &assembly, int const &iteration, float const &length);
	void triangle(std::vector <sf::Vertex> &assembly, int const &iteration, float const &length);
	void tree(std::vector <sf::Vertex> &assembly, int const &iteration, float const &length);
	void fern(std::vector <sf::Vertex> &assembly, int const &iteration, float const &length);
};

