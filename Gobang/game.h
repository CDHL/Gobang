#pragma once

const int BOARD_SIZE = 15;

enum Piece
{
	empty = 0,
	black = 1,
	white = 2
};

extern Piece board[BOARD_SIZE][BOARD_SIZE];

bool inBoard(int x, int y);

int setPiece(int x, int y);

int win(int x, int y);
