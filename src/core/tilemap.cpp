#include "core/tilemap.hpp"
#include "core/tracelog.hpp"

Tile createTile(uint32_t y, uint32_t x, uint32_t tileSize, uint32_t textureWidth, uint32_t textureHeight){
    Tile tile = {};
    float tileWidth = (float)tileSize / textureWidth;
    float tileHeight = (float)tileSize / textureHeight;

    float tileLeft = tileWidth * x;
    float tileRight = tileWidth * (x + 1);
    float tileBottom = tileHeight * y;
    float tileTop = tileHeight * (y + 1);

    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, 1.0f, 0.0f, tileLeft, tileTop,
        1.0f, 0.0f, 0.0f, tileRight, tileBottom,
        0.0f, 0.0f, 0.0f, tileLeft, tileBottom, 

        0.0f, 1.0f, 0.0f, tileLeft, tileTop,
        1.0f, 1.0f, 0.0f, tileRight, tileTop,
        1.0f, 0.0f, 0.0f, tileRight, tileBottom 
    };
    for(int i = 0; i < QUAD_VERTEX_SIZE; i++){
        tile.vertices[i] = vertices[i];
    }

    tile.vertCount = QUAD_VERTEX_SIZE;
    tile.width = tileSize;
    tile.height = tileSize;
    return tile;
}

TileSet createTileSet(uint32_t textureWidth, uint32_t textureHeight, uint32_t tileSize){
    TileSet tileset = {};
    uint32_t colTiles = textureWidth / tileSize;
    uint32_t rowTiles = textureHeight / tileSize;

    for(int i = 0; i < rowTiles; i++){
        for (int j = 0; j < colTiles; j++){
            Tile tile = createTile(i, j, tileSize, textureWidth, textureHeight);
            tileset.tiles.push_back(tile);
        }
    }
    tileset.columns = colTiles;
    tileset.rows = rowTiles;

    return tileset;
}

TileMap createTilemap(uint32_t width, uint32_t height, uint32_t tileSize){
    TileMap map = {};
    map.width = width;
    map.height = height;
    map.tileSize = tileSize;
    
    Texture* t = loadTexture("assets/sprites/tileset01.png");
    TileSet tileset = createTileSet(t->width, t->height, tileSize);
    map.tileset = tileset;
    map.tileset.textureId = t->id;
    return map;
}


void renderTileMap(Renderer* renderer, TileMap map){
    glm::mat4 projection = glm::ortho(0.0f, (float)renderer->width, (float)renderer->height, 0.0f, -1.0f, 1.0f);

    setUniform(&renderer->shader, "projection", projection);
    uint32_t xpos = 0;
    uint32_t ypos = 0;

    if(map.tiles.size() < map.width * map.height){
        LOGERROR("Non ci sono abbastanza tiles da renderizzare");
        exit(0);
    }

    for(int i = 0; i < map.height; i++){
        ypos = i * map.tiles[i].width;
        for(int j = 0; j < map.width; j++){
            Tile tile = map.tiles[j + (i * map.height)];
            glm::mat4 model = glm::mat4(1.0f);
            xpos = j * tile.width;
            model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
            model = glm::scale(model, glm::vec3(tile.width, tile.height, 0.0f));
            setUniform(&renderer->shader, "model", model);
            renderDraw(renderer, map.tileset.textureId, tile.vertices, tile.vertCount);
        }
    }

}

void renderTileSet(Renderer* renderer, TileSet set){
    glm::mat4 projection = glm::ortho(0.0f, (float)renderer->width, (float)renderer->height, 0.0f, -1.0f, 1.0f);


    setUniform(&renderer->shader, "projection", projection);
    uint32_t xpos = 0;
    uint32_t ypos = 0;

    for(int i = 0; i < set.tiles.size(); i++){
        Tile tile = set.tiles[i];
        glm::mat4 model = glm::mat4(1.0f);
        xpos = tile.width * (i % set.columns);
        if(!xpos){
            ypos++;
        }
        model = glm::translate(model, glm::vec3(tile.width * (i % 20), tile.height * ypos, 0.0f));
        model = glm::scale(model, glm::vec3(tile.width, tile.height, 0.0f));
        setUniform(&renderer->shader, "model", model);
        renderDraw(renderer, set.textureId, set.tiles[i].vertices, set.tiles[i].vertCount);
    }
}