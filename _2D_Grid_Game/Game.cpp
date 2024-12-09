#include "pch.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <string>

//Basic game functions
#pragma region gameFunctions											
void Start()
{
	// initialize game resources here

	LoadSprites();
	ResetGame();

	std::cout << "Welcome to Tank Wars!\n";
	std::cout << "Press the \"I\" key for more information\n\n";

}

void Draw()
{
	ClearBackground(0.5f,0.5f,0.5f);
	DrawGrid(g_Grid, g_NrRows, g_NrCols, g_CellSize, Point2f(0.f, 0.f));
	
	if (g_Tank1.isAlive)
	{
		DrawTank(g_Tank1);
	}
	if (g_Tank2.isAlive)
	{
		DrawTank(g_Tank2);
	}

	if (!g_Tank1.canShoot)
	{
		DrawBullet(g_Tank1);
	}
	if (!g_Tank2.canShoot)
	{
		DrawBullet(g_Tank2);
	}
}

void Update(float elapsedSec)
{
	const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	MoveTank(g_Tank1, pStates, elapsedSec);
	MoveTank(g_Tank2, pStates, elapsedSec);
	
	if (!g_Tank1.canShoot)
	{
		UpdateBullet(g_Tank1, elapsedSec);
	}

	if (!g_Tank2.canShoot)
	{
		UpdateBullet(g_Tank2, elapsedSec);
	}
}

void End()
{
	// free game resources here
	for (int i{}; int(g_NrCols * g_NrRows) > i; i++)
	{
		if (g_PArrCells[i] != nullptr)
		{
		DeleteTexture(*g_PArrCells[i]);
		}
	}
}
#pragma endregion gameFunctions

//Keyboard and mouse input handling
#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key)
{

}

void OnKeyUpEvent(SDL_Keycode key)
{
	if (key == g_Tank1.shootKey)
	{
		FireBullet(g_Tank1);
	}
	if (key == g_Tank2.shootKey)
	{
		FireBullet(g_Tank2);
	}
	if (g_IsGameOver && key == SDLK_r)
	{
		ResetGame();
	}
	if (key == SDLK_i)
	{
		PrintInfo();
	}
}

void OnMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	// SAMPLE CODE: print mouse position
	//const float mouseX{ float(e.x) };
	//const float mouseY{ float(e.y) };
	//std::cout << "  [" << mouseX << ", " << mouseY << "]\n";
}

void OnMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	// SAMPLE CODE: print mouse position
	//const float mouseX{ float(e.x) };
	//const float mouseY{ float(e.y) };
	//std::cout << "  [" << mouseX << ", " << mouseY << "]\n";
}

void OnMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	// SAMPLE CODE: print mouse position
	//const float mouseX{ float(e.x) };
	//const float mouseY{ float(e.y) };
	//std::cout << "  [" << mouseX << ", " << mouseY << "]\n";
	
	// SAMPLE CODE: check which mouse button was pressed
	//switch (e.button)
	//{
	//case SDL_BUTTON_LEFT:
	//	//std::cout << "Left mouse button released\n";
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	//std::cout << "Right mouse button released\n";
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	//std::cout << "Middle mouse button released\n";
	//	break;
	//}
}
#pragma endregion inputHandling

