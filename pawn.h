/*
#ifndef PAWN_H
#define PAWN_H

#include <SFML/Graphics.hpp>

enum OwningPlayer {
	NOBODY,
	HUMAN,
	COMPUTER,
};

OwningPlayer otherPlayer(OwningPlayer current_player);

enum PawnLevel {
	normal,
	king,
};

class Pawn {
public:
	Pawn(int, int, float, float, OwningPlayer);
	void lightUp();
	OwningPlayer owner;
	PawnLevel level = normal;
	float x;
	float y;
	sf::Vector2i coordinates;
};

#endif
*/

#ifndef PAWN_H
#define PAWN_H

#include <SFML/Graphics.hpp>

enum OwningPlayer {
	NOBODY,
	HUMAN,
	COMPUTER,
};

OwningPlayer otherPlayer(OwningPlayer current_player);

enum PawnLevel {
	normal,
	king,
};

class Pawn {
public:
	Pawn(int, int, float, float, OwningPlayer);
	void lightUp();
	OwningPlayer owner;
	PawnLevel level = normal;
	float x;
	float y;
	sf::Vector2i coordinates;
};

#endif



