
#pragma once

#include PULSE2D_GRAPHICS

#include <etl/map.h>

#include <variant>

namespace asteriods {

using Size_Data = pulse2d_math::Vec2;

// Events

struct Spawn_Event
{
    Size_Data* size = nullptr;
};

struct Impact_Event
{
    Size_Data* size;
};

struct Crash_Event
{
    Size_Data* size;
};

// States

struct Spawned_State
{
    Size_Data* size;
};

struct Crashed_State
{
    Size_Data* size;
};

struct Erased_State
{
    Size_Data* size;
};

struct XL_State
{
    Size_Data* size;
};

struct L_State
{
    Size_Data* size;
};

struct M_State
{
    Size_Data* size;
};

struct S_State
{
    Size_Data* size;
};

using Event = std::variant<Spawn_Event, Impact_Event, Crash_Event>;

using State = std::variant<Spawned_State,
    Erased_State,
    Crashed_State,
    XL_State,
    L_State,
    M_State,
    S_State>;

#define DEFINE_EVENT_SET_STATE(from_state, event, to_state) \
    [](from_state&, event const& e) -> State {              \
        return to_state{ .size = e.size };                  \
    }

// State Manager

class Asteroid_SM
{
  public:
    Asteroid_SM() = default;
    void start(State const& state) { current_state = state; }
    void dispatch(Event const& event)
    {
        current_state = std::visit(
            pulse2d_util::overload{
                DEFINE_EVENT_SET_STATE(
                    Erased_State, Impact_Event, Erased_State),
                DEFINE_EVENT_SET_STATE(
                    Crashed_State, Impact_Event, Erased_State),
                DEFINE_EVENT_SET_STATE(XL_State, Impact_Event, L_State),
                DEFINE_EVENT_SET_STATE(L_State, Impact_Event, M_State),
                DEFINE_EVENT_SET_STATE(M_State, Impact_Event, S_State),
                DEFINE_EVENT_SET_STATE(S_State, Impact_Event, Erased_State),
                DEFINE_EVENT_SET_STATE(XL_State, Crash_Event, Crashed_State),
                DEFINE_EVENT_SET_STATE(L_State, Crash_Event, Crashed_State),
                DEFINE_EVENT_SET_STATE(M_State, Crash_Event, Crashed_State),
                DEFINE_EVENT_SET_STATE(S_State, Crash_Event, Crashed_State),

                [](const auto& s, const auto&) -> State {
                    return s; // ignore event, maintain state
                } },
            current_state,
            event);
    }

    State get_current_state() const { return current_state; }
    void reset() { current_state = Spawned_State{}; }

  private:
    State current_state = Spawned_State{};
};

struct Asteroid_Manager
{
    Asteroid_Manager() = default;
    etl::map<const char*, Asteroid_SM, 5, pulse2d::CStrLess> objects;
    void reset()
    {
        for (auto& [k, v] : objects)
            v.reset();
    }
};

} // namespace asteriods