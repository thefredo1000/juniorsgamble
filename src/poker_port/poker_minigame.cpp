/*
 * Copyright (c) 2020-2023 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */
#include "bn_core.h"
#include "bn_fixed.h"
#include "bn_music.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "bn_music_items.h"
#include "bn_regular_bg_items_title_screen.h"

#include "common_variable_8x16_sprite_font.h"

#include "scene_type.h"
#include "title_screen.h"
#include "menu_screen.h"
#include "match_screen.h"
#include "extras_screen.h"
#include "poker_minigame.h"

namespace Game
{
    void poker_run(bool show_title_screen)
    {
        constexpr bn::fixed title_music_volume = bn::fixed::from_data(bn::fixed::scale() / 2);
        bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);

        bn::optional<bn::regular_bg_ptr> table_background;
        if(! show_title_screen)
        {
            table_background = bn::regular_bg_items::title_screen.create_bg(8, 48);
            table_background->set_blending_enabled(false);
        }

        Game::SceneType scene = show_title_screen ? Game::SceneType::TITLE : Game::SceneType::MENU;
        bool running = true;

        while (running)
        {
            switch (scene)
            {
            case Game::SceneType::TITLE:
                Game::title_screen();
                if(! table_background)
                {
                    table_background = bn::regular_bg_items::title_screen.create_bg(8, 48);
                    table_background->set_blending_enabled(false);
                }
                scene = Game::SceneType::MENU;
                bn::music_items::hassans_spaceship.play(title_music_volume);
                break;
            case Game::SceneType::MENU:
                scene = Game::menu_screen();
                break;
            case Game::SceneType::GAME:
                scene = Game::match_screen(text_generator);
                break;
            case Game::SceneType::EXTRAS:
                scene = Game::extras_screen();
                break;
            case Game::SceneType::EXIT:
                running = false;
                break;
            default:
                scene = Game::SceneType::MENU;
                break;
            }
        }
    }
}

#ifndef POKER_DISABLE_STANDALONE_MAIN
int main()
{
    bn::core::init();
    Game::poker_run(true);
}
#endif
