#pragma once

/****************************************************************************
 * Asteroid state machine (boost/sml)
 *
 * The SM is self-contained: it owns both state logic and rendering.
 * The game loop dispatches two event types and nothing else:
 *
 *   Render_Event  - every frame, SM draws the correct sprite for the
 *                   current state and size via the injected draw callback
 *   Impact_Event  - on laser collision, SM advances hit/shrink counters
 *                   and transitions to Erased when the asteroid is exhausted
 *
 *   Spawn_Event   - dispatched once by add() and again by reset();
 *                   on_spawn positions the body and enters Live
 *   Crash_Event   - transitions unconditionally to Crashed
 *
 * Asteroid_Config is set per-instance at add() time and injected into all
 * SM actions via sml dependency injection. Draw calls go through a
 * non-capturing function pointer so the game's runtime stays decoupled.
 *
 *
 *   lvl_asteroids_.add({
 *       .body      = &my_game.get_body("asteroid_1"),
 *       .sprite_m  = "asteroid_1m_sprite",
 *       .sprite_s  = "asteroid_1s_sprite",
 *       .width_m   = px_to_units(65.0f, 65.0f),
 *       .width_s   = asteroid::get_size_by_pixel(
 *          asteroid::Size::S, 65.0f, 65.0f),
 *       .draw      = [](pulse2d_body* b, const char* s) {
 *          my_game.draw_body(b, s); },
 *       .spawn_size  = asteroid::Size::M,
 *       .spawn_total = 2,
 *   });
 *
 *
 *   // per frame:
 *   lvl_asteroids_.objects.at(current).dispatch(asteroid::Render_Event{});
 *   if (lvl_asteroids_.objects.at(current).is<asteroid::Erased>()) { ... }
 *
 ****************************************************************************/

#include PULSE2D_HEADER
#include PULSE2D_GRAPHICS

#include <etl/array.h>

namespace asteroid {

///////////////////////////////////////////////////////////////////////////////
// Size

enum class Size : p_ui8
{
    XL = 0,
    L,
    M,
    S,
    E // exhausted
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

///////////////////////////////////////////////////////////////////////////////
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
struct Evaded_Event
{};
struct Render_Event
{};

///////////////////////////////////////////////////////////////////////////////
// State tags

struct Spawned
{};
struct Live
{};
struct Erased
{};
struct Crashed
{};
struct Evaded
{};

///////////////////////////////////////////////////////////////////////////////
// Live_Data

struct Live_Data
{
    p_ui8 hits = 0;
    Size current_size = Size::E;
    p_ui8 total_sizes = 0;
    p_ui8 shrink = 0;
};

///////////////////////////////////////////////////////////////////////////////
// Asteroid_Config
//
// pulse2d::state::Draw_Fn is a non-capturing function pointer, lambdas that
// reference only translation unit globals (e.g. the game runtime) are
// convertible to it.

struct Asteroid_Config
{
    pulse2d_body* body = nullptr;
    const char* sprite_l = nullptr;         // large sprite
    const char* sprite_m = nullptr;         // medium sprite
    const char* sprite_s = nullptr;         // small sprite
    pulse2d_math::Vec2 width_l = {};        // physics width for L
    pulse2d_math::Vec2 width_m = {};        // physics width for M
    pulse2d_math::Vec2 width_s = {};        // physics width for S
    pulse2d_math::Vec2 spawn_velocity = {}; // restored on every reset
    pulse2d::state::Draw_Fn draw = nullptr;
    Size spawn_size = Size::L;
    p_ui8 spawn_total = 1;
};

///////////////////////////////////////////////////////////////////////////////
// Transition table

struct asteroid_sm
{
    auto operator()() const
    {
        using namespace sml;

        auto will_erase = [](Live_Data const& d) -> bool {
            if (d.hits < 2)
                return false;
            auto c = static_cast<int>(d.current_size);
            Size next =
                (c > 0 and c <= 4) ? static_cast<Size>(c + 1) : d.current_size;
            return next == Size::E or (d.shrink + 1) >= d.total_sizes;
        };

        auto on_spawn = [](Spawn_Event const& e,
                            Live_Data& d,
                            Asteroid_Config& cfg) {
            d = { .current_size = e.size, .total_sizes = e.total };
            auto w = (e.size == Size::L) ? cfg.width_l : cfg.width_m;
            cfg.body->set_width({ w.x, w.y })
                .set_velocity({ cfg.spawn_velocity.x, cfg.spawn_velocity.y })
                .set_position({ 6.0f, pulse2d_math::trng_random(-2.5f, 2.5f) });
        };

        auto on_impact = [](Live_Data& d) {
            if (d.hits >= 2) {
                auto c = static_cast<int>(d.current_size);
                if (c > 0 and c <= 4)
                    d.current_size = static_cast<Size>(c + 1);
                d.hits = 0;
                d.shrink++;
            } else {
                d.hits++;
            }
        };

        auto will_evade = [](Live_Data const&, Asteroid_Config const& cfg) {
            return cfg.body->position.x < -5.0f;
        };

        auto on_render = [](Live_Data const& d, Asteroid_Config const& cfg) {
            if (d.current_size == Size::S) {
                cfg.body->set_width({ cfg.width_s.x, cfg.width_s.y });
                cfg.draw(cfg.body, cfg.sprite_s);
            } else if (d.current_size == Size::M) {
                cfg.body->set_width({ cfg.width_m.x, cfg.width_m.y });
                cfg.draw(cfg.body, cfg.sprite_m);
            } else {
                cfg.body->set_width({ cfg.width_l.x, cfg.width_l.y });
                cfg.draw(cfg.body, cfg.sprite_l);
            }
        };

        // clang-format off
        return make_transition_table(
            *state<Spawned> + event<Spawn_Event>                / on_spawn    = state<Live>,
             state<Spawned> + event<Crash_Event>                              = state<Crashed>,
             state<Live>    + event<Render_Event>               / on_render,
             state<Live>    + event<Impact_Event> [ will_erase] / on_impact   = state<Erased>,
             state<Live>    + event<Impact_Event> [!will_erase] / on_impact,
             state<Live>    + event<Crash_Event>                              = state<Crashed>,
             state<Live>    + event<Evaded_Event> [ will_evade]               = state<Evaded>,
             state<Erased>  + on_entry<_> / [](Asteroid_Config const& cfg) {
                 cfg.body->set_width({ 0.0f, 0.0f });
             }
        );
        // clang-format on
    }
};

///////////////////////////////////////////////////////////////////////////////
// Asteroid_Controller

using Asteroid_Controller =
    pulse2d::state::Entity_Controller<asteroid_sm, Live_Data, Asteroid_Config>;

///////////////////////////////////////////////////////////////////////////////
// Asteroid_Manager

template<size_t N = 4>
class Asteroid_Manager
{
  public:
    Asteroid_Manager() = default;

