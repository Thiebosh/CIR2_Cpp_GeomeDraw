#include "DrawIO.h"

using namespace std;
using namespace sf;


DrawIO::DrawIO() : m_refresh(true) { setlocale(LC_CTYPE, "fr-FR"); /*caracteres accentues francais en console*/ }

bool DrawIO::needRefresh() const { return m_refresh; }
void DrawIO::setRefresh(bool const &state) { m_refresh = state; }
vector <LineDetail> DrawIO::getUserInput() const { return m_userInput; }


Color DrawIO::getColor(string &RGBcode, Color const &defaultColor) const {
	size_t pos;
	int values[3] = { 0 };

	for (int index = 0; index < 2; index++) {
		if ((pos = RGBcode.find('-')) != string::npos) {
			if (pos > 0 && isInteger(RGBcode.substr(0, pos))) values[index] = abs(stoi(RGBcode.substr(0, pos))) % 256;
			RGBcode.erase(0, pos + 1);
		}
	}
	if (!RGBcode.empty()) values[2] = (int)stof(RGBcode);

	return Color(values[0], values[1], values[2], 255);
}


map <string, string> DrawIO::splitConsoleLine() {
	map <string, string> detailedLine;
	string consoleLine = GetLine();//appel au thread de saisie non bloquant

	if (!consoleLine.empty()) replace(consoleLine.begin(), consoleLine.end(), ',', '.');//rend les string convertible en float

	//decoupe chaine en duo clé:valeur
	while (!consoleLine.empty()) {
		string currentSplit;
		size_t pos;

		do {
			pos = consoleLine.find(' ') != string::npos ? consoleLine.find(' ') : consoleLine.size();
			if (pos != 0) currentSplit = consoleLine.substr(0, pos);
			consoleLine.erase(0, pos + 1);//efface 1 espace
		} while (currentSplit.empty() && !consoleLine.empty());//récupère contenu entre espaces

		if ((pos = currentSplit.find(':')) != string::npos && currentSplit.size() > pos + 1) {//récupère duo s'il existe
			string key = currentSplit.substr(0, pos);
			string value = currentSplit.substr(pos + 1, currentSplit.size());
			detailedLine.insert(make_pair(key, value));
		}
	}

	//si on a des valeurs et un nom d'ensemble correct, on continue
	if (detailedLine.size() > 1 &&
		(detailedLine.find("cursor") != detailedLine.end() ||
			detailedLine.find("draw") != detailedLine.end() ||
			detailedLine.find("window") != detailedLine.end())) {
		return detailedLine;
	}
	return map <string, string>();//tableau vide
}

void DrawIO::formatUserInput(DrawCursor const &cursor) {
	map <string, string> detailedLine = splitConsoleLine();

	if (!detailedLine.empty()) {
		LineDetail input;

		//1- détermine action
		input.set = detailedLine.find("draw") != detailedLine.end() ? "draw" : detailedLine.find("window") != detailedLine.end() ? "window" : "cursor";//enoughData => forcément l'un des trois
		input.function = !input.set.compare("draw") ? !detailedLine[input.set].compare("line") || !detailedLine[input.set].compare("spiral") || !detailedLine[input.set].compare("mosaic") || !detailedLine[input.set].compare("fractal") ? detailedLine[input.set] : "error" :
						!input.set.compare("cursor") ? !detailedLine[input.set].compare("move") || !detailedLine[input.set].compare("style") || !detailedLine[input.set].compare("origin") ? detailedLine[input.set] : "error" :
														!detailedLine[input.set].compare("display") ? detailedLine[input.set] : "error";

		//2- définit variables
		if (input.function.compare("error")) {//0 si egal
			input.color = detailedLine.find("color") == detailedLine.end() ? input.color = cursor.getColor() :
				!detailedLine["color"].compare("white") ? Color::White :
				!detailedLine["color"].compare("red") ? Color::Red :
				!detailedLine["color"].compare("green") ? Color::Green :
				!detailedLine["color"].compare("blue") ? Color::Blue :
				!detailedLine["color"].compare("cyan") ? Color::Cyan :
				!detailedLine["color"].compare("magenta") ? Color::Magenta :
				!detailedLine["color"].compare("yellow") ? Color::Yellow :
				!detailedLine["color"].compare("black") ? Color::Black : getColor(detailedLine["color"], cursor.getColor());

			input.length = detailedLine.find("advance") != detailedLine.end() ? stof(detailedLine["advance"]) : 0;
			input.rotation = detailedLine.find("rotate") != detailedLine.end() ? DrawCursor::simplifyAngle(stof(detailedLine["rotate"])) : 0;
			input.repeat = detailedLine.find("repeat") != detailedLine.end() && stof(detailedLine["repeat"]) > 1 ? (int)stof(detailedLine["repeat"]) : 1;

			input.radius = detailedLine.find("radius") != detailedLine.end() ? abs(stof(detailedLine["radius"])) < 400 ? abs(stof(detailedLine["radius"])) : 400 : 0;
			input.radVariation = detailedLine.find("variation") != detailedLine.end() ? stof(detailedLine["variation"]) : 1;
			input.nbRevolution = detailedLine.find("revolution") != detailedLine.end() ? stof(detailedLine["revolution"]) : 1;
			input.wrap = detailedLine.find("wrap") != detailedLine.end() && !detailedLine["wrap"].compare("true") ? true : false;

			input.iteration = detailedLine.find("iterate") != detailedLine.end() ? (int)stof(detailedLine["iterate"]) : 0;
			input.fractalName = detailedLine.find("name") == detailedLine.end() ? "error" :
									!detailedLine["name"].compare("flake") ? "flake" :
									!detailedLine["name"].compare("invertedFlake") ? "invertedFlake" :
									!detailedLine["name"].compare("squareFlake") ? "squareFlake" :
									!detailedLine["name"].compare("triangle") ? "triangle" :
									!detailedLine["name"].compare("tree") ? "tree" :
									!detailedLine["name"].compare("fern") ? "fern" : "error";

			input.originRotation = cursor.getOriginRot() + (detailedLine.find("rotate") != detailedLine.end() ? DrawCursor::simplifyAngle(stof(detailedLine["rotate"])) : 0);
			input.originCoord.x = cursor.getOriginCoord().x + (detailedLine.find("xCoord") != detailedLine.end() ? stof(detailedLine["xCoord"]) : 0);
			input.originCoord.y = cursor.getOriginCoord().y + (detailedLine.find("yCoord") != detailedLine.end() ? stof(detailedLine["yCoord"]) : 0);

			input.coord.x = cursor.getCoord().x + (detailedLine.find("xCoord") != detailedLine.end() ? stof(detailedLine["xCoord"]) : 0);
			input.coord.y = cursor.getCoord().y + (detailedLine.find("yCoord") != detailedLine.end() ? stof(detailedLine["yCoord"]) : 0);

			input.visible = detailedLine.find("visible") != detailedLine.end() ? (!detailedLine["visible"].compare("show") ? true : !detailedLine["visible"].compare("hide") ? false : cursor.isVisible()) : cursor.isVisible();
			input.thickness = detailedLine.find("thickness") != detailedLine.end() && stoi(detailedLine["thickness"]) > 1 ? stoi(detailedLine["thickness"]) : cursor.getThickness();


			if (detailedLine.find("clearLast") != detailedLine.end() && !detailedLine["clearLast"].compare("true")) m_userInput.pop_back();
			else if (detailedLine.find("clear") != detailedLine.end() && !detailedLine["clear"].compare("true")) m_userInput.clear();
			else m_userInput.push_back(input);
			m_refresh = true;
		}
	}
}