#pragma region ownDefinitions
// Define your own functions here
void MoveTank(Tank& tank, const Uint8* pStates, float elapsedSec)
{
	if (!tank.isAlive)
	{
		return;
	}

	const float moveSpeed{ 200.f };

	Vector2f directionVector{};
	Direction direction{Direction::STATE_COUNT};

	//TODO direction veranderen met vector ipv per input
	if (pStates[tank.movementKeys[0]])
	{
		++directionVector.y;
		direction = Direction::down;
	}
	if (pStates[tank.movementKeys[1]])
	{
		--directionVector.y;
		direction = Direction::up;
	}
	if (pStates[tank.movementKeys[2]])
	{
		--directionVector.x;
		direction = Direction::left;
	}
	if (pStates[tank.movementKeys[3]])
	{
		++directionVector.x;
		direction = Direction::right;
	}

	//Only normalize vector when there is input
	if (Length(directionVector))
	{
		directionVector = Normalize(directionVector);
	}

	Point2f delta{
		directionVector.x * moveSpeed * elapsedSec,
		directionVector.y * moveSpeed * elapsedSec
	};

	Rectf playerHitbox{ tank.GetHitbox()};
	playerHitbox.left += delta.x;
	playerHitbox.top += delta.y;

	if (IsOverlapping(playerHitbox, getEnemyPointer(tank)->GetHitbox()))
	{
		return;
	}

	bool isHorizontalMovementAllowed{ IsHitboxMovementAllowed(Rectf(tank.position.x + delta.x, tank.position.y, tank.sprite.spriteSize, tank.sprite.spriteSize)) };
	bool isVerticalMovementAllowed{ IsHitboxMovementAllowed(Rectf(tank.position.x, tank.position.y + delta.y , tank.sprite.spriteSize, tank.sprite.spriteSize)) };

	if (isHorizontalMovementAllowed)
	{
		tank.position.x += delta.x;
	}
	else if (directionVector.y)
	{
		if (signbit(directionVector.y))
		{
			direction = Direction::up;
		}
		else 
		{
			direction = Direction::down;
		}
	}

	if (isVerticalMovementAllowed)
	{
		tank.position.y += delta.y;
	}

	if (direction != Direction::STATE_COUNT)
	{
		tank.direction = direction;
	}
}

void DrawGrid(int grid[], int nrRows, int nrCols, int cellSize, Point2f drawPos, float scale)
{
	const float startX{ drawPos.x };



	SetColor(0.5f, 0.5f, 0.5f, 1.f);
	for (int row{}; row < nrRows; ++row)
	{
		for (int col{}; col < nrCols; ++col)
		{

			Texture* pTexture{ g_PArrCells[GetIndex(row, col, nrCols)] };
			if (pTexture != nullptr)
			{
				Rectf destinationRect{ drawPos.x, drawPos.y, g_CellSize, g_CellSize };
				DrawTexture(*(g_PArrCells[GetIndex(row, col, nrCols)]), destinationRect);
				
			}
			// DrawRect(drawPos, float(cellSize), float(cellSize));
			drawPos.x += cellSize;
		}

		drawPos.x = startX;
		drawPos.y += cellSize;
	}
}

void DrawTank(const Tank& tank)
{
	SetColor(1.f, 0.f, 0.f, 1.f);
	const float tankSize{ 20.f };

	const float spriteSize{ tank.sprite.spriteSize };
	const float scale{ 3.f / 4.f };

	//Calc offset from center with texture dimensions:
	Rectf sourceRect{ int(tank.direction) * tank.sprite.spriteSize, 0, tank.sprite.spriteSize, tank.sprite.spriteSize };
	Rectf destinationRect{ tank.position.x, tank.position.y, tank.sprite.spriteSize, tank.sprite.spriteSize};

	//Draw tank sprite
	DrawTexture(tank.sprite.texture, destinationRect, sourceRect);
	//FillEllipse(tank.position, 5, 5);

}

int GetIndex(int rowIndex, int columnIndex, int numColumns)
{
	int index{ numColumns * rowIndex + columnIndex };
	return  index;
}
int GetRow(int index, int numCols)
{
	int row{  };
	if (index < numCols)
	{
		row = 0;
	}
	else
	{
		row = index / numCols;
	}

	return row;
}
int GetCol(int index, int numCols)
{
	int col{ index % numCols };
	return col;
}

int PointToGridIndex(Texture **pointerArr, int rows, int cols, int cellSize, const Point2f& gridPos, const Point2f& point)
{
	const Point2f delta{ point.x - gridPos.x, point.y - gridPos.y };
	if (delta.x < 0 || delta.x > cellSize * cols)
	{
		return  -1;
	}
	if (delta.y < 0 || delta.y > cellSize * rows)
	{
		return -1;
	}

	const int colIndex{ int(delta.x) / cellSize };
	const int rowIndex{ int(delta.y) / cellSize };
	int tankIndex{ rowIndex * cols + colIndex }; // = tankindex

	return tankIndex;
}
int PointToGridIndex(const Point2f& point)
{
	return PointToGridIndex(g_PArrCells, g_NrRows, g_NrCols, g_CellSize, Point2f(0, 0), point);
}
bool IsMovementAllowed(int gridIndex)
{
	return g_PArrCells[gridIndex] == nullptr;
}

