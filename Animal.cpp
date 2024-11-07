#include "Animal.h"
#include "Game.h"


const std::vector<Animal::Type> Animal::listAnimalTypes = {
	{ "Animal 1", 0.5f },
	{ "Animal 2", 0.6f },
	{ "Animal 3", 0.95f }
};




Animal::Animal(SDL_Renderer* renderer, int setTypeID, Vector2D setPos, float setAngle) :
	stateCurrent(State::idle), typeID(setTypeID), pos(setPos), angle(setAngle), timerStateIdle(1.0f),
	timerGrowth(7.5f + MathAddon::randFloat() * 7.5f) {

	if (setTypeID > -1 && setTypeID < listAnimalTypes.size()) {
		std::string name = listAnimalTypes[setTypeID].name;
		textureSmallMain = TextureLoader::loadTexture(renderer, name + " Small.bmp");
		textureSmallShadow = TextureLoader::loadTexture(renderer, name + " Small Shadow.bmp");
		textureMain = TextureLoader::loadTexture(renderer, name + ".bmp");
		textureShadow = TextureLoader::loadTexture(renderer, name + " Shadow.bmp");
	}
}



void Animal::update(float dT, Game& game) {
	//Grow animal if needed.
	timerGrowth.countUp(dT);


	//Update this animal based on it's current state.
	switch (stateCurrent) {
	case State::idle:
		updateStateIdle(dT, game);
		break;
	case State::moving:
		//Note: Bitwise and is used on purpose here so that both functions are called.
		if (updateMove(dT) & updateAngle(dT))
			stateCurrent = State::idle;
		break;
	case State::rotating:
		if (updateAngle(dT))
			stateCurrent = State::idle;
		break;
	}
}


void Animal::updateStateIdle(float dT, Game& game) {
	//Periodically check if a new target point to move to is needed.
	timerStateIdle.countUp(dT);
	if (timerStateIdle.timeSIsMax()) {
		timerStateIdle.resetToZero();

		float probRandom = MathAddon::randFloat();
		const float probMove = 0.1f, probRotate = 0.2f;

		if (probRandom < probMove)
			moveToRandomPosition(game);
		else if (probRandom < (probMove + probRotate))
			rotateToRandomAngle();
	}
}


bool Animal::updateMove(float dT) {
	//Move this towards the target point.
	bool reachedPosTarget = false;

	//Determine the distance to move this frame.
	float distanceMove = speed * dT;
	if (distanceMove > distanceToTarget) {
		distanceMove = distanceToTarget;
		reachedPosTarget = true;
	}

	pos += (directionNormalTarget * distanceMove);
	distanceToTarget -= distanceMove;

	return reachedPosTarget;
}


bool Animal::updateAngle(float dT) {
	//Rotate this towards the target point.
	bool reachedAngleTarget = false;

	//Determine the angle to the target.
	float angleToTarget = Vector2D(angle).angleBetween(directionNormalTarget);

	//Determine the angle to move this frame.
	float angleMove = copysign(speedAngular * dT, angleToTarget);

	//Update the angle as required.
	if (abs(angleMove) > abs(angleToTarget)) {
		//It will point directly at it's target this frame.
		angle = directionNormalTarget.angle();
		reachedAngleTarget = true;
	}
	else {
		//It won't reach it's target this frame.
		angle += angleMove;
	}

	return reachedAngleTarget;
}



void Animal::moveToRandomPosition(Game& game) {
	for (int count = 0; count < 10; count++) {
		Vector2D normal = Vector2D(MathAddon::randAngleRad());
		float distance = MathAddon::randFloat() * 1.0f + 0.5f;

		Vector2D posCheck = pos + (normal * distance);
		if (checkIfPositionOK(posCheck, game)) {
			directionNormalTarget = normal;
			distanceToTarget = distance;
			stateCurrent = State::moving;
			return;
		}
	}
}


void Animal::rotateToRandomAngle() {
	directionNormalTarget = Vector2D(MathAddon::randAngleRad());
	distanceToTarget = 0.0f;
	stateCurrent = State::rotating;
}



void Animal::draw(SDL_Renderer* renderer, int tileSize) {
	if (timerGrowth.timeSIsMax())
		//Fully grown.
		drawTextureWithOffset(renderer, textureMain, tileSize, 0);
	else
		//Still growing.
		drawTextureWithOffset(renderer, textureSmallMain, tileSize, 0);
}


void Animal::drawShadow(SDL_Renderer* renderer, int tileSize) {
	if (timerGrowth.timeSIsMax())
		//Fully grown.
		drawTextureWithOffset(renderer, textureShadow, tileSize, 8);
	else
		//Still growing.
		drawTextureWithOffset(renderer, textureSmallShadow, tileSize, 5);
}


void Animal::drawTextureWithOffset(SDL_Renderer* renderer, SDL_Texture* textureSelected,
	int tileSize, int offset) {
	if (renderer != nullptr && textureSelected != nullptr) {
		int w, h;
		SDL_QueryTexture(textureSelected, NULL, NULL, &w, &h);

		SDL_Rect rect = {
			(int)round(pos.x * tileSize) - w / 2 - offset,
			(int)round(pos.y * tileSize) - h / 2 + offset,
			w,
			h };
		SDL_RenderCopyEx(renderer, textureSelected, NULL, &rect,
			MathAddon::angleRadToDeg(angle), NULL, SDL_FLIP_NONE);
	}
}



bool Animal::checkIfTilesUnderOk(Level& level) {
	if (typeID > -1 && typeID < listAnimalTypes.size())
		return level.checkIfPositionOkForAnimal(pos, listAnimalTypes[typeID].radius);

	return false;
}


bool Animal::checkIfPositionOkForType(Vector2D posCheck, int animalTypeID, Game& game) {
	return checkIfPositionOkGeneral(posCheck, animalTypeID, nullptr, game);
}


bool Animal::checkIfPositionOK(Vector2D posCheck, Game& game) {
	return checkIfPositionOkGeneral(posCheck, typeID, this, game);
}


bool Animal::checkIfPositionOkGeneral(Vector2D posCheck, int animalTypeID, Animal* animalExclude,
	Game& game) {
	//Check if the input position is ok or blocked.
	if (animalTypeID > -1 && animalTypeID < listAnimalTypes.size()) {
		float radiusCheck = listAnimalTypes[animalTypeID].radius;

		//Check overlap with the level.
		if (game.getLevel().checkIfPositionOkForAnimal(posCheck, radiusCheck) == false)
			return false;

		//Check overlap with animals.
		for (auto& animalSelected : game.getListAnimals())
			if ((&animalSelected != animalExclude) &&
				animalSelected.checkCircleOverlap(posCheck, radiusCheck))
				return false;

		//Check overlap with plants.
		for (auto& plantSelected : game.getListPlants())
			if (plantSelected.checkCircleOverlap(posCheck, radiusCheck))
				return false;

		return true;
	}

	return false;
}



bool Animal::checkCircleOverlap(Vector2D posCircle, float radiusCircle) {
	if (typeID > -1 && typeID < listAnimalTypes.size())
		return (pos - posCircle).magnitude() <= (listAnimalTypes[typeID].radius + radiusCircle);

	return false;
}