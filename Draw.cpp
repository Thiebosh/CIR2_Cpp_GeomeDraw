#include "Draw.h"
#include <Windows.h>

using namespace std;
using namespace sf;


//cursor
vector <Vertex> localCursorOrigin(Draw &ref, LineDetail &lineInput) {
	ref.cursorOrigin(lineInput);
	return vector <Vertex>();//tableau vide
}

vector <Vertex> localCursorMove(Draw &ref, LineDetail &lineInput) {
	ref.cursorMove(lineInput);
	return vector <Vertex>();//tableau vide
}

vector <Vertex> localCursorStyle(Draw &ref, LineDetail &lineInput) {
	ref.cursorStyle(lineInput);
	return vector <Vertex>();//tableau vide
}

//draw
vector <Vertex> localDrawLine(Draw &ref, LineDetail &lineInput) { return ref.drawLine(lineInput.color, lineInput.length, lineInput.rotation, lineInput.repeat, lineInput.thickness); }

vector <Vertex> localDrawSpiral(Draw &ref, LineDetail &lineInput) { return ref.drawSpiral(lineInput.color, lineInput.radius, lineInput.nbRevolution, lineInput.radVariation, lineInput.repeat, lineInput.wrap); }

vector <Vertex> localDrawMosaic(Draw &ref, LineDetail &lineInput) { return ref.drawMosaic(lineInput.color, lineInput.radius, lineInput.nbRevolution, lineInput.radVariation, lineInput.repeat, lineInput.wrap); }

vector <Vertex> localDrawFractal(Draw &ref, LineDetail &lineInput) { return ref.drawFractal(lineInput); }

//file



Draw::Draw() {//création du tableau de fonctions
	map <string, functionPointer> drawCall = {//=> retourne VertexArray
		{ "line",  &localDrawLine },//(*(drawCall["line"]))(paramètres); 
		{ "spiral", &localDrawSpiral },
		{ "mosaic", &localDrawMosaic },
		{ "fractal", &localDrawFractal }
	};

	map <string, functionPointer> cursorCall = {//=>retournerait void
		{ "origin", &localCursorOrigin },//position et rotation du curseur à l'origine du tracé
		{ "move", &localCursorMove },//position, rotation
		{ "style", &localCursorStyle }//épaisseur, couleur, visible
	};

	m_functionCall.insert(make_pair("cursor", cursorCall));
	m_functionCall.insert(make_pair("draw", drawCall));
}


DrawCursor Draw::getCursor() const { return m_cursor; }


void Draw::drawAll(RenderWindow &window, vector <LineDetail> const &userInput) {
	window.clear(Color::Black);//réinitialise la frame
	m_cursor.applyOrigin();//réinitialise curseur

	//récupère l'ensemble des lignes et les dessine progressivement
	VertexArray assembly(Lines);//2 entrées pour une ligne
	int i = 0;
	for (auto line : userInput) {
		for (auto point : (*(m_functionCall[line.set][line.function]))(*this, line)) {
			assembly.append(point);
			
			if (i++ % 2 && line.function.compare("fractal")) {//display:progressive
				window.draw(assembly);
				window.display();
				Sleep(10);
			}
		}
		m_cursor.setColor(line.color);
	}
	window.draw(assembly);//remplit la frame au cas ou (fractales)
	
	m_cursor.drawCursor(window);//ajoute potentiellement le curseur à la frame

	//affiche résumé de tout
	system("cls");
	for (auto line : userInput) {
		if (!line.function.compare("line")) cout << "Ligne - longueur : " << line.length << " - rotation : " << line.rotation << " - épaisseur : " << line.thickness << " - répétition : " << line.repeat << endl;
		else if (!line.function.compare("spiral")) cout << "Spirale - rayon : " << line.radius << " - nb de révolution : " << line.nbRevolution << " - variation : " << line.radVariation << " - répétition : " << line.repeat << endl;
		else if (!line.function.compare("mosaic")) cout << "Mosaïque - rayon : " << line.radius << " - nb de révolution : " << line.nbRevolution << " - variation : " << line.radVariation << " - répétition : " << line.repeat << endl;
		else if (!line.function.compare("fractal")) cout << "Fractale - nom : " << line.fractalName << " - nb d'itération : " << line.iteration << " - longueur : " << line.length << endl;
	}
	cout << endl << " Commande à exécuter : ";//réaffiche pour continuité
}



