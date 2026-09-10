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

    // Pivot coords are centered on the map: dream.bmp is 1024x256, so the
    // pivot ranges are x [-392, 391] and y [-48, 47]. Start in the middle.
    constexpr int start_world_x = 0;
    constexpr int start_world_y = 0;

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
    };

    constexpr bn::string_view guide_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "You look a lot like me...",
        "aren't you my brother?",
    };

    constexpr bn::string_view host_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "I'm the dealer here!",
        "Hold on... you look a lot like me.",
    };

    constexpr bn::string_view traveler_dialog_lines[] = {
        "Hello, my name is Junior P. Luche!",
        "",
        "You look a lot like me...",
        "aren't you my brother?",
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
    // Sprite showcase: one NPC per character sheet, lined up two tiles above
    // the player start so every graphic is on screen as soon as the world map
    // opens. Delete this block (and its entries in npc_definitions) once the
    // sprites are wired into the real cast.
    // ---------------------------------------------------------------------

    constexpr bn::string_view bibi_dialog_lines[] = {
        "I'm Bibi.",
        "",
        "Sprite sheet: bibi.bmp",
    };

    constexpr bn::string_view croupier_dialog_lines[] = {
        "I'm the Croupier.",
        "",
        "Sprite sheet: croupier.bmp",
    };

    constexpr bn::string_view excelsa_dialog_lines[] = {
        "I'm Excelsa.",
        "",
        "Sprite sheet: excelsa.bmp",
    };

    constexpr bn::string_view federica_dialog_lines[] = {
        "I'm Federica.",
        "",
        "Sprite sheet: federica.bmp",
    };

    constexpr bn::string_view ludovico_dialog_lines[] = {
        "I'm Ludovico.",
        "",
        "Sprite sheet: ludovico.bmp",
    };

    constexpr bn::string_view ludoviquito_dialog_lines[] = {
        "I'm Ludoviquito.",
        "",
        "Sprite sheet: ludoviquito.bmp",
    };

    constexpr bn::string_view ninja_dialog_lines[] = {
        "I'm the Ninja.",
        "",
        "Sprite sheet: ninja.bmp",
    };

    constexpr npc_definition npc_definitions[] = {
        { start_world_x + tile_step, start_world_y, 12, guide_dialog_lines,
          int(sizeof(guide_dialog_lines) / sizeof(guide_dialog_lines[0])),
          minigame_id::none },
        { start_world_x - tile_step, start_world_y + tile_step, 8, host_dialog_lines,
          int(sizeof(host_dialog_lines) / sizeof(host_dialog_lines[0])),
          minigame_id::poker },
        { start_world_x + (tile_step * 2), start_world_y + tile_step, 4, traveler_dialog_lines,
          int(sizeof(traveler_dialog_lines) / sizeof(traveler_dialog_lines[0])),
          minigame_id::none },
        { start_world_x + tile_step, start_world_y + (tile_step * 2), 0, slot_host_dialog_lines,
          int(sizeof(slot_host_dialog_lines) / sizeof(slot_host_dialog_lines[0])),
          minigame_id::slots },
        { start_world_x - (tile_step * 2), start_world_y, 8, roulette_host_dialog_lines,
          int(sizeof(roulette_host_dialog_lines) / sizeof(roulette_host_dialog_lines[0])),
          minigame_id::roulette },
        { start_world_x - (tile_step * 2), start_world_y + tile_step, 4, sports_betting_host_dialog_lines,
          int(sizeof(sports_betting_host_dialog_lines) / sizeof(sports_betting_host_dialog_lines[0])),
          minigame_id::sports_betting },

        // Sprite showcase row, two tiles above the player start.
        { start_world_x - (tile_step * 3), start_world_y - (tile_step * 2), 0, bibi_dialog_lines,
          int(sizeof(bibi_dialog_lines) / sizeof(bibi_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::bibi },
        { start_world_x - (tile_step * 2), start_world_y - (tile_step * 2), 0, croupier_dialog_lines,
          int(sizeof(croupier_dialog_lines) / sizeof(croupier_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::croupier },
        { start_world_x - tile_step, start_world_y - (tile_step * 2), 0, excelsa_dialog_lines,
          int(sizeof(excelsa_dialog_lines) / sizeof(excelsa_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::excelsa },
        { start_world_x, start_world_y - (tile_step * 2), 0, federica_dialog_lines,
          int(sizeof(federica_dialog_lines) / sizeof(federica_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::federica },
        { start_world_x + tile_step, start_world_y - (tile_step * 2), 0, ludovico_dialog_lines,
          int(sizeof(ludovico_dialog_lines) / sizeof(ludovico_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::ludovico },
        { start_world_x + (tile_step * 2), start_world_y - (tile_step * 2), 0, ludoviquito_dialog_lines,
          int(sizeof(ludoviquito_dialog_lines) / sizeof(ludoviquito_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::ludoviquito },
        { start_world_x + (tile_step * 3), start_world_y - (tile_step * 2), 0, ninja_dialog_lines,
          int(sizeof(ninja_dialog_lines) / sizeof(ninja_dialog_lines[0])),
          minigame_id::none, &bn::sprite_items::ninja }
    };

    constexpr int npc_count = int(sizeof(npc_definitions) / sizeof(npc_definitions[0]));
}

#endif
