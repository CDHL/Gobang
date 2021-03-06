#include "stdafx.h"

#include "game.h"

#include <cstring>

const int path[4][2] = { 1,0, 1,1, 0,1, -1,1 };

Board g_mainBoard;

Board::Board() : m_curPlayer(black)
{
	std::memset(m_board, 0, sizeof(m_board));
}

bool Board::inBoard(int x, int y)
{
	return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

Board::Piece Board::setPiece(int x, int y)
{
	if (!inBoard(x, y)) return error;
	if (m_board[x][y]) return error;
	if (m_curPlayer == black)
	{
		m_board[x][y] = black;
		m_curPlayer = white;
	}
	else
	{
		m_board[x][y] = white;
		m_curPlayer = black;
	}
	return win(x, y);
}

Board::Piece Board::win(const int x, const int y)
{
	int cx, cy;
	const Piece curColor = m_board[x][y];
	int match;
	for (int i = 0; i < 4; ++i)
	{
		cx = x;
		cy = y;
		while (inBoard(cx - path[i][0], cy - path[i][1]) && 
			m_board[cx - path[i][0]][cy - path[i][1]] == curColor)
		{
			cx -= path[i][0];
			cy -= path[i][1];
		}
		for (match = 0; inBoard(cx, cy) && m_board[cx][cy] == curColor; ++match)
		{
			cx += path[i][0];
			cy += path[i][1];
		}
		if (match >= 5) return curColor;
	}
	return empty;
}

Board::Piece Board::getPiece(int x, int y)
{
	return inBoard(x, y) ? m_board[x][y] : error;
}

void Board::clear()
{
	m_curPlayer = black;
	std::memset(m_board, 0, sizeof(m_board));
}