void Draw::cursorOrigin(LineDetail &lineInput) { m_cursor.setOrigin(lineInput.originCoord, lineInput.originRotation); }

void Draw::cursorMove(LineDetail &lineInput) { 
	m_cursor.setCoord(lineInput.coord);
	m_cursor.setRotation(lineInput.rotation);
}

void Draw::cursorStyle(LineDetail &lineInput) {
	m_cursor.setColor(lineInput.color);
	m_cursor.setVisible(lineInput.visible);
	m_cursor.setThickness(lineInput.thickness);
}




//draw:line advance:90 rotate:90 repeat:4 => carré (pour damier, utilise toute les combinaisons de positif et negatif)
//draw:line advance:20 rotate:20 repeat:18 => cercle (peu pratique)

vector <Vertex> Draw::drawLine(Color const &color, float const &length, float const &rotation = 0, int repetition = 1, int const &thickness = 1) {
	vector <Vertex> assembly;//tableau de lignes (2 entrées par ligne)

	do {
		if (length < 0) m_cursor.setRotation(m_cursor.getRotation() + 180);//retourne le curseur
		for (int i = thickness; i >= 0; i--) {
			float cursorRotation = DrawCursor::simplifyAngle(m_cursor.getRotation());

			//premier point : continuité
			assembly.push_back(Vertex(Vector2f(m_cursor.getCoord().x - (m_cursor.getRotation() != 0 && m_cursor.getRotation() != 180 ? i : 0), 
												m_cursor.getCoord().y - (m_cursor.getRotation() != 90 && m_cursor.getRotation() != 270 ? i : 0)), color));//gère épaisseur

			//second point : calculé selon rotation
			RectangleShape sketch(Vector2f(abs(length), 0));//modélise avec épaisseur 0 pour éviter d'agrandir la bounding box
			sketch.rotate(cursorRotation);
			sketch.setPosition(Vector2f(m_cursor.getCoord().x - (m_cursor.getRotation() != 0 && m_cursor.getRotation() != 180? i : 0), 
										m_cursor.getCoord().y - (m_cursor.getRotation() != 90 && m_cursor.getRotation() != 270? i : 0)));//gère épaisseur

			Vector2f tmp = (0 <= cursorRotation && cursorRotation < 90) ? Vector2f(sketch.getGlobalBounds().left + sketch.getGlobalBounds().width, sketch.getGlobalBounds().top + sketch.getGlobalBounds().height) ://lowerRightCorner
							(90 <= cursorRotation && cursorRotation < 180) ? Vector2f(sketch.getGlobalBounds().left, sketch.getGlobalBounds().top + sketch.getGlobalBounds().height) ://lowerLeftCorner
							(180 <= cursorRotation && cursorRotation < 270) ? Vector2f(sketch.getGlobalBounds().left, sketch.getGlobalBounds().top) ://upperLeftCorner
																				Vector2f(sketch.getGlobalBounds().left + sketch.getGlobalBounds().width, sketch.getGlobalBounds().top);//upperRightCorner

			assembly.push_back(Vertex(tmp, color));//applique second point

			if (!i) m_cursor.setCoord(tmp);//récupère coordonnées finales sans modif
		}
		m_cursor.setRotation(m_cursor.getRotation() + rotation);//retourne le curseur
		if (length < 0) m_cursor.setRotation(m_cursor.getRotation() - 180);
	} while (--repetition > 0);

	return assembly;
}


//CONTINUITE
//draw:spiral radius:40 revolution:0.25 variation:1 repeat:4 => maitrise (peut utiliser toutes les combinaisons de positif et négatif pour l'éprouver)

//EN PLUS
//draw:spiral radius:60 revolution:20 variation:-1.68 => spirale intérieure
//draw:spiral radius:5 revolution:5 variation:1.5 repeat:10 => spirale "mosaique"
//draw:spiral radius:5 revolution:5 variation:1.5 repeat:10 wrap:true => spirale "mosaique" 3D
//draw:spiral radius:1 revolution:4 variation:2.4 repeat:20 => pentagone
//draw:spiral radius:1 revolution:4 variation:2.4 repeat:20 wrap:true => mini soleil
//draw:spiral radius:30 revolution:10 variation:8 wrap:true => etoile / lotus
//draw:spiral radius:28 revolution:10 variation:20 wrap:true => escalier
//draw:spiral radius:800 revolution:1.4 variation:-25 wrap:true => carré plein

