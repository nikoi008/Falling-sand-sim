#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h> 

#define GRID_W 200
#define GRID_H 200
#define PIXEL_SCALE 4

typedef enum { AIR = 0, SAND, WATER } blockType;

typedef struct {
    blockType type;
    bool updated;
} block;

static block grid[GRID_H][GRID_W];

bool inBounds(int x, int y) {
    return x >= 0 && x < GRID_W && y >= 0 && y < GRID_H;
}

void moveBlock(int x1, int y1, int x2, int y2) {
    block temp = grid[y1][x1];
    grid[y1][x1] = grid[y2][x2];
    grid[y2][x2] = temp;
    grid[y2][x2].updated = true;
    grid[y1][x1].updated = true;
}

void updateGrid(void) {
    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) grid[y][x].updated = false;
    }

    for (int y = GRID_H - 1; y >= 0; y--) {
        for (int x = 0; x < GRID_W; x++) {
            block *bloc = &grid[y][x];
            if (bloc->type == AIR || c->updated) continue;

            if (bloc->type == SAND) {
                if (inBounds(x, y + 1) && (grid[y+1][x].type == AIR || grid[y+1][x].type == WATER)) {
                    moveBlock(x, y, x, y + 1);
                } else if (inBounds(x - 1, y + 1) && (grid[y+1][x-1].type == AIR || grid[y+1][x-1].type == WATER)) {
                    moveBlock(x, y, x - 1, y + 1);
                } else if (inBounds(x + 1, y + 1) && (grid[y+1][x+1].type == AIR || grid[y+1][x+1].type == WATER)) {
                    moveBlock(x, y, x + 1, y + 1);
                }
            } 
            else if (bloc->type == WATER) {
                if (inBounds(x, y + 1) && grid[y+1][x].type == AIR) {
                    moveBlock(x, y, x, y + 1);
                } else if(inBounds(x - 1, y + 1) && grid[y+1][x-1].type == AIR) {
                    moveBlock(x, y, x - 1, y + 1);
                } else if(inBounds(x + 1, y + 1) && grid[y+1][x+1].type == AIR) {
                    moveBlock(x, y, x + 1, y + 1);
                } else {
                    int moveDir = (rand() % 2 == 0) ? 1 : -1;
                    if (inBounds(x + moveDir, y) && grid[y][x + moveDir].type == AIR) {
                        moveBlock(x, y, x + moveDir, y);
                    } else if (inBounds(x - moveDir, y) && grid[y][x - moveDir].type == AIR) {
                        moveBlock(x, y, x - moveDir, y);
                    }
                }
            }
        }
    }
}

int main(void) {
    InitWindow(GRID_W * PIXEL_SCALE, GRID_H * PIXEL_SCALE, "cool sand falling type shit");
    SetTargetFPS(60);

    blockType selected = SAND;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) selected = SAND;
        if (IsKeyPressed(KEY_TWO)) selected = WATER;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            int gridX = (int)mousePos.x / PIXEL_SCALE;
            int gridY = (int)mousePos.y / PIXEL_SCALE;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (inBounds(gridX + dx, gridY + dy) && grid[gridY + dy][gridX + dx].type == AIR) {
                        grid[gridY + dy][gridX + dx].type = selected;
                    }
                }
            }
        }
        updateGrid();
        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < GRID_H; y++) {
            for (int x = 0; x < GRID_W; x++) {//optimise??
                if (grid[y][x].type == SAND) DrawRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE, GOLD);
                else if (grid[y][x].type == WATER) DrawRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE, BLUE);
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}