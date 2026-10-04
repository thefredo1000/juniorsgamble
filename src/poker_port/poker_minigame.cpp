/*
 * Copyright (c) 2020-2023 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_core.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palettes.h"
#include "bn_sprite_text_generator.h"

#include "bn_regular_bg_items_poker_table.h"

#include "madspixel_sprite_font.h"

#include "match_screen.h"
#include "poker_minigame.h"

namespace Game
{
    void poker_run()
    {
        bn::sprite_text_generator text_generator(Game::madspixel_sprite_font);

        // Start fully black, the first hand fades the table in once it is set up.
        bn::bg_palettes::set_fade(bn::color(0, 0, 0), 1);
        bn::sprite_palettes::set_fade(bn::color(0, 0, 0), 1);

        bn::regular_bg_ptr table_background = bn::regular_bg_items::poker_table.create_bg(8, 48);
        table_background.set_blending_enabled(false);

        bool fade_in = true;

        while(Game::match_screen(text_generator, fade_in))
        {
            fade_in = false;
        }
    }
}

#ifndef POKER_DISABLE_STANDALONE_MAIN
int main()
{
    bn::core::init();
    Game::poker_run();
}
#endif
