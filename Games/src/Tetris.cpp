
#include <iostream>
#include <Windows.h>
#include <array>
#include <thread>

/*
Button mapping:

	A = LEFT
	D = RIGHT
	S = Increase speed
	Spacebar = Rotate piece
	Escape = Exit game
*/

constexpr int size = 20;
char grid[size][size];
int speed = 100, speedUp = 0;
int score = 0;
bool gameOver = false;

struct Tetromino
{
	char block[4][4];
	int posY, posX;
};

Tetromino testPiece;
Tetromino currentPiece;
Tetromino* nextPiece = nullptr;

Tetromino I = { {{' ','#',' ',' '}, {' ','#',' ',' '}, {' ','#',' ',' '}, {' ','#',' ',' '}} };
Tetromino L = { {{' ','#',' ',' '}, {' ','#',' ',' '}, {' ','#','#',' '}, {' ',' ',' ',' '}} };
Tetromino J = { {{' ',' ','#',' '}, {' ',' ','#',' '}, {' ','#','#',' '}, {' ',' ',' ',' '}} };
Tetromino T = { {{' ',' ',' ',' '}, {'#','#','#',' '}, {' ','#',' ',' '}, {' ',' ',' ',' '}} };
Tetromino O = { {{' ',' ',' ',' '}, {' ','#','#',' '}, {' ','#','#',' '}, {' ',' ',' ',' '}} };
Tetromino S = { {{' ',' ',' ',' '}, {' ',' ','#','#'}, {'#','#',' ',' '}, {' ',' ',' ',' '}} };
Tetromino Z = { {{' ',' ',' ',' '}, {'#','#',' ',' '}, {' ',' ','#','#'}, {' ',' ',' ',' '}} };

std::array<Tetromino*, 7> pieces = { &I,&L,&J,&T,&O,&S,&Z };

enum Movement { NILL=0, RIGHT, LEFT, SPACEBAR };
Movement move = NILL;

Tetromino* GetRandomPiece()
{
	int index = std::rand() % pieces.size();
	pieces[index]->posY = 0;
	pieces[index]->posX = size / 2;
	return pieces[index];
}

void GotoXY()
{
	COORD c;
	c.Y = 0;
	c.X = 0;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

bool ValidatePosition(const Tetromino& T)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (T.block[i][j] == ' ')
				continue;

			int pY = T.posY + i;
			int pX = T.posX + j;

			if (pX < 0 || pX >= size || pY >= size)
				return false;

			if (grid[pY][pX] != '_')
				return false;
		}
	}
	return true;
}



void RotatePiece(const Tetromino& moveFrom, Tetromino& moveTo)
{
	memset(moveTo.block, ' ', sizeof(moveTo.block));

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			moveTo.block[3 - j][i] = moveFrom.block[i][j];
		}
	}
}

void DrawInfo()
{
	std::cout << "\nNext Piece:\n";
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (nextPiece->block[i][j] == ' ')
				std::cout << '_';
			else
				std::cout << nextPiece->block[i][j];
		}
		std::cout << '\n';
	}
	std::cout << "\nScore: " << score << '\n';
}

void DrawGrid()
{
	char buffer[size * size + 100];
	int index = 0;

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			buffer[index++] = grid[i][j];
		}
		buffer[index++] = '\n';
	}

	buffer[index++] = '\0';
	printf("%s", buffer);
}

void DrawPieceOnGrid(const Tetromino& T)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (T.block[i][j] != ' ')
			{
				grid[T.posY + i][T.posX + j] = T.block[i][j];
			}
		}
	}
}

void RemovePieceTrackFromGrid(Tetromino& T)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (T.block[i][j] != ' ')
			{
				grid[T.posY + i][T.posX + j] = '_';
			}
		}
	}
}

void ClearLine()
{
	for (int i = size - 1; i >= 0; i--)
	{
		bool lineFull = true;

		for (int j = 0; j < size; j++)
		{
			if (grid[i][j] == '_')
			{
				lineFull = false;
				break;
			}
		}

		if (lineFull)
		{
			for (int row = i; row > 0; row--)
			{
				for (int col = 0; col < size; col++)
				{
					grid[row][col] = grid[row - 1][col];
				}
			}

			for (int col = 0; col < size; col++)
				grid[0][col] = '_';

			score += 45;
			i++;
		}
	}
}

void RunLoop()
{
	std::srand(std::time(nullptr));
	memset(grid, '_', sizeof(grid));
	nextPiece = GetRandomPiece();
	currentPiece = *nextPiece;

	while (!gameOver)
	{
		nextPiece = GetRandomPiece();

		while (!gameOver)
		{
			testPiece = currentPiece;
			speedUp = 0;

			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)		return;
			else if (GetAsyncKeyState(0x53/*S*/) & 0x8000)	speedUp = 50;
			else if (GetAsyncKeyState(0x44/*D*/) & 0x8000)	move = RIGHT;
			else if (GetAsyncKeyState(0x41/*A*/) & 0x8000)	move = LEFT;
			else if (GetAsyncKeyState(VK_SPACE) & 0x8000)	move = SPACEBAR;
			else move = NILL;

			if (move != NILL)
			{
				if (move == RIGHT)			testPiece.posX++;
				else if (move == LEFT)		testPiece.posX--;
				else if (move == SPACEBAR)	RotatePiece(currentPiece, testPiece);

				if (!ValidatePosition(testPiece))
					testPiece = currentPiece;
				else
				{
					DrawPieceOnGrid(currentPiece);
					DrawGrid();
					DrawInfo();
					RemovePieceTrackFromGrid(currentPiece);
					GotoXY();
					currentPiece = testPiece;
				}
			}

			testPiece.posY++;

			if (ValidatePosition(testPiece))
			{
				DrawPieceOnGrid(currentPiece);
				DrawGrid();
				DrawInfo();
				RemovePieceTrackFromGrid(currentPiece);
				currentPiece = testPiece;
			}
			else
			{
				DrawPieceOnGrid(currentPiece);
				ClearLine();
				score += 15;
				if (speed > 70) speed--;
				currentPiece = *nextPiece;

				if (!ValidatePosition(currentPiece))
				{
					system("cls");
					std::cout << "Game Over!\nYour Score: " << score << '\n';
					gameOver = true;
				}
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(speed - speedUp));
			GotoXY();
		}
	}
}

#if 0

int main()
{
	RunLoop();
}

#endif
