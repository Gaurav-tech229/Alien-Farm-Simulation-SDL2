#include "Level.h"



Level::Level(SDL_Renderer* renderer, int setTileCountX, int setTileCountY) :
	tileCountX(setTileCountX), tileCountY(setTileCountY) {

	size_t listTilesSize = (size_t)tileCountX * tileCountY;
	listTiles.assign(listTilesSize, Tile(renderer));
}



void Level::draw(SDL_Renderer* renderer, int tileSize) {
	for (int y = 0; y < tileCountY; y++) {
		for (int x = 0; x < tileCountX; x++) {
			int index = x + y * tileCountX;
			listTiles[index].draw(renderer, x, y, tileSize);
		}
	}
}


void Level::drawShadows(SDL_Renderer* renderer, int tileSize) {
	for (int y = 0; y < tileCountY; y++) {
		for (int x = 0; x < tileCountX; x++) {
			int index = x + y * tileCountX;
			listTiles[index].drawShadows(renderer, x, y, tileSize,
				listTiles, tileCountX, tileCountY);
		}
	}
}



void Level::setTileTypeIDSelected(int setTileTypeIDSelected) {
	tileTypeIDSelected = setTileTypeIDSelected;
}


void Level::placeTileTypeIDSelected(int x, int y) {
	int index = x + y * tileCountX;
	if (index > -1 && index < listTiles.size() &&
		x > -1 && x < tileCountX &&
		y > -1 && y < tileCountY) {
		listTiles[index].setTypeID(tileTypeIDSelected);

		Tile::refreshSurroundingIsWet(x, y, listTiles, tileCountX, tileCountY);
	}
}



bool Level::checkIfTileOkForPlant(int x, int y, bool growsOnWetDirt) {
	int index = x + y * tileCountX;
	if (index > -1 && index < listTiles.size() &&
		x > -1 && x < tileCountX &&
		y > -1 && y < tileCountY)
		return listTiles[index].checkIfOkForPlant(growsOnWetDirt);

	return false;
}



bool Level::checkIfPositionOkForAnimal(Vector2D posCircle, float radiusCircle) {
	//Check if the input circle overlaps any tiles that are the wrong type for animals,
	//or isn't within the bounds of the level.

	//Define a rectangle that contains the input circle.
	int rectLeft = (int)(posCircle.x - radiusCircle);
	int rectTop = (int)(posCircle.y - radiusCircle);
	int rectRight = (int)(posCircle.x + radiusCircle);
	int rectBottom = (int)(posCircle.y + radiusCircle);

	//Ensure that it's within bounds of the level.
	//Check the top and left float values because (int)-0.1f casts to 0 but is out of bounds.
	if (rectLeft < 0 || rectTop < 0 || rectRight >= tileCountX || rectBottom >= tileCountY ||
		(posCircle.x - radiusCircle) < 0.0f || (posCircle.y - radiusCircle) < 0.0f)
		return false;
	else {
		//Loop through all the tiles for the above rectangle.
		for (int y = rectTop; y <= rectBottom; y++) {
			for (int x = rectLeft; x <= rectRight; x++) {
				int index = x + y * tileCountX;
				//Check if the input circle overlaps the tile and if it's type is ok.
				if (listTiles[index].checkIfOkForAnimal(x, y, posCircle, radiusCircle) == false)
					return false;
			}
		}
	}

	return true;
}