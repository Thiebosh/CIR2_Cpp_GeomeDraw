#include "Draw.h"

using namespace std;
using namespace sf;


int main() {
	//cr�ation des �l�ments n�c�ssaires
	Draw drawer;
	DrawIO console;

	// cr�ation de la fen�tre
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(WINDOW_X_SIZE_, WINDOW_Y_SIZE_), "GeomeDraw", Style::Default, settings);
	//RenderWindow window(VideoMode(), "GeomeDraw", Style::Fullscreen, settings);//mode capture fond d'�cran

	while (window.isOpen())	{
		Event event;
		while (window.pollEvent(event)) if (event.type == Event::Closed) window.close();// fermeture de la fen�tre lorsque l'utilisateur le souhaite

		console.formatUserInput(drawer.getCursor());//r�cup�re potentielle entr�e console
		
		if (console.needRefresh()) {//limite les calculs
			drawer.drawAll(window, console.getUserInput());//dessine entr�es re�ues
			console.setRefresh(false);
		}
	}

	return 0;
}