vector <Vertex> Draw::drawSpiral(Color const &color, float radius, float const &revolution = 1, float const &variation = 1, int repetition = 1, bool const &wrap = false) {
	vector <Vertex> assembly;

	double nbPoints = 2 * sqrt(2 * M_PI * radius) * abs(revolution);//prend un peu plus de points que la moitié du périmètre
	LineDetail virtualRadius;
	virtualRadius.length = radius;
	virtualRadius.rotation = (float)(360 * revolution / nbPoints);//écart angulaire entre deux points
	if (wrap) virtualRadius.rotation *= variation; //applique variation du rayon

	do {
		int nbPointsRounded = (int)ceil(nbPoints);
		while (nbPointsRounded-- > 0) {
			assembly.push_back(Vertex(m_cursor.getCoord(), color));//premier point du premier segment (origine)
			
			//applique rotation
			m_cursor.setRotation(m_cursor.getRotation() + virtualRadius.rotation);

			RectangleShape sketch(Vector2f(virtualRadius.length, 0));//modélise avec épaisseur 0 pour éviter d'agrandir la bounding box
			sketch.setPosition(m_cursor.getCoord());
			sketch.rotate(m_cursor.getRotation());

			virtualRadius.length += variation > 0 ? log(variation) : -log(abs(variation));
			float cursorRotation = DrawCursor::simplifyAngle(m_cursor.getRotation());

			m_cursor.setCoord((0 <= cursorRotation && cursorRotation < 90) ? Vector2f(sketch.getGlobalBounds().left + sketch.getGlobalBounds().width, sketch.getGlobalBounds().top + sketch.getGlobalBounds().height) ://lowerRightCorner
				(90 <= cursorRotation && cursorRotation < 180) ? Vector2f(sketch.getGlobalBounds().left, sketch.getGlobalBounds().top + sketch.getGlobalBounds().height) ://lowerLeftCorner
				(180 <= cursorRotation && cursorRotation < 270) ? Vector2f(sketch.getGlobalBounds().left, sketch.getGlobalBounds().top) ://upperLeftCorner
				Vector2f(sketch.getGlobalBounds().left + sketch.getGlobalBounds().width, sketch.getGlobalBounds().top));//upperRightCorner

			assembly.push_back(Vertex(m_cursor.getCoord(), color));//second point du segment précédent
		}
	} while (--repetition > 0);

	return assembly;
}


//CONTINUITE
//draw:mosaic radius:40 revolution:0.25 repeat:4 => léger décalages => fonction différente
//draw:mosaic radius:10 revolution:20 variation:1.5 => spirale
//draw:mosaic radius:10 revolution:40 variation:1.8 wrap:true => spirale "mosaique"
//draw:mosaic radius:30 revolution:10 variation:8 wrap:true => etoile / lotus
//draw:mosaic radius:40 revolution:10 variation:8 wrap:true => escalier
//draw:mosaic radius:800 revolution:1.4 variation:-25 wrap:true => carré plein

//EN PLUS
//draw:mosaic radius:36 repeat:66 => rosace
//draw:mosaic radius:30 repeat:51 => tore
//draw:mosaic radius:5 revolution:5 variation:1.5 repeat:10 => tourbillon
//draw:mosaic radius:6 revolution:6 variation:2 repeat:6 wrap:true => tourbillon 2
//draw:mosaic radius:5 revolution:5 variation:1.5 repeat:10 wrap:true => fleur épaisse
//draw:mosaic radius:5 variation:1.2 repeat:40  => escargot
//draw:mosaic radius:5 revolution:5 variation:1.1 repeat:30 => fleur escargot
//draw:mosaic radius:5 revolution:5 variation:1.1 repeat:30 wrap:true => rose
//draw:mosaic radius:5 variation:1.2 repeat:40 wrap:true => fleur "triangle"
//draw:mosaic radius:10 revolution:30 variation:2.4 wrap:true => fleur multitude petites pétales
//draw:mosaic radius:6 revolution:6 variation:2.1 repeat:15 wrap:true => petite tornade
//draw:mosaic radius:6 revolution:6 variation:2.4 repeat:20 wrap:true => moyenne tornade
//draw:mosaic radius:10 revolution:6 variation:2.4 repeat:20 wrap:true => grosse tornade
//draw:mosaic radius:1 revolution:6 variation:2.4 repeat:14 => fleur petales pentagonales
//draw:mosaic radius:1 revolution:6 variation:2.4 repeat:14 wrap:true => arc étoilé
//draw:mosaic radius:1 revolution:4 variation:2 repeat:15 wrap:true => étoiles condensées
//draw:mosaic radius:1 revolution:3 variation:2.4 repeat:50 => tourbillon de petales pentagonales
//draw:mosaic radius:1 revolution:3 variation:2.4 repeat:50 wrap:true => tourbillon d'étoiles
//draw:mosaic radius:20 revolution:6 variation:12 repeat:4 wrap:true => constellation

