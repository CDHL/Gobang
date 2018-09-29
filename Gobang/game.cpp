#include "game.h"

const int path[4][2] = { 1,0, 1,1, 0,1, -1,1 };

Piece board[BOARD_SIZE][BOARD_SIZE];

Piece curPlayer = black;

bool inmap(int x, int y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

int setPiece(int x, int y)
{
	if (!inmap(x, y)) return -1;
	if (board[x][y]) return -1;
	if (curPlayer == black)
	{
		board[x][y] = black;
		curPlayer = white;
	}
	else
	{
		board[x][y] = white;
		curPlayer = black;
	}
	return win(x, y);
}

int win(int x, int y)
{
	int cx, cy;
	const Piece curColor = board[x][y];
	int match;
	for (int i = 0; i < 4; ++i)
	{
		cx = x;
		cy = y;
		while (inmap(cx - path[i][0], cy - path[i][1]) && 
			board[cx - path[i][0]][cy - path[i][1]] == curColor)
		{
			cx -= path[i][0];
			cy -= path[i][1];
		}
		for (match = 0; inmap(cx, cy) && board[cx][cy] == curColor; ++match)
		{
			cx += path[i][0];
			cx += path[i][1];
		}
		if (match >= 5) return curColor;
	}
	return 0;
}