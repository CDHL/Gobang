#pragma once

class Board
{
public:
	enum Piece
	{
		error = -1,
		empty = 0,
		black = 1,
		white = 2
	};

	Board();

	static bool inBoard(int x, int y);

	Piece setPiece(int x, int y);

	Piece win(int x, int y);

	Piece getPiece(int x, int y);

	void clear();

	static const int BOARD_SIZE = 15;

private:

	Piece m_board[BOARD_SIZE][BOARD_SIZE];

	Piece m_curPlayer;
};

extern Board g_mainBoard;