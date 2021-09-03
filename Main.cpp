#include "raylib.h"
#include <iostream>

struct AnimData {
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight) {
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame) {
    //- update running time  --------------------------------
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime) {
        data.runningTime = 0.0;

        //- Update animation frame -----------------------------
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame) {
            data.frame = 0;
        }
    }
    return data;
}

int main() {

    //- Window Dimensions --------------------------------
    const int windowDimensions[2] = {512 , 380};

    //- Initialize Window --------------------------------
    InitWindow(windowDimensions[0], windowDimensions[1], "Dasher Game!");

    //- acceleration due to gravity(pixels/s)/s --------------------------------
    const int gravity{1000};

    //- Nebula Variables ------------------------------
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    
    const int sizeOfNebulae{6};
    AnimData nebulae[sizeOfNebulae]{};

    for(int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
    } 

    for(int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].pos.x = windowDimensions[0] + (i*300);

    }

    float finishLine{nebulae[sizeOfNebulae-1].pos.x};

    //- Nebula x velocity --------------------------------
    int nebVel{-250};

    //- Player Variables ----------------------------------------------------
    Texture2D player = LoadTexture("textures/scarfy.png");
    AnimData playerData;
    playerData.rec.width = player.width/6;
    playerData.rec.height = player.height;
    playerData.rec.x = 0;
    playerData.rec.y = 0;
    playerData.pos.x = windowDimensions[0]/2 - playerData.rec.width/2;
    playerData.pos.y = windowDimensions[1] - playerData.rec.height;
    playerData.frame = 0;
    playerData.updateTime = 1.0/12.0;
    playerData.runningTime = 0.0;

    int velocity{0};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX{};
    float mgX{};
    float fgX{};

    //- Is player in the air? --------------------------------
    bool isInAir{};

    //- Jump velocity(pixels/s) --------------------------------
    const int jumpVelocity{-600};

    bool collision{};
    SetTargetFPS(60);
    //- Main Loop --------------------------------
    while(!WindowShouldClose()) {
        //- Start Drawing --------------------------------
        BeginDrawing();
        ClearBackground(WHITE);

        //- delta time (time since last frame) ------------------------------
        const float dT{GetFrameTime()};

        //- scroll background ------------------------------
        bgX -= 20 * dT;
        if(bgX <= -background.width*2) {
            bgX = 0.0;
        }

        //- scroll midground ------------------------------
        mgX -= 40 * dT;
        if(mgX <= -midground.width*2) {
            mgX = 0.0;
        }

        //- scroll foreground ------------------------------
        fgX -= 80 * dT;
        if(fgX <= -foreground.width*2) {
            fgX = 0.0;
        }

        //- draw the background --------------------------------
        Vector2 bg1Pos{bgX,0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        Vector2 bg2Pos{bgX + background.width*2, 0.0};
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        //- draw the midground ------------------------------
        Vector2 mg1Pos{mgX, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width*2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        //- draw the foreground ------------------------------
        Vector2 fg1Pos{fgX, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        //- perform ground check --------------------------------
        if(isOnGround(playerData, windowDimensions[1])) {
            //- Rectangle is on the ground --------------------------------
            velocity = 0;
            isInAir = false;
        } else {
            //- Rectangle is in the air --------------------------------
            velocity += gravity * dT;
            isInAir = true;
        }

        //- Jump check --------------------------------
         if(IsKeyPressed(KEY_SPACE) && !isInAir) {
                velocity += jumpVelocity;
            }

        for(int i = 0; i < sizeOfNebulae; i++) {
            //- Update the position of each nebula --------------------------------
            nebulae[i].pos.x += nebVel * dT;
        }

        //- Update finishLine ------------------------------
        finishLine += nebVel * dT;

        //- update player position --------------------------------
        playerData.pos.y += velocity * dT;

        //- Update animation frame --------------------------------
        if(!isInAir) {
            playerData = updateAnimData(playerData, dT, 5);
        }

        for(int i = 0; i < sizeOfNebulae; i++) {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        } 

        for(AnimData nebula : nebulae) {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };

            Rectangle playerRec{
                playerData.pos.x,
                playerData.pos.y,
                playerData.rec.width,
                playerData.rec.height
            };
            if(CheckCollisionRecs(nebRec,playerRec)) {
                collision = true;
            }
        }

        if(collision) {
            //- lose the game ------------------------------
            DrawText("GAME OVER!", windowDimensions[0]/4, windowDimensions[1]/2,32, WHITE);
        } 
        else if(playerData.pos.x >= finishLine) {
            //- Win the game ----------------
            DrawText("You Win!", windowDimensions[0]/4, windowDimensions[1]/2,32, WHITE);
        } 
        else {    
            for(int i = 0; i < sizeOfNebulae; i++) {
                //- draw obstacle ------------------------------------------------
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            //- draw player --------------------------------
            DrawTextureRec(player, playerData.rec, playerData.pos, WHITE);     
        }
        

        //- Stop Drawing --------------------------------
        EndDrawing();
    }

    UnloadTexture(player);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
}