bool IsHitboxMovementAllowed(const Rectf& rect)
{
	const Point2f points[]{
		Point2f(rect.left, rect.top),
		Point2f(rect.left + rect.width, rect.top),
		Point2f(rect.left, rect.top + rect.height),
		Point2f(rect.left + rect.width, rect.top + rect.height),
		Point2f(rect.left + rect.width / 2, rect.top),
		Point2f(rect.left + rect.width / 2, rect.top + rect.height),
		Point2f(rect.left, rect.top + rect.height / 2),
		Point2f(rect.left + rect.width, rect.top + rect.height / 2)
	};

	bool isMovementAllowed{ true };
	for (int idx{}; idx < sizeof(points) / sizeof(points[0]); ++idx)
	{
		int gridIndex{ PointToGridIndex(points[idx]) };
		isMovementAllowed = isMovementAllowed && IsMovementAllowed(gridIndex);
	}

	return isMovementAllowed;
}

Tank* getEnemyPointer(const Tank& tank)
{
	if (tank.id == 1)
	{
		return &g_Tank2;
	}
	else
	{
		return &g_Tank1;
	}
}

void DrawBullet(const Tank& tank)
{
	const float radius{ 10.f };
	utils::FillEllipse(tank.bullet.position, radius, radius);
}

void UpdateBullet(Tank& tank, float elapsedSec)
{
	const float bulletSpeed{300.f};
	
	Vector2f directionVectors[]{
		Vector2f{0.f, -1.f},
		Vector2f{1.f, 0.f},
		Vector2f{0.f, 1.f},
		Vector2f{-1.f, 0.f}
	};

	Vector2f direction{ directionVectors[int(tank.bullet.direction)] };
	tank.bullet.position.x += direction.x * bulletSpeed * elapsedSec;
	tank.bullet.position.y += direction.y * bulletSpeed * elapsedSec;

	CheckBulletConditions(tank);
}
void CheckBulletConditions(Tank& tank)
{
	//is bullet out of bounds?
	Rectf window{ 0, 0, g_WindowWidth, g_WindowHeight };
	if (!IsPointInRect(tank.bullet.position, window))
	{
		tank.canShoot = true;
		return;
	}

	//does bullet hit a wall?
	int bulletIndex{ PointToGridIndex(g_PArrCells, g_NrRows, g_NrCols, g_CellSize, Point2f(0,0), tank.bullet.position) };

	if (!IsMovementAllowed(bulletIndex))
	{
		tank.canShoot = true;
		DestroyWall(bulletIndex, tank.bullet.direction);
		return;
	}

	//Did bullet hit the enemy
	Tank* pEnemy{getEnemyPointer(tank)};

	bool isEnemyHit{ IsPointInRect(tank.bullet.position, pEnemy->GetHitbox()) };
	if (isEnemyHit && pEnemy->isAlive)
	{
		tank.canShoot = true;
		HitEnemy(*pEnemy);
		return;
	}

	//Does bullet collide with enemy's bullet?
	if (IsOverlapping(Circlef(tank.bullet.position, 5.f), Circlef(pEnemy->bullet.position, 5.f)) && !pEnemy->canShoot)
	{
		tank.canShoot = true;
		pEnemy->canShoot = true;
	}

}
void FireBullet(Tank& tank)
{
	if (tank.isAlive && tank.canShoot)
	{
		tank.bullet.position = tank.GetCenterPos();
		tank.bullet.direction = tank.direction;
		tank.canShoot = false;
	}
}
void HitEnemy(Tank& enemy)
{
	--enemy.health;

	if(!enemy.health)
	{
		enemy.isAlive = false;
		g_IsGameOver = true;
	}
}
void DestroyWall(int gridIndex, Direction bulletDirection)
{
	g_PArrCells[gridIndex] = nullptr;

		if (bulletDirection == Direction::up || bulletDirection == Direction::down)
		{
			if (gridIndex - 1 >= 0)
			{
				g_PArrCells[gridIndex - 1] = nullptr;
			}
			if (gridIndex + 1 < g_NrRows * g_NrCols)
			{
				g_PArrCells[gridIndex + 1] = nullptr;
			}
		}
		else
		{
			if (gridIndex - g_NrCols >= 0)
			{
				g_PArrCells[gridIndex - g_NrCols] = nullptr;
			}
			if (gridIndex + g_NrCols < g_NrRows * g_NrCols)
			{
				g_PArrCells[gridIndex + g_NrCols] = nullptr;
			}
		}
}
void PrintInfo()
{
	std::cout << "Welcome to Tank Wars!\n";
	std::cout << "Each player controls his tank using their respective controls\n";
	std::cout << "Tank 1 uses ZSQD to move around and Left Shift to shoot\n";
	std::cout << "Tank 2 uses the arrow keys to move around and Right Shift to shoot\n";
	std::cout << "Each player starts with 5 lives\n";
	std::cout << "Shoot the walls to destroy them and create your own path\n\n";
	std::cout << "When one of you dies and the game is over you can press the \"R\" key to reset the game\n";
}
#pragma endregion ownDefinitions

