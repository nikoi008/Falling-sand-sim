#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h> 

#define GRID_W 200
#define GRID_H 200
#define PIXEL_SCALE 4
#define UI_HEIGHT 80
#define TOTAL_BLOCKS 5

typedef enum { AIR = 0, SAND, WATER, ROCK, LAVA } blockType;

typedef struct {
    blockType type;
    bool updated;
    unsigned char colorVariation;
} block;

static block grid[GRID_H][GRID_W];

bool inBounds(int x, int y) {
    return x >= 0 && x < GRID_W && y >= 0 && y < GRID_H;
}

Color getBlockColor(block bloc) {
    float variation = (float)bloc.colorVariation / 255.0f; 
    switch (bloc.type) {
        case SAND:  return (Color){ (unsigned char)(230 + (variation * 25)), (unsigned char)(190 + (variation * 40)), (unsigned char)(100 + (variation * 50)), 255 };
        case WATER: return (Color){ (unsigned char)(30 + (variation * 20)), (unsigned char)(100 + (variation * 50)), (unsigned char)(200 + (variation * 55)), 255 };
        case ROCK:  return (Color){ (unsigned char)(80 + (variation * 40)), (unsigned char)(80 + (variation * 40)), (unsigned char)(85 + (variation * 40)), 255 };
        case LAVA:  return (Color){ (unsigned char)(220 + (variation * 35)), (unsigned char)(60 + (variation * 80)), 0, 255 };
        default:    return BLACK;
    }
}

void moveBlock(int x1, int y1, int x2, int y2) {
    block temp = grid[y1][x1];
    grid[y1][x1] = grid[y2][x2];
    grid[y2][x2] = temp;
    grid[y2][x2].updated = true;
}


void lavaWaterInteraction(int x, int y) {
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (inBounds(nx, ny) && grid[ny][nx].type == WATER) {
            grid[y][x].type = ROCK;
            grid[y][x].colorVariation = (unsigned char)(rand() % 256);
            grid[ny][nx].type = ROCK;
            grid[ny][nx].colorVariation = (unsigned char)(rand() % 256);
            return;
        }
    }
}

