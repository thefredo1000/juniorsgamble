#include "world_map_screen.h"

#include "bn_algorithm.h"
#include "bn_camera_ptr.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_point.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "bn_regular_bg_items_dream.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_items_junior.h"

#include "common_variable_8x16_sprite_font.h"
#include "dialogue_box.h"
#include "game_input.h"
#include "minigame.h"
#include "money.h"
#include "text_box.h"
#include "text_format.h"
#include "world_map_config.h"
#include "world_map_dialog.h"
#include "world_map_interaction.h"
#include "world_map_logic.h"
#include "world_map_movement.h"
#include "world_map_npc_visual.h"
#include "world_map_player_visual.h"
#include "world_map_state.h"

namespace
{
    using Game::world_map_logic::direction;
    using Game::world_map_logic::solid_rect;
    using Game::world_map_state::runtime_state;

    // Add world-space solid areas here to block movement. There is deliberately
    // no placeholder entry: point_inside_rect is inclusive, so a { 0, 0, 0, 0 }
    // rect would make world tile (0, 0) solid, which is the player start tile.
    constexpr const solid_rect* solid_rects = nullptr;
    constexpr int solid_rect_count = 0;

    bool try_move_direction(runtime_state& state,
                            direction input_direction,
                            bool held,
                            int delta_x,
                            int delta_y,
                            int world_x_limit,
                            int world_y_limit,
                            const Game::world_map_config::npc_definition* npc_definitions,
                            int npc_count,
                            const solid_rect* local_solid_rects,
                            int local_solid_rect_count,
                            bn::sprite_ptr& junior_sprite,
                            Game::world_map_player_visual::walk_animate_action& junior_animate_action)
    {
        const Game::world_map_movement::direction_input_result input_result =
                Game::world_map_movement::process_direction_input(state, input_direction, held);

        if(input_result == Game::world_map_movement::direction_input_result::no_input)
        {
            return false;
        }

        Game::world_map_player_visual::update_walk_animation(state, input_direction,
                                                              junior_sprite, junior_animate_action);

        if(input_result == Game::world_map_movement::direction_input_result::turned)
        {
            Game::world_map_player_visual::set_standing_frame(junior_sprite, input_direction);
            state.player_walking = false;
            return true;
        }

        Game::world_map_movement::try_start_move(state, delta_x, delta_y,
                                                 world_x_limit, world_y_limit,
                                                 npc_definitions, npc_count,
                                                 local_solid_rects, local_solid_rect_count);

        // A refused step (map edge, NPC or solid tile) never advances the walk
        // cycle, so the sprite would freeze on whatever mid-stride frame the
        // animation left behind. Drop back to the standing frame instead.
        if(! state.remaining_move_x && ! state.remaining_move_y)
        {
            Game::world_map_player_visual::stop_walking(state, junior_sprite, input_direction);
        }

        return true;
    }

    // The camera follows the player, but stops once the map edge reaches the
    // edge of the screen. Inside that last screen-worth of map the view is
    // frozen and the player sprite walks across it instead.
    void follow_player(bn::camera_ptr& camera,
                       const runtime_state& state,
                       int camera_x_limit,
                       int camera_y_limit)
    {
        camera.set_x(bn::clamp(state.world_x, -camera_x_limit, camera_x_limit));
        camera.set_y(bn::clamp(state.world_y, -camera_y_limit, camera_y_limit));
    }
}