    // Configure only - does not spawn. Call activate(0) after all add() calls.
    PULSE2D_INLINE Asteroid_Manager& add(Asteroid_Config const& cfg)
    {
        objects.at(index).configure(cfg);
        index++;
        return *this;
    }

    // Dispatch Spawn_Event for asteroid i (initial activation).
    PULSE2D_INLINE void activate(size_t i)
    {
        auto const& cfg = objects.at(i).config();
        objects.at(i).dispatch(Spawn_Event{ cfg.spawn_size, cfg.spawn_total });
    }

    // Reset SM to Spawned then re-activate (mid-game advancement).
    PULSE2D_INLINE void respawn(size_t i)
    {
        objects.at(i).reset();
        activate(i);
    }

    PULSE2D_INLINE Asteroid_Controller& get(size_t i) { return objects.at(i); }

    // Pick the next asteroid index, excluding current. Drains a Fisher-Yates
    // shuffled pool of the remaining N-1 indices; refills when exhausted.
    PULSE2D_INLINE size_t pick_next(size_t current)
    {
        if (pool_head_ == 0) {
            size_t j = 0;
            for (size_t i = 0; i < N; ++i)
                if (i != current)
                    pool_[j++] = i;
            for (size_t i = N - 2; i > 0; --i) {
                auto k = static_cast<size_t>(
                    pulse2d_math::trng_random(0.0f, static_cast<float>(i)));
                size_t tmp = pool_[i];
                pool_[i] = pool_[k];
                pool_[k] = tmp;
            }
            pool_head_ = N - 1;
        }
        return pool_[--pool_head_];
    }

    // Reset all SMs to Spawned. Resets index so scene_enter can re-add on
    // restart.
    PULSE2D_INLINE void reset_all()
    {
        for (auto& obj : objects)
            obj.reset();
        index = 0;
        pool_head_ = 0;
    }

  public:
    etl::array<Asteroid_Controller, N> objects;

  private:
    size_t index{ 0 };
    etl::array<size_t, N - 1> pool_{};
    size_t pool_head_{ 0 };
};

} // namespace asteroid
