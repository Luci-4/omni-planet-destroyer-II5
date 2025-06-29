#pragma once
#include "constants.hpp"
#include <raylib.h>
#include <array>

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
    static constexpr uint32_t ELEMENT_SIZE {6};

    // Those values are temporal and the limited board works well in a testing stage
    static constexpr uint32_t X_ELEMENT_COUNT {SCREEN_WIDTH / ELEMENT_SIZE};
    static constexpr uint32_t Y_ELEMENT_COUNT {SCREEN_HEIGHT / ELEMENT_SIZE};

    using Storage = std::array<std::array<AtomicElement, X_ELEMENT_COUNT>, Y_ELEMENT_COUNT>;
public:
    void spawn_water(Vector2 const world_position);
    void update(float const delta);

    void render() const;
    void debug_grid() const;

private:
    bool move_down(size_t const xpos, size_t const ypos);
    bool move_left(size_t const xpos, size_t const ypos);
    bool move_right(size_t const xpos, size_t const ypos);
    
    float last_update_{};
    Storage grid_;
};
