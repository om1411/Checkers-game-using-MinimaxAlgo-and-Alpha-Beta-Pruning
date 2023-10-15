#include "board.h"
#include <iostream>
#include <vector>
#include <memory>

Board::Board() {
	pawn_vector.reserve(24);
	float new_x, new_y;
	OwningPlayer new_player;
	std::shared_ptr<Pawn> new_ptr;

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (i % 2 == j % 2) {
				if (j < 3 || j > 4) {
					new_x = border_size + i * field_size + 5;
					new_y = border_size + (7 - j) * field_size + 5;
					if (j < 3)
						new_player = HUMAN;
					else if (j > 4)
						new_player = COMPUTER;
					new_ptr = std::make_shared<Pawn>(i, j, new_x, new_y, new_player);
					field[i][j] = new_ptr;
					pawn_vector.push_back(std::weak_ptr<Pawn>(new_ptr));
					getVector(new_player).push_back(std::weak_ptr<Pawn>(new_ptr));
				}
			}
		}
	}
}

Board::~Board() {
	// Destructor is empty since shared_ptr handles memory automatically.
}

Board::Board(const Board& copied) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (copied.field[i][j]) {
				std::shared_ptr<Pawn> new_ptr = std::make_shared<Pawn>(*copied.field[i][j]);
				field[i][j] = new_ptr;
				pawn_vector.push_back(std::weak_ptr<Pawn>(new_ptr));
				getVector(new_ptr->owner).push_back(std::weak_ptr<Pawn>(new_ptr));
			}
		}
	}
}

std::vector<std::weak_ptr<Pawn>>& Board::getVector(OwningPlayer player) {
	if (player == HUMAN)
		return player_pawns[0];
	else
		return player_pawns[1];
}

std::shared_ptr<Pawn> Board::getPawn(const sf::Vector2i& coords) {
	if (field[coords.x][coords.y])
		return field[coords.x][coords.y];
	else
		return nullptr;
}

int Board::setPawn(const sf::Vector2i& coords, const std::shared_ptr<Pawn>& new_ptr) {
	field[coords.x][coords.y] = new_ptr;
	return 0;
}

void Board::print() {
	for (int y = 7; y > -1; --y) {
		for (int x = 0; x < 8; ++x) {
			auto printed_pawn = getPawn(sf::Vector2i(x, y));
			if (printed_pawn) {
				if (printed_pawn->owner == HUMAN)
					std::cerr << 'O';
				else
					std::cerr << 'X';
			}
			else
				std::cerr << ' ';
		}
		std::cerr << '\n';
	}
	std::cerr << '\n';
}

std::shared_ptr<Pawn> Board::movePawn(sf::Vector2i start, sf::Vector2i finish, MoveType type) {
	if (auto pawn = getPawn(start)) {
		int direction = 1;
		if (pawn->owner == COMPUTER)
			direction = -1;
		if (type == BEAT) {
			sf::Vector2i beaten_pawn(start.x + (finish.x - start.x) / 2, start.y + direction);
			getPawn(beaten_pawn).reset();
			setPawn(beaten_pawn, nullptr);
		}
		setPawn(start, nullptr);
		setPawn(finish, pawn);
		pawn->coordinates = finish;
		resolveBeating(pawn->owner);

		return pawn;
	}
	return nullptr;
}

std::shared_ptr<Pawn> Board::movePawn(const Move& move) {
	return movePawn(move.start, move.finish, move.type);
}

