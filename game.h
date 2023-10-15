/*
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "pawn.h"
#include "board.h"

const int plus_infty = 10000;
const int minus_infty = -10000;

class Game {
public:
	Game();
	void start();
	void play();
	void end();
	void view();
	int getMove(OwningPlayer player);
	int manualMove(OwningPlayer player);
	int computerMove();
	void executeMove(sf::Vector2i& start, sf::Vector2i& finish, MoveType type);
	bool pollEvents(sf::Vector2i& mouse_position);
	int alphabeta(Board&, Move&, int depth, OwningPlayer, int alpha, int beta);

	sf::RenderWindow window;
	sf::Texture textures[5];
	sf::Sprite sprites[5];
	sf::Image icon;

	OwningPlayer players[2] = { HUMAN, COMPUTER };
	Board game_board;
	OwningPlayer active_player = HUMAN;
};
#endif
*/


#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "pawn.h"
#include "board.h"

const int plus_infty = 10000;
const int minus_infty = -10000;

class Game {
public:
	Game();
	void start();
	void play();
	void end();
	void view();
	int getMove(OwningPlayer player);
	int manualMove(OwningPlayer player);
	int computerMove();
	void executeMove(sf::Vector2i& start, sf::Vector2i& finish, MoveType type);
	bool pollEvents(sf::Vector2i& mouse_position);
	int alphabeta(Board&, Move&, int depth, OwningPlayer, int alpha, int beta);

	sf::RenderWindow window;
	sf::Texture textures[5];
	sf::Sprite sprites[5];
	sf::Image icon;

	OwningPlayer players[2] = { HUMAN, COMPUTER };
	Board game_board;
	OwningPlayer active_player = HUMAN;
};
#endif