vector <Vertex> Draw::drawMosaic(Color const &color, float radius, float const &revolution = 1, float const &variation = 1, int repetition = 1, bool const &wrap = false) {
	vector <Vertex> assembly;

	double nbPoints = 2 * sqrt(2 * M_PI * radius) * abs(revolution);//prend un peu plus de points que la moitié du périmètre
	LineDetail virtualRadius;
	virtualRadius.length = radius;
	virtualRadius.rotation = (float) (360 * revolution / nbPoints);//écart angulaire entre deux points
	if (wrap) virtualRadius.rotation *= variation;

	Vertex circlePoint(m_cursor.getCoord(), color);//premier point du premier segment (origine)
	do {
		int nbPointsRounded = (int)ceil(nbPoints);
		do {
			assembly.push_back(circlePoint);//second point du segment précédent

			circlePoint = Vertex(m_cursor.getCoord(), color);//premier point du segment suivant (récupère points du cercle)
			assembly.push_back(circlePoint);

			//applique rotation
			m_cursor.setRotation(m_cursor.getRotation() + virtualRadius.rotation);

			RectangleShape sketch(Vector2f(virtualRadius.length, 0));//modélise avec épaisseur 0 pour éviter d'agrandir la bounding box
			sketch.setPosition(m_cursor.getCoord());
			sketch.rotate(m_cursor.getRotation());

			float cursorRotation = DrawCursor::simplifyAngle(m_cursor.getRotation());
			m_cursor.setCoord((0 <= cursorRotation && cursorRotation < 90) ?		Vector2f(sketch.getGlobalBounds().left + sketch.getGlobalBounds().width, sketch.getGlobalBounds().top + sketch.getGlobalBounds().height) ://lowerRightCorner
								(90 <= cursorRotation && cursorRotation < 180) ?	Vector2f(sketch.getGlobalBounds().left, sketch.getGlobalBounds().top + sketch.getGlobalBounds().height) ://lowerLeftCorner
								(180 <= cursorRotation && cursorRotation < 270) ?	Vector2f(sketch.getGlobalBounds().left, sketch.getGlobalBounds().top) ://upperLeftCorner
																					Vector2f(sketch.getGlobalBounds().left + sketch.getGlobalBounds().width, sketch.getGlobalBounds().top));//upperRightCorner

			virtualRadius.length += variation > 0 ? log(variation) : -log(abs(variation));
		} while (--nbPointsRounded > 0);
		m_cursor.setCoord(assembly[assembly.size() - 1].position);//subtil décalage du curseur pour créer effet
	} while (--repetition > 0);
	m_cursor.setCoord(assembly[assembly.size() - 1].position);//dernier élément tracé
	
	return assembly;
}


//BONUS
//draw:fractal name:flake advance:400 iterate:4
//draw:fractal name:invertedFlake advance:400 iterate:4
//draw:fractal name:squareFlake advance:300 iterate:4
//draw:fractal name:triangle advance:500 iterate:4
//draw:fractal name:tree advance:500 iterate:8
//draw:fractal name:fern advance:100 iterate:8