MoveType Board::checkMove(sf::Vector2i& start, sf::Vector2i& finish) {
	MoveType result = INVALID;
	if (finish.x >= 0 && finish.x <= 7 && finish.y >= 0 && finish.y <= 7) {
		if (std::shared_ptr<Pawn> pawn = getPawn(start)) {
			int direction = 1;
			if (pawn->owner == COMPUTER)
				direction = -1;
			if (finish.y == start.y + direction) {
				if (finish.x == start.x + 1 || finish.x == start.x - 1) {
					if (!getPawn(finish)) {
						if (!getBeatPossible(pawn->owner)) {
							result = NORMAL;
						}
					}
				}
			}
			else if (finish.y == start.y + 2 * direction) {
				if (finish.x == start.x + 2 || finish.x == start.x - 2) {
					if (!getPawn(finish)) {
						sf::Vector2i beaten_pawn(start.x + (finish.x - start.x) / 2, start.y + direction);
						if (getPawn(beaten_pawn)) {
							if (getPawn(beaten_pawn)->owner == otherPlayer(pawn->owner)) {
								result = BEAT;
							}
						}
					}
				}
			}
		}
	}
	return result;
}

bool& Board::getBeatPossible(OwningPlayer player) {
	if (player == COMPUTER)
		return beat_possible[1];
	return beat_possible[0];
}

void Board::resolveBeating(OwningPlayer player) {
	getBeatPossible(player) = false;
	std::vector<Move>* move_vector = getAvailibleMoves(player);
	for (auto tested_move : *move_vector) {
		if (tested_move.type == BEAT)
			getBeatPossible(player) = true;
	}
	delete move_vector;
}

std::vector<Move>* Board::getAvailibleMoves(OwningPlayer player) {
	std::vector<Move>* move_vector = new std::vector<Move>;
	for (auto pawn_ptr : getVector(player)) {
		if (auto pawn = pawn_ptr.lock()) {
			auto new_moves = getAvailibleMoves(player, pawn);
			if (!new_moves->empty())
				move_vector->insert(move_vector->end(), new_moves->begin(), new_moves->end());
			delete new_moves;
		}
	}
	return move_vector;
}

std::vector<Move>* Board::getAvailibleMoves(OwningPlayer player, const std::shared_ptr<Pawn> pawn) {
	std::vector<Move>* move_vector = new std::vector<Move>;
	sf::Vector2i start, finish;
	int direction = 1;
	if (pawn) {
		if (player == COMPUTER)
			direction = -1;
		start = pawn->coordinates;
		for (int k : {1, 2}) {
			for (int l : {-1, 1}) {
				finish = start + sf::Vector2i(l * k, k * direction);
				MoveType result = checkMove(start, finish);
				if (result != INVALID) {
					Move new_move = Move(start, finish, result);
					move_vector->push_back(new_move);
				}
			}
		}
	}
	return move_vector;
}

int Board::getScore(OwningPlayer player) {
	int score = 0;
	for (auto pawn_weak : getVector(player)) {
		if (auto pawn = pawn_weak.lock()) {
			int x = pawn->coordinates.x;
			int y = pawn->coordinates.y;
			score += 10;

			if (player == HUMAN) {
				if (y == 2 || y == 3)
					score += 1;
				else if (y == 4 || y == 5)
					score += 3;
				else if (y == 6 || y == 7)
					score += 5;
			}
			else {
				if (y == 5 || y == 4)
					score += 1;
				else if (y == 3 || y == 2)
					score += 3;
				else if (y == 1 || y == 0)
					score += 5;
			}
			if ((x == 0 || x == 7) && (y == 0 || y == 7))
				score += 2;
			else if ((x == 1 || x == 6) && (y == 1 || y == 6))
				score += 1;
		}
	}
	return score;
}

OwningPlayer Board::checkWin(OwningPlayer player) {
	resolveBeating(player);
	OwningPlayer winner = NOBODY;
	std::vector<Move>* available_moves;

	available_moves = getAvailibleMoves(player);
	if (available_moves->empty()) {
		winner = otherPlayer(player);
	}
	else {
		int pawn_count = 0;
		for (auto checked_pawn : getVector(player)) {
			if (!checked_pawn.expired())
				++pawn_count;
		}
		if (!pawn_count) {
			winner = otherPlayer(player);
		}
	}
	delete available_moves;
	return winner;
}
