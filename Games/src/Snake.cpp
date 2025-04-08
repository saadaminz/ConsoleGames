
#include <iostream>
#include <Windows.h>
#include <vector>

/*
Button mapping:

	W = UP
	S = DOWN
	A = LEFT
	D = RIGHT
	Escape = Exit game
*/

constexpr int size = 25;

struct Segments
{
	int posY, posX;
};

struct Snake
{
	int snakeY, snakeX;
	int fruitY, fruitX;
	int fruitsCount;
	int score, speed;
	char grid[size][size];
	std::vector<Segments> body;

	enum Move { UP = 0, DOWN, RIGHT, LEFT };
	Move move = LEFT;
	Move lastMove = move;

	Snake()
		: snakeY(17), snakeX(17), score(0), fruitsCount(0), speed(80)
	{
		memset(grid, '_', sizeof(grid));
		grid[snakeY][snakeX] = '0';
		body.reserve(25);
		body.push_back({ snakeY, snakeX });
		std::srand(std::time(nullptr));
		GRand();
	}

	~Snake() {}

	void GotoXY()
	{
		COORD c;
		c.X = 0;
		c.Y = 0;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	}

	void GRand()
	{
		fruitsCount++;

		while (true)
		{
			fruitY = (rand() % (size - 2)) + 1;
			fruitX = (rand() % (size - 2)) + 1;

			if (grid[fruitY][fruitX] == '_')
				break;
		}

		if ((fruitsCount % 5) == 0)
			grid[fruitY][fruitX] = '$';
		else
			grid[fruitY][fruitX] = '%';
	}

	void RunLoop()
	{
		while (true)
		{
			for (int i = 0; i < size; i++)
			{
				grid[i][0] = '#';
				grid[0][i] = '#';
				grid[i][size - 1] = '#';
				grid[size - 1][i] = '#';
			}

			for (int i = 0; i < body.size(); i++)
				grid[body[i].posY][body[i].posX] = '_';

			if (body.size() > 1)
				for (int i = body.size() - 1; i > 0; i--)
					body[i] = body[i - 1];

			grid[snakeY][snakeX] = '_';

			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return;
			else if (GetAsyncKeyState(0x41/*A*/) & 0x8000 && lastMove != RIGHT)	move = LEFT;
			else if (GetAsyncKeyState(0x44/*D*/) & 0x8000 && lastMove != LEFT)	move = RIGHT;
			else if (GetAsyncKeyState(0x57/*W*/) & 0x8000 && lastMove != DOWN)	move = UP;
			else if (GetAsyncKeyState(0x53/*S*/) & 0x8000 && lastMove != UP)	move = DOWN;
			lastMove = move;

			switch (move)
			{
			case LEFT:	snakeX--; break;
			case RIGHT:	snakeX++; break;
			case UP:	snakeY--; break;
			case DOWN:	snakeY++; break;
			}

			body[0].posY = snakeY;
			body[0].posX = snakeX;

			for (int i = 1; i < body.size(); i++)
			{
				if ((body[i].posY == snakeY) && (body[i].posX == snakeX))
				{
					system("cls");
					std::cout << "Game Over!\nYour Score: " << score << '\n';
					return;
				}
			}

			if (grid[snakeY][snakeX] == '#')
			{
				system("cls");
				std::cout << "Game Over!\nYour Score: " << score << '\n';
				return;
			}

			if (snakeY == fruitY && snakeX == fruitX)
			{
				if ((fruitsCount % 5) == 0) score += 15;
				else score += 5;

				GRand();
				if (speed > 30) speed -= 3;
				Segments newSegment = body.back();
				body.push_back(newSegment);
			}

			for (int i = 0; i < body.size(); i++)
				grid[body[i].posY][body[i].posX] = (i == 0) ? '0' : 'O';

			char buffer[size * size + 100];
			int bufferIndex = 0;

			for (int i = 0; i < size; i++)
			{
				for (int j = 0; j < size; j++)
					buffer[bufferIndex++] = grid[i][j];
				buffer[bufferIndex++] = '\n';
			}
			buffer[bufferIndex++] = '\0';
			printf("%sScore: %d\n", buffer, score);

			Sleep(speed);
			GotoXY();
		}
	}
};

#if 0

int main()
{
	Snake game;
	game.RunLoop();
}

#endif
