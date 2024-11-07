#include "Tile.h"


const std::vector<Tile::Type> Tile::listTileTypes = {
	{ "water", 0, SDL_Color{ 0, 67, 190 }, SDL_Color{ 0, 67, 190 } },
	{ "dirt", 1, SDL_Color{ 138, 47, 50 }, SDL_Color{ 158, 56, 62 } },
	{ "grassPurple", 2, SDL_Color{ 184, 33, 117 }, SDL_Color{ 218, 50, 143 } },
	{ "grassGreen", 2, SDL_Color{ 11, 100, 100 }, SDL_Color{ 14, 131, 131 } },
	{ "grassYellow", 2, SDL_Color{ 184, 176, 33 }, SDL_Color{ 218, 209, 50 } },
	{ "grassBlue", 2, SDL_Color{ 33, 41, 184 }, SDL_Color{ 50, 59, 218 } },
	{ "grassWhite", 2, SDL_Color{ 167, 167, 167 }, SDL_Color{ 199, 199, 199 } }
};

std::vector<SDL_Texture*> Tile::listTextureTileShadows;




Tile::Tile(SDL_Renderer* renderer) :
	typeID(2) {

	//Load the shadow textures once only.
	if (listTextureTileShadows.empty()) {
		std::vector<std::string> listTileShadowNames{ "Top Left", "Top", "Top Right", "Left",
				"Right", "Bottom Left", "Bottom", "Bottom Right" };

		for (const auto& nameSelected : listTileShadowNames)
			listTextureTileShadows.push_back(TextureLoader::loadTexture(renderer,
				"Tile Shadow " + nameSelected + ".bmp"));
	}
}



void Tile::draw(SDL_Renderer* renderer, int x, int y, int tileSize) {
	if (typeID > -1 && typeID < listTileTypes.size()) {
		const Type& typeSelected = listTileTypes[typeID];
		bool dark = ((x + y) % 2 == 0);

		//Set the correct color.
		SDL_Color colorTile = (dark ? typeSelected.colorDark : typeSelected.colorLight);
		//If it's wet dirt then make it darker.
		if (typeSelected.name == "dirt" && isWet) {
			const float fWet = 0.65f;
			colorTile.r = (Uint8)(colorTile.r * fWet);
			colorTile.g = (Uint8)(colorTile.g * fWet);
			colorTile.b = (Uint8)(colorTile.b * fWet);
		}
		SDL_SetRenderDrawColor(renderer, colorTile.r, colorTile.g, colorTile.b, 255);

		//Draw the tile.
		SDL_Rect rect = { x * tileSize, y * tileSize, tileSize, tileSize };
		SDL_RenderFillRect(renderer, &rect);
	}
}


void Tile::drawShadows(SDL_Renderer* renderer, int x, int y, int tileSize,
	std::vector<Tile>& listTiles, int tileCountX, int tileCountY) {
	//Setup a rectangle for drawing purposes.
	SDL_Rect rect = { x * tileSize, y * tileSize, tileSize, tileSize };

	//Loop through the list and draw each shadow image as required.
	for (int count = 0; count < listTextureTileShadows.size(); count++) {
		SDL_Texture* textureSelected = listTextureTileShadows[count];
		if (textureSelected != nullptr) {
			//Map count to an index on a 3x3 grid.  If count is the center tile or greater then skip it.
			int index = count;
			if (count >= 4)
				index++;

			//Convert index to an x and y offset ranging from -1 to 1 for a 3x3 grid.
			int xOff = index % 3 - 1;
			int yOff = index / 3 - 1;

			//Check if offset tile is a corner, then draw the shadow image if required.
			bool isCorner = (abs(xOff) == 1 && abs(yOff) == 1);
			if (isCorner) {
				if (isTileHigher(x + xOff, y + yOff, listTiles, tileCountX, tileCountY) &&
					isTileHigher(x + xOff, y, listTiles, tileCountX, tileCountY) == false &&
					isTileHigher(x, y + yOff, listTiles, tileCountX, tileCountY) == false)
					SDL_RenderCopy(renderer, textureSelected, NULL, &rect);
			}
			else {
				if (isTileHigher(x + xOff, y + yOff, listTiles, tileCountX, tileCountY))
					SDL_RenderCopy(renderer, textureSelected, NULL, &rect);
			}
		}
	}
}



