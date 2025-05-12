#include <stdio.h>
#include <vector>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

/* TODO:
 * - add ants and input to a separate thread
 * - catch ants if user tries to spawn then outisde the box
 * - add controls to the box
 * - add hud elements (current ants, updates per second, fps, etc)
 * - add ways to remove ants and cells
 * - make box's border large if user unzooms a lot (raylib does not render it very well)
 */

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class ant{
public:
    int x = SCREEN_WIDTH / 2;
    int y = SCREEN_HEIGHT / 2;
    int rot = 1;

    ant(int x, int y, int rot) : x(x), y(y), rot(rot) {}
    void antTrail(std::vector<std::vector<bool>> &grid, RenderTexture2D &canvas);
};

void ant::antTrail(std::vector<std::vector<bool>> &grid, RenderTexture2D &canvas)
{
    if (grid[y][x]){rot--;}
    else {rot++; }

    grid[y][x] = !grid[y][x];

    BeginTextureMode(canvas);
        DrawPixel(x, y, grid[y][x] ? WHITE : BLACK);
    EndTextureMode();

    if (rot > 4) { rot = 1;}
    if (rot < 1) { rot = 4;}

    switch(rot)
    {
    case 1:
        y++;
        break;
    case 2:
        x++;
        break;
    case 3:
        y--;
        break;
    case 4:
        x--;
        break;
    }

    BeginTextureMode(canvas);
            DrawPixel(x,y,RED);
    EndTextureMode();

    x = (x + grid[0].size()) % grid[0].size();
    y = (y + grid.size()) % grid.size();
}

void antCreate(std::vector<ant> &ants, Camera2D cam, std::vector<std::vector<bool>> grid)
{
    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);

    if (mouseWorldPos.x >= 0 && mouseWorldPos.x < 0 + grid.size() && mouseWorldPos.y >= 0 && grid[0].size())
    {
        ants.push_back(ant((int)mouseWorldPos.x, (int)mouseWorldPos.y, rand() % 4 + 1));
    }
}

void cameraMove(Camera2D &cam)
{
    float wheel = GetMouseWheelMove();

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);//get the positon of the mouse relative to the world

    if (wheel != 0) //if mouse wheel is moving
    {
        cam.offset = GetMousePosition(); //offset based on where the mouse is

        cam.target = mouseWorldPos; //aim camera at world position of the mose

        float scale = 0.2f*wheel; // zoom
        cam.zoom = Clamp(expf(logf(cam.zoom)+scale), 0.125f, 64.0f); //i have no idea what this does
    }

    float camSped = 10.f / cam.zoom;

    if(IsKeyDown(KEY_LEFT_SHIFT))
    {
        camSped = 20.f / cam.zoom;
    }

    if(IsKeyDown(KEY_W))
    {
        cam.target.y -= camSped;
    }
    if(IsKeyDown(KEY_S))
    {
        cam.target.y += camSped;
    }
    if(IsKeyDown(KEY_D))
    {
        cam.target.x += camSped;
    }
    if(IsKeyDown(KEY_A))
    {
        cam.target.x -= camSped;
    }
}

void controls(float &fixedDT, float &updateSpeed)
{
    if(IsKeyPressed(KEY_UP))
    {
        if(IsKeyPressed(KEY_LEFT_SHIFT))
        {
            updateSpeed *= 8;
        }
        else
        {
            updateSpeed *= 3;
        }
        printf("updats per second: %f\n", updateSpeed);
    }

    if(IsKeyPressed(KEY_DOWN))
    {
        if(IsKeyPressed(KEY_LEFT_SHIFT))
        {
            updateSpeed *= 0.5;
        }
        else
        {
            updateSpeed *= 0.1;
        }
        printf("updats per second: %f\n", updateSpeed);
    }

    if (updateSpeed >= 6000) {updateSpeed = 6000;}
    if (updateSpeed <= 1) {updateSpeed = 1;}
    fixedDT = 1.f / updateSpeed;
}

int main(int argc, char *argv[])
{
    int updatesPerSec = 25;
    int gridSizeY = 500;
    int gridSizeX = 500;

    float updateSpeed = 60;

    float acom = 0.f;
    float fixedDT = 1.f / updateSpeed;

    std::vector<std::vector<bool>> grid(gridSizeY, std::vector<bool>(gridSizeX, false));

    std::vector<ant> ants;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "antTest");

    RenderTexture2D canvas = LoadRenderTexture(gridSizeX,gridSizeY);

    BeginTextureMode(canvas);
    ClearBackground(BLACK);
    EndTextureMode();

    Camera2D camera = { 0 };
    camera.zoom = 12.0f;
    camera.target = {gridSizeX / 2.0f, gridSizeY / 2.0f};
    camera.offset = {gridSizeX / 2.0f, gridSizeY / 2.0f};

    SetTargetFPS(60);

    HideCursor();

    while(!WindowShouldClose())
    {

        ClearBackground(BLACK);
        BeginDrawing();
        BeginMode2D(camera);

        //drawing stuff
        DrawRectangle(-3,-3,canvas.texture.width+6,canvas.texture.height+6, RED);
        DrawTextureRec(canvas.texture, {0,0, (float)canvas.texture.width, -(float)canvas.texture.height}, {0,0}, WHITE);


        //input stuff
        cameraMove(camera);

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            antCreate(ants,camera, grid);
        }

        controls(fixedDT, updateSpeed);

        //physics stuff

        float frameTime = GetFrameTime(); // seconds
        acom += frameTime;

        while (acom >= fixedDT)
        {
            for (int i = 0; i < ants.size(); i++)
            {
                ants[i].antTrail(grid,canvas);
            }
            acom -= fixedDT;

        }

        EndMode2D();

        DrawRectangle(GetMouseX(),GetMouseY(), 2 , 2 , RED);

        EndDrawing();
    }

    CloseWindow();
}
