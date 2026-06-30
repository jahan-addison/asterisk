
#pragma once

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <etl/array.h>

#include <variant>

namespace asteroid {

enum class Size : p_ui8
{
    XL = 0,
    L,
    M,
    S,
    E
};

PULSE2D_INLINE pulse2d_math::Vec2 get_size_by_pixel(Size size,
    float w,
    float h,
    p_ui8 factor = 20)
{
    if (size == Size::XL or size == Size::E)
        return { w, h };
    auto co_e = static_cast<float>(factor * static_cast<p_ui8>(size));
    if (w - co_e <= 0 or h - co_e <= 0)
        return { w, h };
    return { w - co_e, h - co_e };
}

// clang-format off
// Events

struct Spawn_Event
{
    Size size;
    p_ui8 total;
};
struct Impact_Event
{};
struct Crash_Event
{};

// States

struct Live_State
{
    p_ui8 hits{ 0 };
    Size current_size = Size::E;
    p_ui8 total_sizes{0};
    p_ui8 shrink{0};
};

struct Crashed_State
{};
struct Erased_State
{};
struct Spawned_State
{};

// Manager

using Event = std::variant<Spawn_Event,
    Impact_Event,
    Crash_Event>;

using State = std::variant<Spawned_State,
    Erased_State,
    Crashed_State,
    Live_State>;

// clang-format on

class Asteroid_SM
{
  public:
    Asteroid_SM() = default;
    void dispatch(Event const& event)
    {
        current_state = std::visit(
            pulse2d_util::overload{

                [](Spawned_State&, Spawn_Event const& e) -> State {
                    return Live_State{ .current_size = e.size,
                        .total_sizes = e.total };
                },

                [](Spawned_State&, Crash_Event const&) -> State {
                    return Crashed_State{};
                },

                [](Live_State&, Crash_Event const&) -> State {
                    return Crashed_State{};
                },

                [](Live_State& s, Impact_Event const&) -> State {
                    if (s.hits >= 2) {
                        if (s.total_sizes == 1)
                            return Erased_State{};
                        auto current = static_cast<int>(s.current_size);
                        if (current > 0 and current <= 4)
                            s.current_size = static_cast<Size>(current + 1);
                        else
                            PULSE2D_DEBUG_SERIAL(
                                "[asteroid]", "invalid hit count impact event");

                        s.hits = 0;
                        s.shrink++;
                    } else {
                        s.hits++;
                    }
                    if (s.current_size == Size::E or s.shrink >= s.total_sizes)
                        return Erased_State{};
                    return s;
                },

                [](const auto& s, const auto&) -> State {
                    return s; // ignore event, maintain state
                } },
            current_state,
            event);
    }

    PULSE2D_INLINE State get_current_state() const { return current_state; }
    PULSE2D_INLINE void start() { current_state = Spawned_State{}; }
    PULSE2D_INLINE void reset() { current_state = Spawned_State{}; }

  private:
    State current_state = Spawned_State{};
};

template<size_t N = 4>
struct Asteroid_Manager
{
    Asteroid_Manager() = default;
    etl::array<Asteroid_SM, N> objects;
    void reset_all()
    {
        for (auto& v : objects)
            v.reset();
    }
};

} // namespace asteriods