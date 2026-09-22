#include "world_map_screen.h"

#include "bn_algorithm.h"
#include "bn_blending.h"
#include "bn_bgs_mosaic.h"
#include "bn_bgs_mosaic_actions.h"
#include "bn_camera_ptr.h"
#include "bn_core.h"
#include "bn_display.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_point.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprites_mosaic.h"
#include "bn_sprites_mosaic_actions.h"
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

    constexpr direction dream_intro_spin_directions[] = {
        direction::down,
        direction::right,
        direction::up,
        direction::left,
    };

    void wait_for_intro_input_release()
    {
        while(bn::keypad::a_held() || bn::keypad::start_held() || bn::keypad::b_held())
        {
            bn::core::update();
        }
    }

    [[nodiscard]] bn::string_view intro_notice_line_1_for(const Game::run_save_data& save_data)
    {
        return save_data.stage == Game::story_stage::casino ?
                       "I have to save my family." : Game::world_map_config::intro_notice_line_1;
    }

    [[nodiscard]] bn::string_view intro_notice_line_2_for(const Game::run_save_data& save_data)
    {
        return save_data.stage == Game::story_stage::casino ?
                       "Time to win at the casino." : Game::world_map_config::intro_notice_line_2;
    }

    void run_intro_dialogue(Game::DialogueBox& dialogue_box, const Game::run_save_data& save_data)
    {
        wait_for_intro_input_release();

        if(save_data.stage == Game::story_stage::dream)
        {
            constexpr bn::string_view intro_dialog_lines[] = {
                Game::world_map_config::intro_notice_line_1,
                Game::world_map_config::intro_notice_line_2
            };

            dialogue_box.open(intro_dialog_lines, 2);
        }
        else
        {
            dialogue_box.open_notice(intro_notice_line_1_for(save_data),
                                     intro_notice_line_2_for(save_data));
        }

        wait_for_intro_input_release();

        while(dialogue_box.is_open())
        {
            dialogue_box.tick();

            if(Game::input::confirm_pressed())
            {
                if(! dialogue_box.advance())
                {
                    dialogue_box.close();
                }
            }
            else if(Game::input::back_pressed())
            {
                dialogue_box.close();
            }

            bn::core::update();
        }
    }

    void run_dream_intro_cinematic(bn::regular_bg_ptr& map_bg,
                                   bn::sprite_ptr& junior_sprite,
                                   bn::ivector<bn::sprite_ptr>& npc_sprites,
                                   runtime_state& state)
    {
        constexpr int intro_frames = 88;
        constexpr int deblur_frames = 40;
        constexpr int fade_frames = 16;
        constexpr int fall_start_frame = 12;
        constexpr int spin_hold_frames = 4;
        constexpr int settle_frames = 8;
        constexpr int offscreen_y = -96;
        constexpr bn::fixed blur_stretch = 1;
        constexpr bn::fixed start_scale = 2;
        constexpr bn::fixed end_scale = 1;

        bn::bgs_mosaic::set_stretch(blur_stretch);
        bn::sprites_mosaic::set_stretch(blur_stretch);
        map_bg.set_mosaic_enabled(true);
        map_bg.set_blending_enabled(true);
        junior_sprite.set_mosaic_enabled(true);
        junior_sprite.set_blending_enabled(true);
        junior_sprite.set_visible(false);
        junior_sprite.set_scale(start_scale);
        junior_sprite.set_position(state.world_x, offscreen_y);

        for(bn::sprite_ptr& npc_sprite : npc_sprites)
        {
            npc_sprite.set_mosaic_enabled(true);
            npc_sprite.set_blending_enabled(true);
        }

        bn::blending::set_black_fade_color();
        bn::blending::set_fade_alpha(1);

        wait_for_intro_input_release();

        bn::bgs_mosaic_stretch_to_action deblur_action(deblur_frames, 0);
        bn::sprites_mosaic_stretch_to_action sprite_deblur_action(deblur_frames, 0);

        for(int frame = 0; frame < intro_frames; ++frame)
        {
            if(frame < fade_frames)
            {
                bn::blending::set_fade_alpha(bn::fixed(fade_frames - frame - 1) / fade_frames);
            }

            if(frame < deblur_frames)
            {
                deblur_action.update();
                sprite_deblur_action.update();
            }

            if(frame == fall_start_frame)
            {
                junior_sprite.set_visible(true);
            }

            if(junior_sprite.visible())
            {
                const int fall_frame = frame - fall_start_frame;
                const int fall_frames = intro_frames - fall_start_frame;
                const bn::fixed progress = bn::fixed(fall_frame) / fall_frames;
                const bn::fixed eased_progress = progress * progress;
                const int junior_y = offscreen_y + int((state.world_y - offscreen_y) * eased_progress);
                const bn::fixed scale = start_scale - ((start_scale - end_scale) * progress);
                const int spin_index = (fall_frame / spin_hold_frames) % int(sizeof(dream_intro_spin_directions) /
                                                                              sizeof(dream_intro_spin_directions[0]));
                const direction spin_direction = fall_frame >= fall_frames - settle_frames ?
                                                        direction::down : dream_intro_spin_directions[spin_index];

                junior_sprite.set_y(junior_y);
                junior_sprite.set_scale(scale);
                Game::world_map_player_visual::set_standing_frame(junior_sprite, spin_direction);
                state.facing_direction = spin_direction;
                state.animation_direction = spin_direction;
            }

            bn::core::update();
        }

        junior_sprite.set_scale(end_scale);
        junior_sprite.set_position(state.world_x, state.world_y);

        bn::blending::set_fade_alpha(0);
        bn::bgs_mosaic::set_stretch(0);
        bn::sprites_mosaic::set_stretch(0);
        map_bg.set_mosaic_enabled(false);
        map_bg.set_blending_enabled(false);
        junior_sprite.set_mosaic_enabled(false);
        junior_sprite.set_blending_enabled(false);

        for(bn::sprite_ptr& npc_sprite : npc_sprites)
        {
            npc_sprite.set_mosaic_enabled(false);
            npc_sprite.set_blending_enabled(false);
        }

        junior_sprite.set_scale(end_scale);
        junior_sprite.set_position(state.world_x, state.world_y);
        Game::world_map_player_visual::set_standing_frame(junior_sprite, direction::down);
        state.facing_direction = direction::down;
        state.animation_direction = direction::down;

        wait_for_intro_input_release();
    }

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
        const run_save_data save_data = load_run_save();

        hud_text_box.set_alignment(TextBox::alignment_type::LEFT)
            .line(-112, -68, text::format<16>("Money: ${}", save_data.money));

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
        world_map_npc_visual::sync_npc_sprites(npc_definitions, npc_count, camera, npc_sprites);

        bn::vector<bn::sprite_ptr, 96> dialog_sprites;
        DialogueBox dialogue_box(text_generator, dialog_sprites);

        if(save_data.stage == story_stage::dream)
        {
            for(bn::sprite_ptr& hud_sprite : hud_sprites)
            {
                hud_sprite.set_visible(false);
            }

            run_dream_intro_cinematic(map_bg, junior_sprite, npc_sprites, state);

            for(bn::sprite_ptr& hud_sprite : hud_sprites)
            {
                hud_sprite.set_visible(true);
            }
        }

        run_intro_dialogue(dialogue_box, save_data);

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
