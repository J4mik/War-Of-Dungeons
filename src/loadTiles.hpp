SDL_Texture* tempTexture;

SDL_Texture* grass;

SDL_Texture* livelyGrass;

SDL_Texture* darkGrass;

SDL_Texture* driedGrass;

SDL_Texture* ice;

SDL_Texture* dirt;

SDL_Texture* snow;

SDL_Texture* sand;

SDL_Texture* redSand;

SDL_Texture* stone;

SDL_Texture* playerTexture;

void loadTiles(SDL_Renderer* rend)
{
    grass = IMG_LoadTexture(rend, "data/images/GrassTiles.png");
    SDL_SetTextureScaleMode(grass, SDL_SCALEMODE_NEAREST);

    livelyGrass = IMG_LoadTexture(rend, "data/images/LivelyGrassTiles.png");
    SDL_SetTextureScaleMode(livelyGrass, SDL_SCALEMODE_NEAREST);

    darkGrass = IMG_LoadTexture(rend, "data/images/DarkGrassTiles.png");
    SDL_SetTextureScaleMode(darkGrass, SDL_SCALEMODE_NEAREST);

    driedGrass = IMG_LoadTexture(rend, "data/images/DriedGrassTiles.png");
    SDL_SetTextureScaleMode(driedGrass, SDL_SCALEMODE_NEAREST);

    ice = IMG_LoadTexture(rend, "data/images/IceTiles.png");
    SDL_SetTextureScaleMode(ice, SDL_SCALEMODE_NEAREST);

    dirt = IMG_LoadTexture(rend, "data/images/DirtTiles.png");
    SDL_SetTextureScaleMode(dirt, SDL_SCALEMODE_NEAREST);

    snow = IMG_LoadTexture(rend, "data/images/SnowTiles.png");
    SDL_SetTextureScaleMode(snow, SDL_SCALEMODE_NEAREST);

    sand = IMG_LoadTexture(rend, "data/images/SandTiles.png");
    SDL_SetTextureScaleMode(sand, SDL_SCALEMODE_NEAREST);

    redSand = IMG_LoadTexture(rend, "data/images/RedSandTiles.png");
    SDL_SetTextureScaleMode(redSand, SDL_SCALEMODE_NEAREST);

    stone = IMG_LoadTexture(rend, "data/images/StoneTiles.png");
    SDL_SetTextureScaleMode(stone, SDL_SCALEMODE_NEAREST);

    playerTexture = IMG_LoadTexture(rend, "data/images/player.png");
    SDL_SetTextureScaleMode(playerTexture, SDL_SCALEMODE_NEAREST);
}

void getTile(uint8_t tile)
{
    switch (tile)
    {
    case 0:
        tempTexture = driedGrass;
        break;
    case 1:
        tempTexture = sand;
        break;
    case 2:
        tempTexture = livelyGrass;
        break;
    case 3:
        tempTexture = stone;
        break;
    case 4:
        tempTexture = grass;
        break;
    case 5:
        tempTexture = dirt;
        break;
    case 6:
        tempTexture = redSand;
        break;
    case 7:
        tempTexture = snow;
        break;
    case 8:
        tempTexture = ice;
        break;
    case 9:
        tempTexture = darkGrass;
        break;

    default:
        // tempTexture = dirt;
        break;
    }
}
