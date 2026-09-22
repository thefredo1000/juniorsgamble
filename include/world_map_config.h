#ifndef WORLD_MAP_CONFIG_H
#define WORLD_MAP_CONFIG_H

#include <cstdint>

#include "bn_sprite_item.h"
#include "bn_string_view.h"

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
    constexpr bn::string_view intro_notice_line_1 = "Where am I?";
    constexpr bn::string_view intro_notice_line_2 = "Am I dreaming?";

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
        const bn::string_view* dialog_lines;
      const bn::sprite_item* sprite_item = &bn::sprite_items::junior;
      int16_t world_x;
      int16_t world_y;
      uint8_t standing_frame;
      uint8_t dialog_lines_count;

        // Saying yes to this NPC's question leaves the world map instead of
        // entering the next view. Only the NPC at the end of the path uses it.
        bool skips_level = false;
    };

    // Counts a dialog line array, so the table below stays readable.
    template<int Count>
    [[nodiscard]] constexpr uint8_t dialog_line_count(const bn::string_view (&)[Count])
    {
      static_assert(Count <= 255, "NPC dialog count exceeds uint8_t storage");
      return static_cast<uint8_t>(Count);
    }

    [[nodiscard]] constexpr npc_definition make_npc_definition(
        int world_x,
        int world_y,
        int standing_frame,
        const bn::string_view* dialog_lines,
        uint8_t dialog_lines_count,
        const bn::sprite_item* sprite_item = &bn::sprite_items::junior,
        bool skips_level = false)
    {
      return {
        dialog_lines,
        sprite_item,
        static_cast<int16_t>(world_x),
        static_cast<int16_t>(world_y),
        static_cast<uint8_t>(standing_frame),
        dialog_lines_count,
        skips_level
      };
    }

    // What the last NPC asks before handing control to the next view.
    constexpr bn::string_view skip_level_question = "Wake up and move on?";

    constexpr bn::string_view host_dialog_lines[] = {
        "Hi my name is Junior P. Luche!",
        "I can't find my Peluchemon cards,",
        "this place is strange...",
        "",
    };

    constexpr bn::string_view slot_host_dialog_lines[] = {
        "Hi, you look just like me!",
        "There's this guy at the end,",
        "better talk to him, he knows what's",
        "going on.",
    };

    constexpr bn::string_view roulette_host_dialog_lines[] = {
        "Why are mom and dad always",
        "screaming in their rooms?",
        "I wonder, what are they up to?",
        "",
    };

    constexpr bn::string_view sports_betting_host_dialog_lines[] = {
        "Am I hallucinating again?",
        "The other day I ate shoe polish...",
        "and my father told me that I was",
        "saying a bunch of nonsense.",
    };

    // ---------------------------------------------------------------------
    // Everyone else on the path. The lines are short prompts that keep the
    // player moving toward the final NPC on the right.
    // ---------------------------------------------------------------------

    constexpr bn::string_view npc_05_dialog_lines[] = {
        "Hey, are you my brother?",
        "I always dreamed of finding you!",
    };

    constexpr bn::string_view npc_06_dialog_lines[] = {
        "Hey handsome!!!",
        "Go talk to the man at the end.",
    };

    constexpr bn::string_view npc_07_dialog_lines[] = {
        "Pozoleeeeee...",
        "Pozole miooooo...",
    };

    constexpr bn::string_view npc_08_dialog_lines[] = {
        "I miss my brother,",
        "I hope he's safe.",
    };

    constexpr bn::string_view npc_09_dialog_lines[] = {
        "The man at the end...",
        "I think he's the one in charge.",
    };

    constexpr bn::string_view npc_10_dialog_lines[] = {
        "Hey, do you know what's going on?",
        "Something strange is happening...",
    };

    constexpr bn::string_view npc_11_dialog_lines[] = {
        "I miss my family a lot, I think",
        "the man at the end can help us.",
    };

    constexpr bn::string_view npc_12_dialog_lines[] = {
        "Are you me? am I you?",
        "Bibi is not the strange one...",
    };

    constexpr bn::string_view npc_13_dialog_lines[] = {
        "Go talk to the man at the end,",
        "He can help.",
    };

    constexpr bn::string_view npc_14_dialog_lines[] = {
        "Are you talking to all of us?",
        "Go talk to the man at the end.",
    };

    constexpr bn::string_view npc_15_dialog_lines[] = {
        "The man at the end can help,",
        "We should go talk to him.",
    };

    constexpr bn::string_view npc_16_dialog_lines[] = {
        "The man at the end...",
        "Maybe he knows what's going on.",
    };

    constexpr bn::string_view final_npc_dialog_lines[] = {
        "I am Commander Alighieri.",
        "The original Junior P. Luche.",
        "Listen carefully:",
        "this is a dream, Junior.",
        "Wake up now.",
        "Your family needs you...",
    };

    // The cast, in the order the player meets it walking left to right. Each
    // entry names its tile in world_map_layout.txt, so the two stay in step.
    constexpr npc_definition npc_definitions[] = {
        //  1: col  3, row  4, facing D
        make_npc_definition(-384, -32, frame_facing_down, host_dialog_lines,
                            dialog_line_count(host_dialog_lines)),
        //  2: col  6, row  8, facing U
        make_npc_definition(-336, 32, frame_facing_up, slot_host_dialog_lines,
                            dialog_line_count(slot_host_dialog_lines)),
        //  3: col 11, row  4, facing R
        make_npc_definition(-256, -32, frame_facing_right, roulette_host_dialog_lines,
                            dialog_line_count(roulette_host_dialog_lines)),
        //  4: col 12, row  9, facing R
        make_npc_definition(-240, 48, frame_facing_right, sports_betting_host_dialog_lines,
                            dialog_line_count(sports_betting_host_dialog_lines)),
        //  5: col 15, row  3, facing L
        make_npc_definition(-192, -48, frame_facing_left, npc_05_dialog_lines,
                            dialog_line_count(npc_05_dialog_lines)),
        //  6: col 19, row 11, facing R
        make_npc_definition(-128, 80, frame_facing_right, npc_06_dialog_lines,
                            dialog_line_count(npc_06_dialog_lines)),
        //  7: col 22, row  2, facing U
        make_npc_definition(-80, -64, frame_facing_up, npc_07_dialog_lines,
                            dialog_line_count(npc_07_dialog_lines)),
        //  8: col 22, row  4, facing R
        make_npc_definition(-80, -32, frame_facing_right, npc_08_dialog_lines,
                            dialog_line_count(npc_08_dialog_lines)),
        //  9: col 26, row  9, facing R
        make_npc_definition(-16, 48, frame_facing_right, npc_09_dialog_lines,
                            dialog_line_count(npc_09_dialog_lines)),
        // 10: col 31, row  1, facing D
        make_npc_definition(64, -80, frame_facing_down, npc_10_dialog_lines,
                            dialog_line_count(npc_10_dialog_lines)),
        // 11: col 33, row  4, facing U
        make_npc_definition(96, -32, frame_facing_up, npc_11_dialog_lines,
                            dialog_line_count(npc_11_dialog_lines)),
        // 12: col 34, row 11, facing L
        make_npc_definition(112, 80, frame_facing_left, npc_12_dialog_lines,
                            dialog_line_count(npc_12_dialog_lines)),
        // 13: col 39, row  9, facing U
        make_npc_definition(192, 48, frame_facing_up, npc_13_dialog_lines,
                            dialog_line_count(npc_13_dialog_lines)),
        // 14: col 40, row  3, facing D
        make_npc_definition(208, -48, frame_facing_down, npc_14_dialog_lines,
                            dialog_line_count(npc_14_dialog_lines)),
        // 15: col 46, row  9, facing D
        make_npc_definition(304, 48, frame_facing_down, npc_15_dialog_lines,
                            dialog_line_count(npc_15_dialog_lines)),
        // 16: col 47, row  4, facing D
        make_npc_definition(320, -32, frame_facing_down, npc_16_dialog_lines,
                            dialog_line_count(npc_16_dialog_lines)),

        // The end of the path: talking to him and saying yes leaves the map.
        // col 54, row 6, facing L
        make_npc_definition(432, 0, frame_facing_left, final_npc_dialog_lines,
                            dialog_line_count(final_npc_dialog_lines), &bn::sprite_items::junior,
                            true)
    };

    constexpr int npc_count = int(sizeof(npc_definitions) / sizeof(npc_definitions[0]));
}

#endif
