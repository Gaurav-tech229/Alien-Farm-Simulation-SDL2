#include "Game.h"



Game::Game(SDL_Window* window, SDL_Renderer* renderer, int windowWidth, int windowHeight) :
    placementModeCurrent(PlacementMode::tiles),
    level(renderer, windowWidth / tileSize + (windowWidth % tileSize > 0),
        windowHeight / tileSize + (windowHeight % tileSize > 0)) {
    //Run the game.
    if (window != nullptr && renderer != nullptr) {
        //Initialize a texture that will be used to draw the shadows.
        textureShadows = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);
        SDL_SetTextureBlendMode(textureShadows, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(textureShadows, (int)round(255 * 0.60f));

        //Store the current times for the clock.
        auto time1 = std::chrono::system_clock::now();
        auto time2 = std::chrono::system_clock::now();


        //Start the game loop and run until it's time to stop.
        bool running = true;
        while (running) {
            //Determine how much time has elapsed since the last frame.
            time2 = std::chrono::system_clock::now();
            std::chrono::duration<float> timeDelta = time2 - time1;
            float timeDeltaFloat = timeDelta.count();

            //Store the new time for the next frame.
            time1 = time2;

            //The amount of time for each frame (no longer than 20 fps).
            const float dT = std::min(timeDeltaFloat, 1.0f / 20.0f);

            processEvents(renderer, running);
            update(dT);
            draw(renderer);
        }
    }
}


Game::~Game() {
    //Clean up.
    if (textureShadows != nullptr) {
        SDL_DestroyTexture(textureShadows);
        textureShadows = nullptr;
    }

    TextureLoader::deallocateTextures();
}



void Game::processEvents(SDL_Renderer* renderer, bool& running) {
    bool mouseDownThisFrame = false;

    //Process events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_MOUSEBUTTONDOWN:
            mouseDownThisFrame = (mouseDownStatus == 0);
            if (event.button.button == SDL_BUTTON_LEFT)
                mouseDownStatus = SDL_BUTTON_LEFT;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                mouseDownStatus = SDL_BUTTON_RIGHT;
            break;
        case SDL_MOUSEBUTTONUP:
            mouseDownStatus = 0;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                //Quit the game.
            case SDL_SCANCODE_ESCAPE:
                running = false;
                break;

                //Select the level's tileTypeIDSelected.
            case SDL_SCANCODE_1:
            case SDL_SCANCODE_2:
            case SDL_SCANCODE_3:
            case SDL_SCANCODE_4:
            case SDL_SCANCODE_5:
            case SDL_SCANCODE_6:
            case SDL_SCANCODE_7:
            {int tileTypeID = event.key.keysym.scancode - SDL_SCANCODE_1;
            level.setTileTypeIDSelected(tileTypeID);
            placementModeCurrent = PlacementMode::tiles; }
            break;

            //Select the plants.
            case SDL_SCANCODE_Q:
                setPlantTypeIDSelected(0);
                break;
            case SDL_SCANCODE_W:
                setPlantTypeIDSelected(1);
                break;
            case SDL_SCANCODE_E:
                setPlantTypeIDSelected(2);
                break;
            case SDL_SCANCODE_R:
                setPlantTypeIDSelected(3);
                break;
            case SDL_SCANCODE_T:
                setPlantTypeIDSelected(4);
                break;

                //Select the animals.
            case SDL_SCANCODE_A:
                setAnimalTypeIDSelected(0);
                break;
            case SDL_SCANCODE_S:
                setAnimalTypeIDSelected(1);
                break;
            case SDL_SCANCODE_D:
                setAnimalTypeIDSelected(2);
                break;
            }
        }
    }


    //Process input from the mouse cursor.
    int mouseX = 0, mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    //Convert from the window's coordinate system to the game's coordinate system.
    Vector2D posMouse((float)mouseX / tileSize, (float)mouseY / tileSize);

    if (mouseDownStatus > 0) {
        //The mouse was pressed.
        switch (mouseDownStatus) {
        case SDL_BUTTON_LEFT:
            switch (placementModeCurrent) {
            case PlacementMode::tiles:
                level.placeTileTypeIDSelected((int)posMouse.x, (int)posMouse.y);
                removePlantsIfTilesChanged();
                removeAnimalsIfTilesChanged();
                break;
            case PlacementMode::plants:
                addPlant(renderer, posMouse);
                break;
            case PlacementMode::animals:
                addAnimal(renderer, posMouse);
                break;
            }
            break;

        case SDL_BUTTON_RIGHT:
            removePlantsAtMousePosition(posMouse);
            removeAnimalsAtMousePosition(posMouse);
            break;
        }
    }
}