namespace Game
{
    world_map_result world_map_screen()
    {
        using world_map_config::edge_overshoot_steps_x;
        using world_map_config::edge_overshoot_steps_y;
        using world_map_config::map_border_x;
        using world_map_config::map_border_y;
        using world_map_config::move_speed;
        using world_map_config::npc_count;
        using world_map_config::npc_definition;
        using world_map_config::npc_definitions;
        using world_map_config::start_world_x;
        using world_map_config::start_world_y;
        using world_map_config::step_cooldown_frames;
        using world_map_config::tile_step;
        using world_map_state::runtime_state;

        // runtime_state starts out facing down; the sprite and its walk cycle
        // have to agree with that.
        constexpr direction state_facing_default = direction::down;

        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
        text_generator.set_center_alignment();

        bn::vector<bn::sprite_ptr, 64> hud_sprites;
        TextBox hud_text_box(text_generator, hud_sprites);
        hud_text_box.set_alignment(TextBox::alignment_type::CENTER)
                .line(0, -68, "World map");

        hud_text_box.set_alignment(TextBox::alignment_type::LEFT)
                .line(-112, -68, text::format<16>("Money: ${}", load_money()));

        bn::regular_bg_ptr map_bg = bn::regular_bg_items::dream.create_bg(0, 0);
        bn::camera_ptr camera = bn::camera_ptr::create(start_world_x, start_world_y);
        map_bg.set_camera(camera);

        // How far the camera can travel before the map runs out of background
        // to show. bn::max keeps the clamp valid for maps smaller than the
        // screen, where the view simply never moves.
        const int camera_x_limit = bn::max((map_bg.dimensions().width() - bn::display::width()) / 2, 0);
        const int camera_y_limit = bn::max((map_bg.dimensions().height() - bn::display::height()) / 2, 0);

        bn::sprite_ptr junior_sprite = bn::sprite_items::junior.create_sprite(
                0, 0, world_map_logic::direction_standing_frame(state_facing_default));
        junior_sprite.set_camera(camera);

        // How far the player can walk: up to the painted edge of the map, plus
        // the overshoot into its dithered border. That reaches well past where
        // the camera stops, so the last stretch is walked with the view frozen.
        const int world_x_limit = (map_bg.dimensions().width() - junior_sprite.dimensions().width()) / 2 -
                map_border_x + (edge_overshoot_steps_x * tile_step);
        const int world_y_limit = (map_bg.dimensions().height() - junior_sprite.dimensions().height()) / 2 -
                map_border_y + (edge_overshoot_steps_y * tile_step);

        world_map_player_visual::walk_animate_action junior_animate_action =
                world_map_player_visual::create_walk_animation(junior_sprite, state_facing_default);

        bn::vector<bn::sprite_ptr, world_map_config::npc_count> npc_sprites;

        for(int npc_index = 0; npc_index < npc_count; ++npc_index)
        {
            const npc_definition& npc = npc_definitions[npc_index];
            npc_sprites.push_back(npc.sprite_item->create_sprite(0, 0, npc.standing_frame));
        }

        runtime_state state;
        state.world_x = start_world_x;
        state.world_y = start_world_y;
        state.target_world_x = start_world_x;
        state.target_world_y = start_world_y;

        follow_player(camera, state, camera_x_limit, camera_y_limit);
        junior_sprite.set_position(state.world_x, state.world_y);

        bn::vector<bn::sprite_ptr, 96> dialog_sprites;
        DialogueBox dialogue_box(text_generator, dialog_sprites);

        auto start_npc_dialog = [&](int npc_index)
        {
            const int npc_standing_frame = world_map_interaction::npc_dialog_facing_frame(state.facing_direction);
            npc_sprites[npc_index].set_tiles(
                    npc_definitions[npc_index].sprite_item->tiles_item().create_tiles(npc_standing_frame));

            world_map_interaction::begin_dialog_state(state, npc_index);
            world_map_dialog::begin_dialog(dialogue_box, npc_definitions, npc_index);
        };

        auto end_npc_dialog = [&](int npc_index)
        {
            const npc_definition& npc = npc_definitions[npc_index];
            npc_sprites[npc_index].set_tiles(npc.sprite_item->tiles_item().create_tiles(npc.standing_frame));
        };

        while(true)
        {
            // close_dialog() clears active_npc_index, so remember who was talking
            // before the update to turn them back to their standing frame after.
            const int talking_npc_index = state.active_npc_index;

            const world_map_dialog::dialog_update_result dialog_result =
                    world_map_dialog::update_dialog(dialogue_box, state, npc_definitions);

            if(talking_npc_index >= 0 && state.active_npc_index < 0)
            {
                end_npc_dialog(talking_npc_index);
            }
            if(dialog_result.status == world_map_dialog::dialog_status::start_minigame)
            {
                if(load_money() > 0)
                {
                    return { world_map_outcome::start_minigame, dialog_result.minigame };
                }

                world_map_dialog::close_dialog(dialogue_box, state);
                dialogue_box.open_notice("You're out of money!", "Come back after a break.");

                while(! input::confirm_pressed() && ! input::back_pressed())
                {
                    bn::core::update();
                }

                dialogue_box.close();
                continue;
            }

            if(dialog_result.status == world_map_dialog::dialog_status::skip_level)
            {
                return { world_map_outcome::skip_level };
            }

            if(dialog_result.status == world_map_dialog::dialog_status::continue_loop)
            {
                continue;
            }

            world_map_movement::tick_cooldown(state);

            if(! state.step_cooldown && ! (state.remaining_move_x || state.remaining_move_y))
            {
                try_move_direction(state, direction::left, bn::keypad::left_held(), -tile_step, 0,
                                   world_x_limit, world_y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   junior_sprite, junior_animate_action) ||
                try_move_direction(state, direction::right, bn::keypad::right_held(), tile_step, 0,
                                   world_x_limit, world_y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   junior_sprite, junior_animate_action) ||
                try_move_direction(state, direction::up, bn::keypad::up_held(), 0, -tile_step,
                                   world_x_limit, world_y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   junior_sprite, junior_animate_action) ||
                try_move_direction(state, direction::down, bn::keypad::down_held(), 0, tile_step,
                                   world_x_limit, world_y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   junior_sprite, junior_animate_action);
            }

            const bool moved_x =
                    world_map_movement::update_axis_movement(state, true, move_speed, step_cooldown_frames);
            const bool moved_y =
                    world_map_movement::update_axis_movement(state, false, move_speed, step_cooldown_frames);

            follow_player(camera, state, camera_x_limit, camera_y_limit);
            junior_sprite.set_position(state.world_x, state.world_y);

            world_map_npc_visual::sync_npc_sprites(npc_definitions, npc_count, camera, npc_sprites);

            if(moved_x || moved_y)
            {
                junior_animate_action.update();
                state.player_walking = true;
            }
            else if(! (bn::keypad::left_held() || bn::keypad::right_held() ||
                       bn::keypad::up_held() || bn::keypad::down_held()))
            {
                // Released the d-pad: the walk cycle stops advancing, so put him
                // back on the standing frame. Frames between two held steps keep
                // the walk pose, otherwise walking would stutter every tile.
                world_map_player_visual::stop_walking(state, junior_sprite, state.facing_direction);
            }

            if(! bn::keypad::a_held() && ! bn::keypad::start_held())
            {
                state.can_interact = true;
            }

            if(state.can_interact && ! (state.remaining_move_x || state.remaining_move_y) && bn::keypad::a_pressed())
            {
                const int npc_index = world_map_interaction::facing_npc_index(
                    state, npc_definitions, npc_count, tile_step);

                if(npc_index >= 0)
                {
                    start_npc_dialog(npc_index);
                    bn::core::update();
                    continue;
                }
            }

            if(input::back_pressed())
            {
                return {};
            }

            bn::core::update();
        }
    }
}
