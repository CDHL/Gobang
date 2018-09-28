#pragma once

const int BOARD_SIZE = 15;

enum Piece;

extern Piece board[BOARD_SIZE][BOARD_SIZE];

int setPiece(int x, int y);
