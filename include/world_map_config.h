#ifndef WORLD_MAP_CONFIG_H
#define WORLD_MAP_CONFIG_H

#include "bn_sprite_item.h"
#include "bn_string_view.h"

#include "minigame.h"

#include "bn_sprite_items_bibi.h"
#include "bn_sprite_items_croupier.h"
#include "bn_sprite_items_excelsa.h"
#include "bn_sprite_items_federica.h"
#include "bn_sprite_items_junior.h"
#include "bn_sprite_items_ludovico.h"
#include "bn_sprite_items_ludoviquito.h"
#include "bn_sprite_items_ninja.h"

namespace Game::world_map_config
{
    constexpr int tile_step = 16;
    constexpr int move_speed = 1;
    constexpr int step_cooldown_frames = 2;

    // dream.bmp is a white blob sitting inside a black border, and the player
    // belongs on the white. Measured on the art, the white is solid from pixel
    // (74, 19) to (947, 237) of the 1024x256 bitmap, so it is inset by about
    // this much from every edge. Re-measure these two if the map art changes.
    constexpr int map_border_x = 75;
    constexpr int map_border_y = 19;

    // The left and right border is dithered noise rather than a hard line, so
    // the player can step into it and still read as standing at the edge of the
    // map instead of off it. The top and bottom border is only a fifth as wide,
    // and a whole step into it puts his feet on black, so he stops short there.
    constexpr int edge_overshoot_steps_x = 1;
    constexpr int edge_overshoot_steps_y = 0;

    // World coords are centered on the map, so that border puts the walkable
    // bounds at x [-432, 432] and y [-96, 96] (the player stops on the last
    // whole tile_step that fits). The camera only follows him within
    // x [-392, 392] and y [-48, 48]; for the rest of the way the view is frozen
    // and he walks toward the edge of the screen.
    //
    // The player starts at the leftmost tile of the path and the NPCs below run
    // left to right from there. Every coordinate in this file comes from
    // world_map_layout.txt: world_x = -432 + (16 * col), world_y = -96 + (16 * row).
    constexpr int start_world_x = -432;
    constexpr int start_world_y = 0;

    // Standing frame per facing direction, the same numbers
    // world_map_logic::direction_standing_frame returns. The layout file marks
    // each NPC with one of U/D/L/R; these are those four.
    constexpr int frame_facing_down = 0;
    constexpr int frame_facing_up = 4;
    constexpr int frame_facing_left = 8;
    constexpr int frame_facing_right = 12;

    struct npc_definition
    {
        int world_x;
        int world_y;
        int standing_frame;
        const bn::string_view* dialog_lines;
        int dialog_lines_count;
        // What talking to this NPC offers to start, once its dialog lines finish
        // and the player confirms the follow-up yes/no question.
        minigame_id game_trigger = minigame_id::none;

        // Which graphic this NPC uses. Every character sheet is laid out like
        // junior.bmp (16x256, sixteen 16x16 frames), so any of them can be
        // dropped in here.
        const bn::sprite_item* sprite_item = &bn::sprite_items::junior;

        // Saying yes to this NPC's question leaves the world map instead of
        // starting a game. Only the NPC at the end of the path uses it, and an
        // NPC is either a game host or the way out, never both.
        bool skips_level = false;
    };

    // Counts a dialog line array, so the table below stays readable.
    template<int Count>
    [[nodiscard]] constexpr int dialog_line_count(const bn::string_view (&)[Count])
    {
        return Count;
    }

    // What the last NPC asks, the way a minigame host asks its own question.
    constexpr bn::string_view skip_level_question = "Skip this level?";

