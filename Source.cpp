#include "Draw.h"

using namespace std;
using namespace sf;


int main() {
	//création des éléments nécéssaires
	Draw drawer;
	DrawIO console;

	// création de la fenêtre
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(WINDOW_X_SIZE_, WINDOW_Y_SIZE_), "GeomeDraw", Style::Default, settings);
	//RenderWindow window(VideoMode(), "GeomeDraw", Style::Fullscreen, settings);//mode capture fond d'écran

	while (window.isOpen())	{
		Event event;
		while (window.pollEvent(event)) if (event.type == Event::Closed) window.close();// fermeture de la fenêtre lorsque l'utilisateur le souhaite

		console.formatUserInput(drawer.getCursor());//récupère potentielle entrée console
		
		if (console.needRefresh()) {//limite les calculs
			drawer.drawAll(window, console.getUserInput());//dessine entrées reçues
			console.setRefresh(false);
		}
	}

	return 0;
}
