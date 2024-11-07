#pragma once
#include <vector>
#include "SDL2/SDL.h"
#include "TextureLoader.h"
#include "Vector2D.h"
#include "Timer.h"
#include "MathAddon.h"
#include "Level.h"
class Game;



class Animal
{
private:
	enum class State {
		idle,
		moving,
		rotating
	} stateCurrent;

	
	struct Type {
		std::string name = "";
		float radius = 0.5f;
	};


public:
	Animal(SDL_Renderer* renderer, int setTypeID, Vector2D setPos, float setAngle);
	void update(float dT, Game& game);
	void draw(SDL_Renderer* renderer, int tileSize);
	void drawShadow(SDL_Renderer* renderer, int tileSize);
	bool checkIfTilesUnderOk(Level& level);
	static bool checkIfPositionOkForType(Vector2D posCheck, int animalTypeID, Game& game);
	bool checkCircleOverlap(Vector2D posCircle, float radiusCircle);


private:
	void drawTextureWithOffset(SDL_Renderer* renderer, SDL_Texture* textureSelected,
		int tileSize, int offset);
	void updateStateIdle(float dT, Game& game);
	bool updateMove(float dT);
	bool updateAngle(float dT);
	void moveToRandomPosition(Game& game);
	void rotateToRandomAngle();
	bool checkIfPositionOK(Vector2D posCheck, Game& game);
	static bool checkIfPositionOkGeneral(Vector2D posCheck, int animalTypeID, Animal* animalExclude,
		Game& game);


	Vector2D pos;
	float angle;
	float speed = 1.5f, speedAngular = MathAddon::angleDegToRad(180.0f);

	Timer timerStateIdle;
	Vector2D directionNormalTarget;
	float distanceToTarget = 0.0f;

	Timer timerGrowth;

	int typeID;

	SDL_Texture* textureSmallMain = nullptr, * textureSmallShadow = nullptr,
		* textureMain = nullptr, * textureShadow = nullptr;

	static const std::vector<Type> listAnimalTypes;
};