Point2f Tank::GetCenterPos()
{
	return Point2f(position.x + sprite.spriteSize / 2.f, position.y + sprite.spriteSize / 2.f);
}

Rectf Tank::GetHitbox()
{
	return Rectf(position.x, position.y, sprite.spriteSize, sprite.spriteSize);
}

void ResetGame()
{
	InitializeMap();

	g_Tank1.canShoot = true;
	g_Tank1.health = g_MaxHealth;
	g_Tank1.isAlive = true;

	g_Tank2.canShoot = true;
	g_Tank2.health = g_MaxHealth;
	g_Tank2.isAlive = true;
}

void LoadSprites()
{
	TextureFromFile("resources/Red_Brick.png", g_Muur);

	if (!TextureFromFile("resources/Tank_Two-Sheet.png", g_Tank1.sprite.texture))
	{
		std::cout << "Error: loading tank 1 sprite sheet FAILED\n";
	}
	else {
		g_Tank1.sprite.nrCols = 4;
		g_Tank1.sprite.spriteSize = g_Tank1.sprite.texture.width / g_Tank1.sprite.nrCols;
	}
	if (!TextureFromFile("resources/mech_2-Sheet.png", g_Tank2.sprite.texture))
	{
		std::cout << "Error: loading tank 2 sprite sheet FAILED\n";
	}
	else {
		g_Tank2.sprite.nrCols = 4;
		g_Tank2.sprite.spriteSize = g_Tank2.sprite.texture.width / g_Tank2.sprite.nrCols;
	}

}

void InitializeMap()
{
	std::ifstream file("../resources/map.txt");
	std::string line;

	if (file.is_open())
	{
		int row{};
		while (std::getline(file, line))
		{
			for (int idx{}; idx < g_NrCols; ++idx)
			{
				if (line[idx] == '#')
				{
					g_PArrCells[GetIndex(row, idx, g_NrCols)] = &g_Muur;
				}
				else
				{
					g_PArrCells[GetIndex(row, idx, g_NrCols)] = nullptr;

					if (line[idx] == '1')
					{
						g_Tank1.position.x = float(idx * g_CellSize);
						g_Tank1.position.y = float(row * g_CellSize);
					}
					else if (line[idx] == '2')
					{
						g_Tank2.position.x = float(idx * g_CellSize);
						g_Tank2.position.y = float(row * g_CellSize);

						//std::cout << g_Tank2.position.x << "    " << g_Tank2.position.y;
					}
				}
			}

			++row;
		}
		file.close();
	}
	else
	{
		std::cout << "Error opening map\n\n";
	}


}