    constexpr bn::string_view host_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "I'm the dealer here!",
        "Hold on... you look a lot like me.",
    };

    constexpr bn::string_view slot_host_dialog_lines[] = {
        "Hey there, care to try your luck?",
        "",
        "I run the slot machine over here.",
        "Line up three of a kind to win big!",
    };

    constexpr bn::string_view roulette_host_dialog_lines[] = {
        "Welcome to the roulette table!",
        "",
        "Red, black, odds, or a single number...",
        "Where does your luck lie?",
    };

    constexpr bn::string_view sports_betting_host_dialog_lines[] = {
        "Hey! Cruz Azul plays today!",
        "",
        "I take bets on how the match goes.",
        "Win, lose, or draw, it's your call!",
    };

    // ---------------------------------------------------------------------
    // Everyone else on the path. The lines are placeholders: two of them show
    // per page, so write as many pairs as the conversation needs. Give an NPC
    // a game_trigger to have it offer a minigame once its lines run out.
    // ---------------------------------------------------------------------

    constexpr bn::string_view npc_05_dialog_lines[] = {
        "NPC 5 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_06_dialog_lines[] = {
        "NPC 6 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_07_dialog_lines[] = {
        "NPC 7 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_08_dialog_lines[] = {
        "NPC 8 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_09_dialog_lines[] = {
        "NPC 9 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_10_dialog_lines[] = {
        "NPC 10 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_11_dialog_lines[] = {
        "NPC 11 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_12_dialog_lines[] = {
        "NPC 12 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_13_dialog_lines[] = {
        "NPC 13 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_14_dialog_lines[] = {
        "NPC 14 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_15_dialog_lines[] = {
        "NPC 15 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view npc_16_dialog_lines[] = {
        "NPC 16 here.",
        "Write my lines here.",
    };

    constexpr bn::string_view final_npc_dialog_lines[] = {
        "So you made it all the way here.",
        "",
        "There is nothing left for you on this",
        "map. Shall I move you along?",
    };

    // The cast, in the order the player meets it walking left to right. Each
    // entry names its tile in world_map_layout.txt, so the two stay in step.
    constexpr npc_definition npc_definitions[] = {
        //  1: col  3, row  4, facing D
        { -384, -32, frame_facing_down, host_dialog_lines,
          dialog_line_count(host_dialog_lines), minigame_id::poker },
        //  2: col  6, row  8, facing U
        { -336, 32, frame_facing_up, slot_host_dialog_lines,
          dialog_line_count(slot_host_dialog_lines), minigame_id::slots },
        //  3: col 11, row  4, facing R
        { -256, -32, frame_facing_right, roulette_host_dialog_lines,
          dialog_line_count(roulette_host_dialog_lines), minigame_id::roulette },
        //  4: col 12, row  9, facing R
        { -240, 48, frame_facing_right, sports_betting_host_dialog_lines,
          dialog_line_count(sports_betting_host_dialog_lines), minigame_id::sports_betting },
        //  5: col 15, row  3, facing L
        { -192, -48, frame_facing_left, npc_05_dialog_lines,
          dialog_line_count(npc_05_dialog_lines), minigame_id::none },
        //  6: col 19, row 11, facing R
        { -128, 80, frame_facing_right, npc_06_dialog_lines,
          dialog_line_count(npc_06_dialog_lines), minigame_id::none },
        //  7: col 22, row  2, facing U
        { -80, -64, frame_facing_up, npc_07_dialog_lines,
          dialog_line_count(npc_07_dialog_lines), minigame_id::none },
        //  8: col 22, row  4, facing R
        { -80, -32, frame_facing_right, npc_08_dialog_lines,
          dialog_line_count(npc_08_dialog_lines), minigame_id::none },
        //  9: col 26, row  9, facing R
        { -16, 48, frame_facing_right, npc_09_dialog_lines,
          dialog_line_count(npc_09_dialog_lines), minigame_id::none },
        // 10: col 31, row  1, facing D
        { 64, -80, frame_facing_down, npc_10_dialog_lines,
          dialog_line_count(npc_10_dialog_lines), minigame_id::none },
        // 11: col 33, row  4, facing U
        { 96, -32, frame_facing_up, npc_11_dialog_lines,
          dialog_line_count(npc_11_dialog_lines), minigame_id::none },
        // 12: col 34, row 11, facing L
        { 112, 80, frame_facing_left, npc_12_dialog_lines,
          dialog_line_count(npc_12_dialog_lines), minigame_id::none },
        // 13: col 39, row  9, facing U
        { 192, 48, frame_facing_up, npc_13_dialog_lines,
          dialog_line_count(npc_13_dialog_lines), minigame_id::none },
        // 14: col 40, row  3, facing D
        { 208, -48, frame_facing_down, npc_14_dialog_lines,
          dialog_line_count(npc_14_dialog_lines), minigame_id::none },
        // 15: col 46, row  9, facing D
        { 304, 48, frame_facing_down, npc_15_dialog_lines,
          dialog_line_count(npc_15_dialog_lines), minigame_id::none },
        // 16: col 47, row  4, facing D
        { 320, -32, frame_facing_down, npc_16_dialog_lines,
          dialog_line_count(npc_16_dialog_lines), minigame_id::none },

        // The end of the path: talking to him and saying yes leaves the map.
        // col 54, row 6, facing L
        { 432, 0, frame_facing_left, final_npc_dialog_lines,
          dialog_line_count(final_npc_dialog_lines), minigame_id::none,
          &bn::sprite_items::junior, true }
    };

    constexpr int npc_count = int(sizeof(npc_definitions) / sizeof(npc_definitions[0]));
}

#endif
