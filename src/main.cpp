#include <cstdint>
#include <random>
#include <raylib.h>
#include <iostream>
#include <array>
#include "class_traits.hpp"
#include <string>
#include <memory>

class Randoms : public NonMovableNonCopyable
{
public:
    static bool choice()
    {
        static std::bernoulli_distribution dist(0.5f);
        return dist(rng) == 0;
    }

private:
    inline static std::mt19937 rng{std::random_device{}()};
};



struct Player 
{
    Vector2 position {200, 400};
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

constexpr uint16_t SCREEN_WIDTH {800};
constexpr uint16_t SCREEN_HEIGHT {600};


enum class ElementType : uint8_t
{
    None,
    Static,
    Water
};


struct AtomicElement
{
    ElementType type{ElementType::None};
    Color color() const 
    {
        switch (type)    
        {
            case ElementType::None:
                return BLANK;
            case ElementType::Static:
                return LIGHTGRAY;
            case ElementType::Water:
                return BLUE;
        }
        return BLANK;
    }
};


class PhysicalWorld
{
    static constexpr uint32_t ELEMENT_SIZE {4};
    static constexpr uint32_t X_ELEMENT_COUNT {SCREEN_WIDTH / ELEMENT_SIZE};
    static constexpr uint32_t Y_ELEMENT_COUNT {SCREEN_HEIGHT / ELEMENT_SIZE};

    using Storage = std::array<std::array<AtomicElement, X_ELEMENT_COUNT>, Y_ELEMENT_COUNT>;
public:

    PhysicalWorld()
    {
        grid_[0][0].type = ElementType::Water;
    }

    void spawn_water(Vector2 const world_position)
    {
        if (world_position.x < 0 || world_position.y < 0) return;
        size_t const x_idx {static_cast<size_t>(world_position.x) / ELEMENT_SIZE};
        size_t const y_idx {static_cast<size_t>(world_position.y) / ELEMENT_SIZE};
        if (x_idx > X_ELEMENT_COUNT || y_idx > Y_ELEMENT_COUNT) return;
        grid_[y_idx][x_idx].type = ElementType::Water;
    }

    void update(float const delta)
    {
        constexpr float delay {0.01f};
        last_update_ += delta;
        if (last_update_ < delay)
        {
            last_update_ += delta;
            return;
        }

        for (size_t row_idx {}; row_idx < grid_.size(); ++row_idx)
        {
            for (size_t column_idx{}, row_size{grid_[row_idx].size()}; column_idx < row_size; ++column_idx)
            {
                auto& element {grid_[row_idx][column_idx]};
                if (element.type != ElementType::Water) continue;
                if (move_down(column_idx, row_idx)) continue;
                Randoms::choice() ? move_left(column_idx, row_idx) : move_right(column_idx, row_idx);
            }
        }
        last_update_ = 0.f;
    }

    void render() const 
    {
        size_t row_index {};
        for (auto const& row : grid_)
        {
            size_t element_index {};
            for (auto const& element: row) 
            {
                size_t const x_position {element_index * ELEMENT_SIZE};
                size_t const y_position {row_index * ELEMENT_SIZE};
                DrawRectangle(x_position, y_position, ELEMENT_SIZE, ELEMENT_SIZE, element.color());
                ++element_index;
            }
            ++row_index;
        }
    }

private:
    bool move_down(size_t const xpos, size_t const ypos) 
    {
        if (ypos + 1 > grid_.size()) return false;
        if (grid_[ypos + 1][xpos].type == ElementType::None)
        {
            grid_[ypos][xpos].type = ElementType::None;
            grid_[ypos + 1][xpos].type = ElementType::Water;
            return true;
        }
        return false;
    }

    bool move_left(size_t const xpos, size_t const ypos) 
    {
        auto& row {grid_[ypos]};
        if (static_cast<int32_t>(xpos) - 1 < 0) return false;
        if (row[xpos - 1].type == ElementType::None)
        {
            grid_[ypos][xpos].type = ElementType::None;
            grid_[ypos][xpos - 1].type = ElementType::Water;
            return true;
        }
        return false;
    }

    bool move_right(size_t const xpos, size_t const ypos) 
    {
        auto& row {grid_[ypos]};
        if (xpos + 1 > row.size()) return false;
        if (row[xpos + 1].type == ElementType::None)
        {
            grid_[ypos][xpos].type = ElementType::None;
            grid_[ypos][xpos + 1].type = ElementType::Water;
            return true;
        }
        return false;
    }
    
    float last_update_{};
    Storage grid_;
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
            update();
            render();
        }
    }

    ~Game()
    {
        CloseWindow();
    }

private:
    void update()
    {
        float const delta {GetFrameTime()}; 

        camera_.target = player.position;
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            auto const screen_position {GetMousePosition()};
            auto const mouse_world_position {GetScreenToWorld2D(screen_position, camera_)};
            std::string const pos_text {"Pos x: " + std::to_string(mouse_world_position.x) + " y: " + std::to_string(mouse_world_position.y)};
            DrawText(pos_text.c_str(), screen_position.x, screen_position.y, 20, RED);
            world_->spawn_water(mouse_world_position);
        }
        world_->update(delta);
        player.update(delta);
    }


    void render() const
    {
        BeginDrawing();
        ClearBackground(BLACK);
        world_render();
        ui_render();
        EndDrawing();
    }

    void world_render() const
    {
        BeginMode2D(camera_);
        world_->render();
        player.render();
        EndMode2D();
    }

    void ui_render() const 
    {
        DrawFPS(0, 0);
        std::string const player_pos_text {"Player position x: " + std::to_string(player.position.x) + " y: " + std::to_string(player.position.y)};
        DrawText(player_pos_text.c_str(), 0, 21, 20, RED);
    }
     
    Player player;
    std::unique_ptr<PhysicalWorld> world_{std::make_unique<PhysicalWorld>()};
    Camera2D camera_{};
};


int main()
{
    Game game;
    game.run();
}

