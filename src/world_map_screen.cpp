#include "world_map_screen.h"

#include "bn_affine_bg_ptr.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_point.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "bn_affine_bg_items_land.h"
#include "bn_sprite_items_junior.h"

#include "common_variable_8x16_sprite_font.h"
#include "dialogue_box.h"
#include "game_input.h"
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

    // Add world-space solid areas here to block movement.
    constexpr int solid_rect_count = 0;
    constexpr solid_rect solid_rects[] = {
        { 0, 0, 0, 0 }
    };

    bool try_move_direction(runtime_state& state,
                            direction input_direction,
                            bool held,
                            int delta_x,
                            int delta_y,
                            int x_limit,
                            int y_limit,
                            const Game::world_map_config::npc_definition* npc_definitions,
                            int npc_count,
                            const solid_rect* local_solid_rects,
                            int local_solid_rect_count,
                            int current_x,
                            int current_y,
                            bn::sprite_ptr& junior_sprite,
                            bn::sprite_animate_action<4>& junior_animate_action)
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
            return true;
        }

        Game::world_map_movement::try_start_move(state, delta_x, delta_y,
                                                 x_limit, y_limit,
                                                 npc_definitions, npc_count,
                                                 local_solid_rects, local_solid_rect_count,
                                                 current_x, current_y);
        return true;
    }

    void apply_axis_motion(bn::affine_bg_ptr& land_bg,
                           runtime_state& state,
                           const Game::world_map_movement::axis_motion_result& axis_motion,
                           bool is_x_axis)
    {
        if(! axis_motion.has_step)
        {
            return;
        }

        if(is_x_axis)
        {
            land_bg.set_pivot_x(land_bg.pivot_x() + axis_motion.signed_step);
            if(axis_motion.reached_target)
            {
                land_bg.set_pivot_x(state.target_pivot_x);
            }
            return;
        }

        land_bg.set_pivot_y(land_bg.pivot_y() + axis_motion.signed_step);
        if(axis_motion.reached_target)
        {
            land_bg.set_pivot_y(state.target_pivot_y);
        }
    }
}

namespace Game
{
    bool world_map_screen()
    {
        constexpr int host_npc_index = 1;

        using world_map_config::move_speed;
        using world_map_config::npc_count;
        using world_map_config::npc_definition;
        using world_map_config::npc_definitions;
        using world_map_config::start_world_x;
        using world_map_config::start_world_y;
        using world_map_config::step_cooldown_frames;
        using world_map_config::tile_step;
        using world_map_state::runtime_state;

        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
        text_generator.set_center_alignment();

        bn::vector<bn::sprite_ptr, 64> hud_sprites;
        TextBox hud_text_box(text_generator, hud_sprites);
        hud_text_box.set_alignment(TextBox::alignment_type::CENTER)
                .line(0, -68, "World map");

        hud_text_box.set_alignment(TextBox::alignment_type::LEFT)
                .line(-112, -68, text::format<16>("Money: ${}", load_money()));

        bn::affine_bg_ptr land_bg = bn::affine_bg_items::land.create_bg(0, 0);
        land_bg.set_pivot_position(start_world_x, start_world_y);

        const int x_limit = (land_bg.dimensions().width() - bn::display::width()) / 2;
        const int y_limit = (land_bg.dimensions().height() - bn::display::height()) / 2;

        bn::sprite_ptr junior_sprite = bn::sprite_items::junior.create_sprite(0, 0);
        bn::sprite_animate_action<4> junior_animate_action = bn::create_sprite_animate_action_forever(
                junior_sprite, 4, bn::sprite_items::junior.tiles_item(), 0, 1, 2, 3);

        bn::vector<bn::sprite_ptr, 8> npc_sprites;

        for(int npc_index = 0; npc_index < npc_count; ++npc_index)
        {
            const npc_definition& npc = npc_definitions[npc_index];
            bn::sprite_ptr npc_sprite = bn::sprite_items::junior.create_sprite(0, 0);
            npc_sprite.set_tiles(bn::sprite_items::junior.tiles_item().create_tiles(npc.standing_frame));
            npc_sprites.push_back(bn::move(npc_sprite));
        }

        runtime_state state;
        state.target_pivot_x = start_world_x;
        state.target_pivot_y = start_world_y;

        bn::vector<bn::sprite_ptr, 96> dialog_sprites;
        DialogueBox dialogue_box(text_generator, dialog_sprites);

        auto start_npc_dialog = [&](int npc_index)
        {
            const int npc_standing_frame = world_map_interaction::npc_dialog_facing_frame(state.facing_direction);
            npc_sprites[npc_index].set_tiles(
                    bn::sprite_items::junior.tiles_item().create_tiles(npc_standing_frame));

            world_map_interaction::begin_dialog_state(state, npc_index);
            world_map_dialog::begin_dialog(dialogue_box, npc_definitions, npc_index);
        };

        while(true)
        {
            world_map_npc_visual::sync_npc_sprites(npc_definitions, npc_count, land_bg, npc_sprites);

            const world_map_dialog::dialog_update_result dialog_result =
                    world_map_dialog::update_dialog(dialogue_box, state, host_npc_index);
            if(dialog_result == world_map_dialog::dialog_update_result::start_poker)
            {
                if(load_money() > 0)
                {
                    return true;
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

            if(dialog_result == world_map_dialog::dialog_update_result::continue_loop)
            {
                continue;
            }

            world_map_movement::tick_cooldown(state);

            if(! state.step_cooldown && ! (state.remaining_move_x || state.remaining_move_y))
            {
                const int current_x = land_bg.pivot_x().right_shift_integer();
                const int current_y = land_bg.pivot_y().right_shift_integer();

                try_move_direction(state, direction::left, bn::keypad::left_held(), -tile_step, 0,
                                   x_limit, y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   current_x, current_y,
                                   junior_sprite, junior_animate_action) ||
                try_move_direction(state, direction::right, bn::keypad::right_held(), tile_step, 0,
                                   x_limit, y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   current_x, current_y,
                                   junior_sprite, junior_animate_action) ||
                try_move_direction(state, direction::up, bn::keypad::up_held(), 0, -tile_step,
                                   x_limit, y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   current_x, current_y,
                                   junior_sprite, junior_animate_action) ||
                try_move_direction(state, direction::down, bn::keypad::down_held(), 0, tile_step,
                                   x_limit, y_limit,
                                   npc_definitions, npc_count,
                                   solid_rects, solid_rect_count,
                                   current_x, current_y,
                                   junior_sprite, junior_animate_action);
            }

            const world_map_movement::axis_motion_result x_motion =
                    world_map_movement::update_axis_movement(state, true, move_speed, step_cooldown_frames);
            apply_axis_motion(land_bg, state, x_motion, true);

            const world_map_movement::axis_motion_result y_motion =
                    world_map_movement::update_axis_movement(state, false, move_speed, step_cooldown_frames);
            apply_axis_motion(land_bg, state, y_motion, false);

            if(state.remaining_move_x || state.remaining_move_y)
            {
                junior_animate_action.update();
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
                return false;
            }

            bn::core::update();
        }
    }
}
