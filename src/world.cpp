#include "world.hpp"
#include "randoms.hpp"


void PhysicalWorld::spawn_water(Vector2 const world_position)
{
    if (world_position.x < 0 || world_position.y < 0) return;
    size_t const x_idx {static_cast<size_t>(world_position.x) / ELEMENT_SIZE};
    size_t const y_idx {static_cast<size_t>(world_position.y) / ELEMENT_SIZE};
    if (x_idx > X_ELEMENT_COUNT || y_idx > Y_ELEMENT_COUNT) return;
    grid_[y_idx][x_idx].type = ElementType::Water;
}


void PhysicalWorld::update(float const delta)
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


void PhysicalWorld::render() const 
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


void PhysicalWorld::debug_grid() const
{
    for (size_t y_index{}; y_index < grid_.size() + 1; ++y_index)
    {
        auto const row_positon {y_index * ELEMENT_SIZE};
        DrawLine(0, row_positon, SCREEN_WIDTH, row_positon, DARKGRAY);
    }

    auto const& sample_row {grid_[0]};
    for (size_t x_index{}; x_index < sample_row.size() + 1; ++x_index)
    {
        auto const column_position {x_index * ELEMENT_SIZE};
        DrawLine(column_position, 0, column_position, SCREEN_HEIGHT, DARKGRAY);
    }
}


bool PhysicalWorld::move_down(size_t const xpos, size_t const ypos) 
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

bool PhysicalWorld::move_left(size_t const xpos, size_t const ypos) 
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

bool PhysicalWorld::move_right(size_t const xpos, size_t const ypos) 
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
