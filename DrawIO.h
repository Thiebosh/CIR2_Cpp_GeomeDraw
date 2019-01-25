#pragma once
#include "AsyncGetline.h"
#include "DrawCursor.h"
#include <locale.h> //caracteres accentues en console

inline bool isInteger(std::string const &s) { return count_if(s.begin(), s.end(), isdigit) == s.size(); }

typedef struct LineDetail_ {
	//commun
	std::string set;
	std::string function;

	//cursor:origin
	float originRotation;
	sf::Vector2f originCoord;

	//cursor:move
	//float rotation;
	sf::Vector2f coord;

	//cursor:pimp
	bool visible;
	int thickness;
	sf::Color color;

	//draw:line
	int repeat;
	float length;
	float rotation;

	//draw:mosaic
	float radius;
	float radVariation;//spiral : radVariation = 1 : cercle
	float nbRevolution;//de 0.1 à ... | nbRevolution = 1 : 1 cercle => le signe donne le sens
	bool wrap;


	//fractal
	std::string fractalName;
	int iteration;


	//file:save

	//file:load
} LineDetail;


class DrawIO : public AsyncGetline {
private:
	std::map <std::string, std::string> splitConsoleLine();
	sf::Color getColor(std::string &RGBcode, sf::Color const &defaultColor) const;

protected:
	std::vector <LineDetail> m_userInput;
	bool m_refresh;

public:
	DrawIO();

	bool needRefresh() const;
	void setRefresh(bool const &state);

	std::vector <LineDetail> getUserInput() const;

	void formatUserInput(DrawCursor const &cursor);
};

