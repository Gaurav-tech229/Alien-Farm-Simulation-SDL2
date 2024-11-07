#pragma once
#include <chrono>
#include <vector>
#include "SDL2/SDL.h"
#include "TextureLoader.h"
#include "MathAddon.h"
#include "Vector2D.h"
#include "Level.h"
#include "Plant.h"
#include "Animal.h"



class Game
{
private:
	enum class PlacementMode {
		tiles,
		plants,
		animals
	} placementModeCurrent;


public:
	Game(SDL_Window* window, SDL_Renderer* renderer, int windowWidth, int windowHeight);
	~Game();
	Level& getLevel() { return level; }
	std::vector<Plant>& getListPlants() { return listPlants; }
	std::vector<Animal>& getListAnimals() { return listAnimals; }


private:
	void processEvents(SDL_Renderer* renderer, bool& running);
	void update(float dT);
	void draw(SDL_Renderer* renderer);

	void setPlantTypeIDSelected(int setPlantTypeIDSelected);
	void addPlant(SDL_Renderer* renderer, Vector2D posMouse);
	void removePlantsAtMousePosition(Vector2D posMouse);
	void removePlantsIfTilesChanged();

	void setAnimalTypeIDSelected(int setAnimalTypeIDSelected);
	void addAnimal(SDL_Renderer* renderer, Vector2D posMouse);
	void removeAnimalsAtMousePosition(Vector2D posMouse);
	void removeAnimalsIfTilesChanged();


	int mouseDownStatus = 0;

	const int tileSize = 64;
	Level level;

	int plantTypeIDSelected = 0;
	int animalTypeIDSelected = 0;

	std::vector<Plant> listPlants;
	std::vector<Animal> listAnimals;

	SDL_Texture* textureShadows = nullptr;
};