void Game::update(float dT) {
    //Update the plants.
    for (auto& plantSelected : listPlants)
        plantSelected.update(dT);

    //Update the animals.
    for (auto& animalSelected : listAnimals)
        animalSelected.update(dT, *this);
}



void Game::draw(SDL_Renderer* renderer) {
    //Draw.
    //Set the background color.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //Clear the screen.
    SDL_RenderClear(renderer);


    //**********Layer 1 - Level**********
    level.draw(renderer, tileSize);


    //**********Layer 2 - Shadows**********
    //Switch the render target to textureShadows and clear it.
    SDL_SetRenderTarget(renderer, textureShadows);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    level.drawShadows(renderer, tileSize);

    //Draw the plants shadows.
    for (auto& plantSelected : listPlants)
        plantSelected.drawShadow(renderer, tileSize);

    //Draw the animals shadows.
    for (auto& animalSelected : listAnimals)
        animalSelected.drawShadow(renderer, tileSize);

    //Set the render target back to the window.
    SDL_SetRenderTarget(renderer, NULL);
    //Draw the texture.
    SDL_RenderCopy(renderer, textureShadows, NULL, NULL);


    //**********Layer 3 - Plants**********
    //Draw the plants.
    for (auto& plantSelected : listPlants)
        plantSelected.draw(renderer, tileSize);

    //Draw the animals.
    for (auto& animalSelected : listAnimals)
        animalSelected.draw(renderer, tileSize);


    //Send the image to the window.
    SDL_RenderPresent(renderer);
}



void Game::setPlantTypeIDSelected(int setPlantTypeIDSelected) {
    plantTypeIDSelected = setPlantTypeIDSelected;
    placementModeCurrent = PlacementMode::plants;
}


void Game::addPlant(SDL_Renderer* renderer, Vector2D posMouse) {
    float randOffsetX = (MathAddon::randFloat() * 2.0f - 1.0f) * 0.1f;
    float randOffsetY = (MathAddon::randFloat() * 2.0f - 1.0f) * 0.1f;
    Vector2D pos((int)posMouse.x + 0.5f + randOffsetX, (int)posMouse.y + 0.5f + randOffsetY);

    if (Plant::checkIfPositionOkForType(pos, plantTypeIDSelected, *this))
        listPlants.push_back(Plant(renderer, plantTypeIDSelected, pos));
}


void Game::removePlantsAtMousePosition(Vector2D posMouse) {
    for (auto it = listPlants.begin(); it != listPlants.end();)
        if ((*it).checkOverlapWithMouse((int)posMouse.x, (int)posMouse.y))
            it = listPlants.erase(it);
        else
            it++;
}


void Game::removePlantsIfTilesChanged() {
    for (auto it = listPlants.begin(); it != listPlants.end();)
        if ((*it).checkIfTilesUnderOk(level) == false)
            it = listPlants.erase(it);
        else
            it++;
}



void Game::setAnimalTypeIDSelected(int setAnimalTypeIDSelected) {
    animalTypeIDSelected = setAnimalTypeIDSelected;
    placementModeCurrent = PlacementMode::animals;
}


void Game::addAnimal(SDL_Renderer* renderer, Vector2D posMouse) {
    if (Animal::checkIfPositionOkForType(posMouse, animalTypeIDSelected, *this))
        listAnimals.push_back(Animal(renderer, animalTypeIDSelected, posMouse,
            MathAddon::randAngleRad()));
}


void Game::removeAnimalsAtMousePosition(Vector2D posMouse) {
    for (auto it = listAnimals.begin(); it != listAnimals.end();)
        if ((*it).checkCircleOverlap(posMouse, 0.0f))
            it = listAnimals.erase(it);
        else
            it++;
}


void Game::removeAnimalsIfTilesChanged() {
    for (auto it = listAnimals.begin(); it != listAnimals.end();)
        if ((*it).checkIfTilesUnderOk(level) == false)
            it = listAnimals.erase(it);
        else
            it++;
}