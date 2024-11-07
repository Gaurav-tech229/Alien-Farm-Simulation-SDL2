#include "Plant.h"
#include "Game.h"


const std::vector<Plant::Type> Plant::listPlantTypes = {
	{ "Plant 1", 1, true },
	{ "Plant 2", 1, true },
	{ "Plant 3", 1, false },
	{ "Plant 4", 2, false },
	{ "Plant 5", 2, false }
};




Plant::Plant(SDL_Renderer* renderer, int setTypeID, Vector2D setPos) :
	typeID(setTypeID), pos(setPos), timerGrowth(7.5f + MathAddon::randFloat() * 7.5f),
	timerMoveUpAndDown(2.0f, MathAddon::randFloat() * 2.0f) {

	if (setTypeID > -1 && setTypeID < listPlantTypes.size()) {
		std::string name = listPlantTypes[setTypeID].name;
		textureSmallMain = TextureLoader::loadTexture(renderer, name + " Small.bmp");
		textureSmallShadow = TextureLoader::loadTexture(renderer, name + " Small Shadow.bmp");
		textureMain = TextureLoader::loadTexture(renderer, name + ".bmp");
		textureShadow = TextureLoader::loadTexture(renderer, name + " Shadow.bmp");

		//Offset the plant's position based on it's size.
		pos += computeOffset(setTypeID);
	}
}



void Plant::update(float dT) {
	//Grow plant if needed.
	timerGrowth.countUp(dT);


	//Update the scale that this is drawn to make it appear like it's moving up and down.
	timerMoveUpAndDown.countUp(dT);
	if (timerMoveUpAndDown.timeSIsMax())
		timerMoveUpAndDown.resetToZero();

	//Compute the draw scale by using a cosine wave and the timer's time.
	float fDrawScaleMin = 0.95f;
	float fCos = (cos(timerMoveUpAndDown.computeFTime() * 2.0f * MathAddon::PI) + 1.0f) / 2.0f;
	fDrawScale = fDrawScaleMin + (1.0f - fDrawScaleMin) * fCos;
}



void Plant::draw(SDL_Renderer* renderer, int tileSize) {
	if (timerGrowth.timeSIsMax())
		//Fully grown.
		drawTexture(renderer, textureMain, tileSize);
	else
		//Still growing.
		drawTexture(renderer, textureSmallMain, tileSize);
}


void Plant::drawShadow(SDL_Renderer* renderer, int tileSize) {
	if (timerGrowth.timeSIsMax())
		//Fully grown.
		drawTexture(renderer, textureShadow, tileSize);
	else
		//Still growing.
		drawTexture(renderer, textureSmallShadow, tileSize);
}


void Plant::drawTexture(SDL_Renderer* renderer, SDL_Texture* textureSelected, int tileSize) {
	if (renderer != nullptr && textureSelected != nullptr) {
		int w, h;
		SDL_QueryTexture(textureSelected, NULL, NULL, &w, &h);

		w = (int)round(w * fDrawScale);
		h = (int)round(h * fDrawScale);

		SDL_Rect rect = {
			(int)round(pos.x * tileSize) - w / 2,
			(int)round(pos.y * tileSize) - h / 2,
			w,
			h };
		SDL_RenderCopy(renderer, textureSelected, NULL, &rect);
	}
}



bool Plant::checkOverlapWithPlantTypeID(int x, int y, int plantTypeID) {
	if (plantTypeID > -1 && plantTypeID < listPlantTypes.size())
		return checkOverlap(x, y, listPlantTypes[plantTypeID].size);

	return false;
}


bool Plant::checkOverlapWithMouse(int x, int y) {
	return checkOverlap(x, y, 1);
}


bool Plant::checkOverlap(int x, int y, int size) {
	//Setup a rectangle that represents the tiles that this plant overlaps, and
	//another rectangle that represents the input.  Then check if they overlap.

	if (typeID > -1 && typeID < listPlantTypes.size()) {
		float offset1 = computeOffset(typeID);

		int rect1Left = (int)(pos.x - offset1);
		int rect1Top = (int)(pos.y - offset1);
		int rect1Right = rect1Left + listPlantTypes[typeID].size - 1;
		int rect1Bottom = rect1Top + listPlantTypes[typeID].size - 1;

		int rect2Left = x;
		int rect2Top = y;
		int rect2Right = rect2Left + size - 1;
		int rect2Bottom = rect2Top + size - 1;

		//If there is a gap between any of the sides then they don't overlap.
		return ((rect1Left > rect2Right || rect2Left > rect1Right ||
			rect1Top > rect2Bottom || rect2Top > rect1Bottom) == false);
	}

	return false;
}



float Plant::computeOffset(int plantTypeID) {
	if (plantTypeID > -1 && plantTypeID < listPlantTypes.size())
		return (listPlantTypes[plantTypeID].size - 1) / 2.0f;

	return 0.0f;
}



bool Plant::checkIfTilesUnderOk(Level& level) {
	float offset = computeOffset(typeID);

	int x = (int)(pos.x - offset);
	int y = (int)(pos.y - offset);

	return checkIfTilesUnderOkForType(x, y, typeID, level);
}


bool Plant::checkIfTilesUnderOkForType(int x, int y, int plantTypeID, Level& level) {
	//Check if the tile types on the level, under the input plant type are correct for it or not.

	if (plantTypeID > -1 && plantTypeID < listPlantTypes.size()) {
		const Type& typeSelected = listPlantTypes[plantTypeID];

		int rectLeft = x;
		int rectTop = y;
		int rectRight = rectLeft + typeSelected.size - 1;
		int rectBottom = rectTop + typeSelected.size - 1;

		for (int y2 = rectTop; y2 <= rectBottom; y2++)
			for (int x2 = rectLeft; x2 <= rectRight; x2++)
				if (level.checkIfTileOkForPlant(x2, y2, typeSelected.growsOnWetDirt) == false)
					return false;

		return true;
	}

	return false;
}



bool Plant::checkIfPositionOkForType(Vector2D posCheck, int plantTypeID, Game& game) {
	//Check if the input position is ok or blocked.
	if (plantTypeID > -1 && plantTypeID < listPlantTypes.size()) {
		//Check overlap with the level.
		if (checkIfTilesUnderOkForType((int)posCheck.x, (int)posCheck.y, plantTypeID, 
			game.getLevel()) == false)
			return false;

		//Check overlap with animals.
		Vector2D posCheckWithOffset = posCheck + computeOffset(plantTypeID);
		float radiusCheck = computeRadius(plantTypeID);
		for (auto& animalSelected : game.getListAnimals())
			if (animalSelected.checkCircleOverlap(posCheckWithOffset, radiusCheck))
				return false;

		//Check overlap with plants.
		for (auto& plantSelected : game.getListPlants())
			if (plantSelected.checkOverlapWithPlantTypeID((int)posCheck.x, (int)posCheck.y, plantTypeID))
				return false;

		return true;
	}

	return false;
}



bool Plant::checkCircleOverlap(Vector2D posCircle, float radiusCircle) {
	if (typeID > -1 && typeID < listPlantTypes.size()) {
		//For simplicity assume that this plant is bound by a circle at it's position pos,
		//and radius as follows.
		return (pos - posCircle).magnitude() <= (computeRadius(typeID) + radiusCircle);
	}

	return false;
}



float Plant::computeRadius(int plantTypeID) {
	if (plantTypeID > -1 && plantTypeID < listPlantTypes.size())
		//For simplicity approximate the plant's radius based on it's size.
		return (listPlantTypes[plantTypeID].size * 0.80f / 2.0f);

	return 0.0f;
}