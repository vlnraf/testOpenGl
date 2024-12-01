#pragma once

#ifdef GAME_EXPORT
#define GAME_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllimport)
#endif

#include "input.hpp"
#include "shader.hpp"
#include "scene.hpp"

struct GameState{
    Scene scene;
};

extern "C" {
    GAME_API GameState* gameStart(const char* testo, Renderer* renderer);
    GAME_API void gameRender(GameState* gameState, Renderer* renderer);
    GAME_API void gameUpdate(GameState* gameState, Input* input);
}