void Tile::setTypeID(int setTypeID) {
	if (setTypeID > -1 && setTypeID < listTileTypes.size())
		typeID = setTypeID;
}



void Tile::refreshSurroundingIsWet(int x, int y,
	std::vector<Tile>& listTiles, int tileCountX, int tileCountY) {
	//Refresh isWet for all the tiles within the specified distance of the input x, y position.
    
    //How far the water spreads.
	const int distance = 2;

	//Loop through any tiles surrounding the input x, y position.
	for (int x2 = x - distance; x2 <= x + distance; x2++) {
		for (int y2 = y - distance; y2 <= y + distance; y2++) {
			//Ensure that the tile exists.
			int index2 = x2 + y2 * tileCountX;
			if (index2 > -1 && index2 < listTiles.size() &&
				x2 > -1 && x2 < tileCountX &&
				y2 > -1 && y2 < tileCountY) {

				//Check to see if at least one tile surrounding the x2, y2 position is water.
				bool foundWater = false;
				for (int x3 = x2 - distance; x3 <= x2 + distance && foundWater == false; x3++) {
					for (int y3 = y2 - distance; y3 <= y2 + distance && foundWater == false; y3++) {
						//Ensure that the tile exists.
						int index3 = x3 + y3 * tileCountX;
						if (index3 > -1 && index3 < listTiles.size() &&
							x3 > -1 && x3 < tileCountX &&
							y3 > -1 && y3 < tileCountY) {

							//Ensure that the typeID exists, then check if it's water.
							int typeIDSelected = listTiles[index3].typeID;
							if (typeIDSelected > -1 && typeIDSelected < listTileTypes.size() &&
								listTileTypes[typeIDSelected].name == "water") {
								foundWater = true;
							}
						}
					}
				}

				//Set isWet for the selected tile.
				listTiles[index2].isWet = foundWater;
			}
		}
	}
}



bool Tile::isTileHigher(int x, int y,
	std::vector<Tile>& listTiles, int tileCountX, int tileCountY) {
	if (typeID > -1 && typeID < listTileTypes.size()) {
		int indexOther = x + y * tileCountX;
		if (indexOther > -1 && indexOther < listTiles.size() &&
			x > -1 && x < tileCountX &&
			y > -1 && y < tileCountY) {
			int typeIDOther = listTiles[indexOther].typeID;

			//Compare the heights.
			if (typeIDOther > -1 && typeIDOther < listTileTypes.size())
				return listTileTypes[typeIDOther].height > listTileTypes[typeID].height;
		}
	}

	return false;
}



bool Tile::checkIfOkForPlant(bool growsOnWetDirt) {
	if (typeID > -1 && typeID < listTileTypes.size()) {
		if (listTileTypes[typeID].name == "water")
			return false;
		else if (listTileTypes[typeID].name == "dirt")
			return (growsOnWetDirt ? isWet : false);
		else if (listTileTypes[typeID].name.substr(0, 5) == "grass")
			return (growsOnWetDirt ? false : true);
	}

	return false;
}



bool Tile::checkIfOkForAnimal(int x, int y, Vector2D posCircle, float radiusCircle) {
	if (typeID > -1 && typeID < listTileTypes.size()) {
		//Check if the input circle overlaps this tile or not.
		if (checkCircleOverlap(x, y, posCircle, radiusCircle)) {
			//It does so check if the type is ok for animals.
			if (listTileTypes[typeID].name == "water")
				return false;
			else if (listTileTypes[typeID].name == "dirt")
				return true;
			else if (listTileTypes[typeID].name.substr(0, 5) == "grass")
				return true;
		}

		return true;
	}

	return false;
}



bool Tile::checkCircleOverlap(int x, int y, Vector2D posCircle, float radiusCircle) {
	//Define a rectangle for the edges of the tile.
	float rectLeft = (float)(x);
	float rectTop = (float)(y);
	float rectRight = (float)(x + 1);
	float rectBottom = (float)(y + 1);

	//Start with the center of the circle.
	Vector2D posCheck(posCircle);

	//Constrain posCheck to be within the tile or on it's edge.
	posCheck.x = std::min(std::max(posCheck.x, rectLeft), rectRight);
	posCheck.y = std::min(std::max(posCheck.y, rectTop), rectBottom);

	//Then check if posCheck is within the circle or not.
	return ((posCircle - posCheck).magnitude() < radiusCircle);
}