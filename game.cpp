#include "game.h"
#include <iostream>
#include <algorithm>

void delay(int miliseconds) {
	sf::Clock clock;
	clock.restart();
	while (1) {
		if (clock.getElapsedTime().asMilliseconds() > miliseconds)
			break;
	}
}

Game::Game() {
	// icon initialization
	icon.loadFromFile("./graphics/red.png");
	// texture initialization
	textures[0].loadFromFile("./graphics/board.jpg");
	textures[1].loadFromFile("./graphics/black.png");
	textures[2].loadFromFile("./graphics/red.png");
	textures[3].loadFromFile("./graphics/black_king.png");
	textures[4].loadFromFile("./graphics/red_king.png");
	// sprite initialization
	sprites[0].setTexture(textures[0]);
	for (int i = 1; i < 5; ++i) {
		sprites[i].setTexture(textures[i]);
		sprites[i].setScale(0.6, 0.6);
	}
}

void Game::start() {
	window.create(sf::VideoMode(board_size, board_size), "Checkers Game");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	view();
}

int Game::manualMove(OwningPlayer player) {
	sf::Vector2i mouse_position, start, finish;
	sf::Vector2i* updated_vector;
	std::shared_ptr<Pawn> active_pawn;
	bool mouse_pressed = false;
	while (window.isOpen()) {
		mouse_pressed = pollEvents(mouse_position);
		if (mouse_pressed) {
			if (mouse_position.x > border_size && mouse_position.x < board_size - border_size &&
				mouse_position.y > border_size && mouse_position.y < board_size - border_size) {
				if (!active_pawn) {
					updated_vector = &start;
				}
				else {
					updated_vector = &finish;
				}
				updated_vector->x = (mouse_position.x - border_size) / field_size;
				updated_vector->y = (mouse_position.y - border_size) / field_size;
				updated_vector->y = 7 - updated_vector->y;
				if (active_pawn) {
					// std::cerr << start.x << start.y << '-' << finish.x << finish.y << '\n';
					if (active_pawn->owner == player) {
						MoveType result = game_board.checkMove(start, finish);
						if (result != INVALID) {
							executeMove(start, finish, result);
							return 0;
						}
					}
					active_pawn = nullptr;
				}
				else {
					active_pawn = game_board.getPawn(start);
				}
			}
		}
	}
	return 1;
}

void Game::play() {
	Move computer_move;
	OwningPlayer winner = NOBODY;
	while (winner == NOBODY) {
		if (getMove(active_player))
			break;
		active_player = otherPlayer(active_player);
		winner = game_board.checkWin(active_player);
	}
	if (winner == HUMAN)
		std::cout <<std::endl << "You are the winner!!\n";
	else if (winner == COMPUTER)
		std::cout << std::endl << "AI wins!!\n";
}

int Game::computerMove() {
	
	Move computer_move;
	sf::Clock clock;
	clock.restart();
	std::cout <<std::endl<< "AI is playing its move..." << std::endl; // Add this message
	alphabeta(game_board, computer_move, 6, COMPUTER, minus_infty, plus_infty);
	std::cerr << clock.getElapsedTime().asMilliseconds();
	executeMove(computer_move.start, computer_move.finish, computer_move.type);
	return 0;
}

int Game::getMove(OwningPlayer player) {
	game_board.resolveBeating(player);
	if (player == COMPUTER)
		return computerMove();
	else
		return manualMove(HUMAN);
}

bool Game::pollEvents(sf::Vector2i& mouse_position) {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
			return false;
		}
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				mouse_position.x = event.mouseButton.x;
				mouse_position.y = event.mouseButton.y;
				return true;
			}
		}
	}
	return false;
}

void Game::view() {
	window.clear();
	// draw the board
	window.draw(sprites[0]);
	int sprite_number;
	// draw the pawns
	for (const auto pawn_ptr : game_board.pawn_vector) {
		if (auto drawn_pawn = pawn_ptr.lock()) {
			if (drawn_pawn->owner == HUMAN)
				sprite_number = 1;
			else
				sprite_number = 2;
			sprites[sprite_number].setPosition(drawn_pawn->x, drawn_pawn->y);
			window.draw(sprites[sprite_number]);
		}
	}
	window.display();
}

void Game::executeMove(sf::Vector2i& start, sf::Vector2i& finish, MoveType type) {
	if (auto pawn = game_board.movePawn(start, finish, type)) {
		float distance_x = ((finish.x - start.x) * field_size) / 10;
		float distance_y = ((finish.y - start.y) * field_size) / 10;
		for (int i = 0; i < 10; ++i) {
			pawn->x += distance_x;
			pawn->y -= distance_y;
			delay(20);
			view();
		}
		view();
	}
}

int Game::alphabeta(Board& current_board, Move& best_move, int depth, OwningPlayer player, int alpha, int beta) {
	if (depth == 0) { // Terminal node or max depth reached
		return current_board.getScore(COMPUTER) - current_board.getScore(HUMAN);
	}

	std::vector<Move>* possible_moves = current_board.getAvailibleMoves(player);
	std::vector<Board> possible_boards(possible_moves->size(), current_board);

	if (player == COMPUTER) {
		for (size_t i = 0; i < possible_moves->size(); ++i) {
			possible_boards[i].movePawn(possible_moves->at(i));
			int value = alphabeta(possible_boards[i], best_move, depth - 1, HUMAN, alpha, beta);
			alpha = std::max(alpha, value);
			if (alpha == value && depth == 6)
				best_move = possible_moves->at(i);
			if (alpha >= beta) {
				return alpha;
			}
		}
		return alpha;
	}
	else {
		for (size_t i = 0; i < possible_moves->size(); ++i) {
			beta = std::min(beta, alphabeta(possible_boards[i], best_move, depth - 1, COMPUTER, alpha, beta));
			if (alpha >= beta) {
				return beta;
			}
		}
		return beta;
	}
}

