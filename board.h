#ifndef BOARD_H
#define BOARD_H

#include "pawn.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

const float board_size = 800;
const float field_size = 77.5;
const float border_size = 91;

enum MoveType {
	INVALID,
	NORMAL,
	BEAT,
	MULTI_BEAT,
};

struct Move {
	Move() = default;
	Move(sf::Vector2i start_, sf::Vector2i finish_, MoveType type_) : start(start_), finish(finish_), type(type_) {};
	sf::Vector2i start;
	sf::Vector2i finish;
	MoveType type;
};

struct Node {
	Node(Move move_, int value_) : move(move_), value(value_) {};
	Move move;
	int value;
};

class Board {
public:
	Board();
	~Board();
	Board(const Board& copied);

	std::shared_ptr<Pawn> field[8][8] = { nullptr };
	std::vector<std::weak_ptr<Pawn>> pawn_vector;
	std::vector<std::weak_ptr<Pawn>> player_pawns[2];

	bool beat_possible[2] = { false };
	bool& getBeatPossible(OwningPlayer player);
	void resolveBeating(OwningPlayer player);

	std::vector<std::weak_ptr<Pawn>>& getVector(OwningPlayer player);
	std::shared_ptr<Pawn> getPawn(const sf::Vector2i& coords);
	int setPawn(const sf::Vector2i& coords, const std::shared_ptr<Pawn>& new_ptr);
	std::shared_ptr<Pawn> movePawn(sf::Vector2i start, sf::Vector2i finish, MoveType type);
	std::shared_ptr<Pawn> movePawn(const Move& move);
	MoveType checkMove(sf::Vector2i& start, sf::Vector2i& finish);
	std::vector<Move>* getAvailibleMoves(OwningPlayer player);
	std::vector<Move>* getAvailibleMoves(OwningPlayer player, const std::shared_ptr<Pawn> pawn);
	int getScore(OwningPlayer player);
	OwningPlayer checkWin(OwningPlayer player);
	void print();
};

#endif
