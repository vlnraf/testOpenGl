#include "core/tilemap.hpp"
#include "core/tracelog.hpp"

Tile createTile(uint32_t y, uint32_t x, float tileSize, uint32_t textureWidth, uint32_t textureHeight){
    Tile tile = {};
    float tileWidth = (float)tileSize / textureWidth;
    float tileHeight = (float)tileSize / textureHeight;

    float tileLeft = tileWidth * x;
    float tileRight = tileWidth * (x + 1);
    float tileBottom = tileHeight * y;
    float tileTop = tileHeight * (y + 1);

    float vertices[QUAD_VERTEX_SIZE] = {
        // pos              // tex
        0.0f, 1.0f, 0.0f, tileLeft, tileBottom,
        1.0f, 0.0f, 0.0f, tileRight, tileTop,
        0.0f, 0.0f, 0.0f, tileLeft, tileTop, 

        0.0f, 1.0f, 0.0f, tileLeft, tileBottom,
        1.0f, 1.0f, 0.0f, tileRight, tileBottom,
        1.0f, 0.0f, 0.0f, tileRight, tileTop 
    };
    for(int i = 0; i < QUAD_VERTEX_SIZE; i++){
        tile.vertices[i] = vertices[i];
    }

    tile.vertCount = QUAD_VERTEX_SIZE;
    tile.width = tileSize;
    tile.height = tileSize;

    return tile;
}

TileSet createTileSet(Texture* texture, float tileSize){
    TileSet tileset = {};
    uint32_t colTiles = texture->width / tileSize;
    uint32_t rowTiles = texture->height / tileSize;

    //read from bottom to top because different texture coordinates
    //between opengl and std_image
    for(int i = 0; i < rowTiles; i++){
        for (int j = 0; j < colTiles; j++){
            Tile tile = createTile(i, j, tileSize, texture->width, texture->height);
            tileset.tiles.push_back(tile);
        }
    }
    tileset.columns = colTiles;
    tileset.rows = rowTiles;
    tileset.textureId = texture->id;

    return tileset;
}

TileMap createTilemap(std::vector<uint32_t> tileIdx, uint32_t width, uint32_t height, float tileSize, TileSet tileSet){
    TileMap map = {};
    map.width = width;
    map.height = height;
    map.tileSize = tileSize;
    
    map.tileset = tileSet;
    map.tileset.textureId = tileSet.textureId;

    for(int i = 0; i < map.height; i++){
        for(int j = 0; j < map.width; j++){
            Tile tile = tileSet.tiles[tileIdx[j + (i * map.width)]];
            tile.xPos = j;
            tile.yPos = (map.height - i - 1);
            tile.width = tileSize;
            tile.height = tileSize;
            map.tiles.push_back(tile);
        }
    }
    return map;
}


void renderTileMap(Renderer* renderer, TileMap map){
    if(map.tiles.size() < map.width * map.height){
        LOGERROR("Non ci sono abbastanza tiles da renderizzare");
        exit(0);
    }
    if(map.tiles.size() > map.width * map.height){
        LOGERROR("Ci sono troppe tiles rispetto alla grandezza della mappa");
        exit(0);
    }

    float xpos = 0;
    float ypos = 0;

    for(int i = 0; i < map.height; i++){
        for(int j = 0; j < map.width; j++){
            Tile tile = map.tiles[j + (i * map.width)];
            glm::mat4 model = glm::mat4(1.0f);
            xpos = tile.xPos * tile.width;
            ypos = tile.yPos * tile.height;
            model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
            model = glm::scale(model, glm::vec3(tile.width, tile.height, 0.0f));
            setUniform(&renderer->shader, "model", model);
            renderDraw(renderer, map.tileset.textureId, tile.vertices, tile.vertCount);
        }
    }

}

void renderTileSet(Renderer* renderer, TileSet set){
    //glm::mat4 projection = glm::ortho(0.0f, (float)renderer->width, (float)renderer->height, 0.0f, -1.0f, 1.0f);
    //glm::mat4 projection = glm::ortho(0.0f, 300.0f, 200.0f, 0.0f, -1.0f, 1.0f);


    //setUniform(&renderer->shader, "projection", projection);
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