vector <Vertex> Draw::drawFractal(LineDetail &lineInput) {
	vector <Vertex> assembly;
	
	if (!lineInput.fractalName.compare("flake")) {
		for (int i = 0; i < 3; i++) {
			flake(assembly, lineInput.iteration, lineInput.length);
			m_cursor.setRotation(m_cursor.getRotation() - 120);
		}
	}
	else if (!lineInput.fractalName.compare("invertedFlake")) {
		for (int i = 0; i < 3; i++) {
			flake(assembly, lineInput.iteration, lineInput.length);
			m_cursor.setRotation(m_cursor.getRotation() + 120);
		}
	}
	else if (!lineInput.fractalName.compare("squareFlake")) {
		for (int i = 0; i < 4; i++) {
			m_cursor.setRotation(m_cursor.getRotation() + 45);
			squareFlake(assembly, lineInput.iteration, lineInput.length);
			m_cursor.setRotation(m_cursor.getRotation() - 135);
		}
	}
	else if (!lineInput.fractalName.compare("triangle")) {
		m_cursor.setRotation(m_cursor.getRotation() + 90);
		triangle(assembly, lineInput.iteration, lineInput.length);
		m_cursor.setRotation(m_cursor.getRotation() - 90);
	}
	else if (!lineInput.fractalName.compare("tree")) tree(assembly, lineInput.iteration, lineInput.length);
	else if (!lineInput.fractalName.compare("fern")) fern(assembly, lineInput.iteration, lineInput.length);

	return assembly;
}

void Draw::flake(vector <Vertex> &assembly, int const &iteration, float const &length) {
	if (iteration == 0) for (auto point : drawLine(Color::Blue, length)) assembly.push_back(point);
	else {
		flake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() + 60);

		flake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() - 120);

		flake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() + 60);

		flake(assembly, iteration - 1, length / 3);
	}
}

void Draw::squareFlake(vector <Vertex> &assembly, int const &iteration, float const &length) {
	if (iteration == 0) for (auto point : drawLine(Color::Red, length)) assembly.push_back(point);
	else {
		squareFlake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() - 90);

		squareFlake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() + 90);

		squareFlake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() + 90);

		squareFlake(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() - 90);

		squareFlake(assembly, iteration - 1, length / 3);
	}
}

void Draw::triangle(vector <Vertex> &assembly, int const &iteration, float const &length) {
	if (iteration == 0) for (int i = 0; i < 3; i++) for (auto point : drawLine(Color::Yellow, length, -120)) assembly.push_back(point);
	else {
		triangle(assembly, iteration - 1, length / 2);

		for (auto point : drawLine(Color::Yellow, length / 2)) assembly.push_back(point);

		triangle(assembly, iteration - 1, length / 2);

		for (auto point : drawLine(Color::Yellow, -length / 2, -60)) assembly.push_back(point);

		for (auto point : drawLine(Color::Yellow, length / 2, 60)) assembly.push_back(point);

		triangle(assembly, iteration - 1, length / 2);

		m_cursor.setRotation(m_cursor.getRotation() - 60);

		for (auto point : drawLine(Color::Yellow, -length / 2, 60)) assembly.push_back(point);
	}
}

void Draw::tree(vector <Vertex> &assembly, int const &iteration, float const &length) {
	if (iteration == 0) {
		for (auto point : drawLine(Color::Green, length)) assembly.push_back(point);
		drawLine(Color::Green, -length);
	}
	else {
		for (auto point : drawLine(Color::White, length / 3, -30)) assembly.push_back(point);

		tree(assembly, iteration - 1, 2 * length / 3);

		m_cursor.setRotation(m_cursor.getRotation() + 60);

		tree(assembly, iteration - 1, 2 * length / 3);

		m_cursor.setRotation(m_cursor.getRotation() - 30);

		drawLine(Color::White, -length / 3);
	}
}

void Draw::fern(vector <Vertex> &assembly, int const &iteration, float const &length) {
	if (iteration == 0) {
		for (auto point : drawLine(Color::Green, length)) assembly.push_back(point);

		for (auto point : drawLine(Color::Green, -length / 2, -75)) assembly.push_back(point);

		for (auto point : drawLine(Color::Green, length / 6)) assembly.push_back(point);

		for (auto point : drawLine(Color::Green, -length / 6, 150)) assembly.push_back(point);
		
		for (auto point : drawLine(Color::Green, length / 6)) assembly.push_back(point);

		for (auto point : drawLine(Color::Green, -length / 6, -75)) assembly.push_back(point);

		for (auto point : drawLine(Color::Green, -length / 2)) assembly.push_back(point);
	}
	else {
		for (auto point : drawLine(Color::Green, length / 2, -75)) assembly.push_back(point);

		fern(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() + 70);

		fern(assembly, iteration - 1, length);

		m_cursor.setRotation(m_cursor.getRotation() + 80);

		fern(assembly, iteration - 1, length / 3);

		m_cursor.setRotation(m_cursor.getRotation() - 75);

		for (auto point : drawLine(Color::Green, -length / 2)) assembly.push_back(point);
	}
}

