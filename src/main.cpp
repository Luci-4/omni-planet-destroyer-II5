#include <cstdint>
#include <raylib.h>
#include "constants.hpp"
#include "class_traits.hpp"
#include <string>
#include <memory>
#include <algorithm>
#include <math.h>
#include "world.hpp"

struct Player 
{
    Vector2 position {};
    static constexpr uint16_t SPEED {600};

    void update(float const delta);
    void render() const;
};


void Player::update(float const delta) 
{
    auto const diff {SPEED * delta};
    if (IsKeyDown(KEY_W)) position.y -= diff;
    if (IsKeyDown(KEY_S)) position.y += diff;
    if (IsKeyDown(KEY_A)) position.x -= diff;
    if (IsKeyDown(KEY_D)) position.x += diff;
}


void Player::render() const
{
    DrawRectangle(position.x, position.y, 10, 10, RED);
}

struct iVec2
{
    int32_t x;
    int32_t y;
};


struct PlayerSetState
{
    struct {
        bool debug_render: 1 {true};
        bool is_left_mouse_down: 1 {false};
    } flags;
    float wheel{0.f};
};

class Game : public NonMovableNonCopyable
{
public:
    Game()
    {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Are you sure?");
        SetTargetFPS(60);
    }

    void run()
    {
        camera_.target = player.position;
        camera_.offset = {SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f};
        camera_.rotation = 0.f;
        camera_.zoom = 1.f;

        while (!WindowShouldClose())
        {
            poll();
            update();
            render();
        }
    }

    ~Game()
    {
        CloseWindow();
    }

private:
    void poll()
    {
        player_state_.wheel = GetMouseWheelMove();
        player_state_.flags.is_left_mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        if (IsKeyPressed(KEY_R)) 
        {
            player_state_.flags.debug_render = !player_state_.flags.debug_render;
        }
    }


    void update()
    {
        float const delta {GetFrameTime()}; 

        if (player_state_.wheel != 0.f)
        {
            float scale = 0.2f * player_state_.wheel;
            camera_.zoom = std::clamp(expf(logf(camera_.zoom)+scale), 0.125f, 64.0f);
        }
        camera_.target = player.position;
        if (player_state_.flags.is_left_mouse_down)
        {
            auto const screen_position {GetMousePosition()};
            auto const mouse_world_position {GetScreenToWorld2D(screen_position, camera_)};
            if (player_state_.flags.debug_render)
            {
                std::string const pos_text {"Pos x: " + std::to_string(mouse_world_position.x) + " y: " + std::to_string(mouse_world_position.y)};
                DrawText(pos_text.c_str(), screen_position.x, screen_position.y, 20, RED);
            }
            world_->spawn_water(mouse_world_position);
        }
        world_->update(delta);
        player.update(delta);
    }
    
    void debug_render() const
    {
        world_->debug_grid();
    }

    void render() const
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        world_render();
        ui_render();
        EndDrawing();
    }

    void world_render() const
    {
        BeginMode2D(camera_);
        world_->render();
        player.render();
        if (player_state_.flags.debug_render)
        {
            debug_render();
        }
        DrawText("This is the 0,0 point", 0, 0, 10, RED);
        EndMode2D();
    }

    void ui_render() const 
    {
        DrawFPS(0, 0);
        std::string const player_pos_text {"Player position x: " + std::to_string(player.position.x) + " y: " + std::to_string(player.position.y)};
        DrawText(player_pos_text.c_str(), 0, 21, 20, RED);
    }
 
    Player player;
    PlayerSetState player_state_;
    std::unique_ptr<PhysicalWorld> world_{std::make_unique<PhysicalWorld>()};
    Camera2D camera_{};
};


int main()
{
    Game game;
    game.run();
}

