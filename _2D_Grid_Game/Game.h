#pragma once
#include <utils.h>
using namespace utils;

#pragma region gameInformation
// TODO: Set your name and group in the title here
std::string g_WindowTitle{ "Project name - Name, firstname - 1DAExx" };

// Change the window dimensions here
float g_WindowWidth{ 1280 };
float g_WindowHeight{ 720 };
#pragma endregion gameInformation



#pragma region ownDeclarations
// Declare your own global variables here
bool g_IsGameOver{};

const int g_CellSize{ 32 };
const int g_NrRows{ 24 }, g_NrCols{ 41 };
int g_Grid[g_NrRows * g_NrCols]{};
const int g_MaxHealth{ 5 };

Texture g_Muur{};
Texture * g_PArrCells[g_NrCols * g_NrRows]{};

enum class Direction
{
	//Enum also used in array indexing so don't change order;
	up,
	right,
	down,
	left,
	STATE_COUNT //Amount of options in enum, also used as checker in MoveTank function
};

struct Sprite
{
	Texture texture;
	int nrCols;
	float spriteSize;
};

struct Bullet {
	Point2f position;
	Direction direction;
};

struct Tank
{
	const int id;
	Point2f position; // position of tank (topleft of the sprite)
	Direction direction;
	Sprite sprite;
	Bullet bullet;
	bool canShoot;
	int health;
	bool isAlive;
	SDL_Scancode movementKeys[4]; //Down, Up, Left, Right
	SDL_KeyCode shootKey;

	Point2f GetCenterPos();
	Rectf GetHitbox();
};

Tank g_Tank1{
	1,
	Point2f{},
	Direction{},
	Sprite{},
	Bullet{},
	true,
	g_MaxHealth,
	true,
	{SDL_SCANCODE_DOWN, SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT},
	SDLK_RSHIFT

};

Tank g_Tank2{
	2,
	Point2f{},
	Direction{},
	Sprite{},
	Bullet{},
	true,
	g_MaxHealth,
	true,
	{SDL_SCANCODE_S, SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_D},
	SDLK_LSHIFT
};

// Declare your own functions here
void ResetGame();
void LoadSprites();
void InitializeMap();

void MoveTank(Tank& tank, const Uint8* pStates, float elapsedSec);
void DrawGrid(int grid[], int nrRows, int nrCols, int cellSize, Point2f drawPos, float scale = 1.f);
void DrawTank(const Tank& tank);

int GetIndex(int rowIndex, int columnIndex, int numCols);
int GetRow(int index, int numCols);
int GetCol(int index, int numCols);

int PointToGridIndex(Texture** pointerArr, int rows, int cols, int cellSize, const Point2f& gridPos, const Point2f& point);
int PointToGridIndex(const Point2f& point);
bool IsMovementAllowed(int gridIndex);
bool IsHitboxMovementAllowed(const Rectf& rect);
Tank* getEnemyPointer(const Tank& tank);

void DrawBullet(const Tank& tank);
void UpdateBullet(Tank& tank, float elapsedSec);
void CheckBulletConditions(Tank& tank);
void FireBullet(Tank& tank);
void HitEnemy(Tank& enemy);
void DestroyWall(int gridIndex, Direction bulletDirection);

void PrintInfo();


#pragma endregion ownDeclarations

#pragma region gameFunctions											
void Start();
void Draw();
void Update(float elapsedSec);
void End();
#pragma endregion gameFunctions

#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key);
void OnKeyUpEvent(SDL_Keycode key);
void OnMouseMotionEvent(const SDL_MouseMotionEvent& e);
void OnMouseDownEvent(const SDL_MouseButtonEvent& e);
void OnMouseUpEvent(const SDL_MouseButtonEvent& e);
#pragma endregion inputHandling