void updateGrid(void) {
    static int frameCount = 0;
    frameCount++;
    bool leftToRight = (frameCount % 2 == 0);
    bool lavaCanMove = (frameCount % 3 == 0); 

    for (int y = 0; y < GRID_H; y++) {
        for (int x = 0; x < GRID_W; x++) grid[y][x].updated = false;
    }

    for (int y = GRID_H - 1; y >= 0; y--) {
        for (int i = 0; i < GRID_W; i++) {
            int x = leftToRight ? i : (GRID_W - 1 - i);
            block *bloc = &grid[y][x];
            if (bloc->type == AIR || bloc->type == ROCK || bloc->updated) continue;

            if (bloc->type == SAND) {
                if (inBounds(x, y + 1) && (grid[y+1][x].type == AIR || grid[y+1][x].type == WATER)) {
                    moveBlock(x, y, x, y + 1);
                } else {
                    int first = leftToRight ? -1 : 1;
                    int second = leftToRight ? 1 : -1;
                    if (inBounds(x + first, y + 1) && (grid[y+1][x+first].type == AIR || grid[y+1][x+first].type == WATER)) {
                        moveBlock(x, y, x + first, y + 1);
                    } else if (inBounds(x + second, y + 1) && (grid[y+1][x+second].type == AIR || grid[y+1][x+second].type == WATER)) {
                        moveBlock(x, y, x + second, y + 1);
                    }
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
            else if (bloc->type == LAVA) {
                lavaWaterInteraction(x, y);
                if (grid[y][x].type != LAVA) continue; 

                if (!lavaCanMove) continue;

                if (inBounds(x, y + 1) && grid[y+1][x].type == AIR) {
                    moveBlock(x, y, x, y + 1);
                } else if (inBounds(x - 1, y + 1) && grid[y+1][x-1].type == AIR) {
                    moveBlock(x, y, x - 1, y + 1);
                } else if (inBounds(x + 1, y + 1) && grid[y+1][x+1].type == AIR) {
                    moveBlock(x, y, x + 1, y + 1);
                } else {
                    int moveDir = leftToRight ? 1 : -1;
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
#include <stdio.h>
int main(void) {
    InitWindow(GRID_W * PIXEL_SCALE, (GRID_H * PIXEL_SCALE) + UI_HEIGHT, "sand falling type shit");
    SetTargetFPS(60);

    blockType selected = SAND;
    int brushRadius = 2;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_ONE)) selected = AIR;
        if (IsKeyPressed(KEY_TWO)) selected = SAND;
        if (IsKeyPressed(KEY_THREE)) selected = WATER;
        if (IsKeyPressed(KEY_FOUR)) selected = ROCK;
        if (IsKeyPressed(KEY_FIVE)) selected = LAVA;

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            brushRadius += (int)wheel;
            if (brushRadius < 0) brushRadius = 0; 
            if (brushRadius > 20) brushRadius = 20; 
        }
        printf("%d\n",GetFPS());
        Vector2 mousePos = GetMousePosition();
        int gridX = (int)mousePos.x / PIXEL_SCALE;
        int gridY = (int)mousePos.y / PIXEL_SCALE;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && mousePos.y < (GRID_H * PIXEL_SCALE)) {
            for (int offsetY = -brushRadius; offsetY <= brushRadius; offsetY++) {
                for (int offsetX = -brushRadius; offsetX <= brushRadius; offsetX++) {
                    if (offsetX*offsetX + offsetY*offsetY <= brushRadius*brushRadius) {
                        int spawnX = gridX + offsetX;
                        int spawnY = gridY + offsetY;
                        if (inBounds(spawnX, spawnY)) {
                            if (selected == AIR) {
                                grid[spawnY][spawnX].type = AIR;
                            } else if (grid[spawnY][spawnX].type == AIR) {
                                grid[spawnY][spawnX].type = selected;
                                grid[spawnY][spawnX].colorVariation = (unsigned char)(rand() % 256);
                            }
                        }
                    }
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mousePos.y >= (GRID_H * PIXEL_SCALE)) {
            float sectionWidth = (float)GetScreenWidth() / (float)TOTAL_BLOCKS;
            selected = (blockType)(mousePos.x / sectionWidth);
        }

        updateGrid();

        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < GRID_H; y++) {
            for (int x = 0; x < GRID_W; x++) {
                if (grid[y][x].type != AIR) {
                    DrawRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE, getBlockColor(grid[y][x]));
                }
            }
        }

        int uiTop = GRID_H * PIXEL_SCALE;
        DrawRectangle(0, uiTop, GetScreenWidth(), UI_HEIGHT, DARKGRAY);
        
        char* blockNames[] = {"ERASE", "SAND", "WATER", "ROCK", "LAVA"};
        Color colors[] = {LIGHTGRAY, GOLD, BLUE, GRAY, ORANGE};
        
        for (int i = 0; i < TOTAL_BLOCKS; i++) {
            int xPos = i * (GetScreenWidth() / TOTAL_BLOCKS);
            Rectangle button = { (float)xPos + 5, (float)uiTop + 5, (float)(GetScreenWidth() / TOTAL_BLOCKS) - 10, (float)UI_HEIGHT - 10 };
            
            DrawRectangleRec(button, (selected == i) ? colors[i] : ColorAlpha(colors[i], 0.3f));
            DrawRectangleLinesEx(button, 2, (selected == i) ? WHITE : GRAY);
            
            int fontSize = 20;
            int textWidth = MeasureText(blockNames[i], fontSize);
            DrawText(blockNames[i], button.x + (button.width/2) - (textWidth/2), button.y + (button.height/2) - 10, fontSize, (selected == i) ? BLACK : WHITE);
        }

        DrawText("Dont forget to use the mouse wheel to make the brush larger or smaller!", 0, 0, 20, WHITE);

        if (mousePos.y < (GRID_H * PIXEL_SCALE)) {
            Color previewCol = YELLOW;
            if (selected == AIR) previewCol = WHITE;
            else if (selected == WATER) previewCol = BLUE;
            else if (selected == ROCK)  previewCol = GRAY;
            else if (selected == LAVA)  previewCol = ORANGE;

            DrawCircleLines(mousePos.x, mousePos.y, (brushRadius + 0.5f) * PIXEL_SCALE, WHITE);
            DrawCircle(mousePos.x, mousePos.y, (brushRadius + 0.5f) * PIXEL_SCALE, Fade(previewCol, 0.3f));
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}