#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "bn_fixed.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"
#include "bn_vector_fwd.h"

namespace Game
{
    // Thin wrapper around bn::sprite_text_generator: owns the horizontal
    // alignment / line-height policy for a block of text so screens stop
    // calling text_generator.generate(...) directly with hardcoded
    // coordinates and manual alignment flips. Sprite storage is still
    // supplied by the caller (an ivector), matching Butano's usual
    // per-screen sprite pools.
    class TextBox
    {
    public:
        using alignment_type = bn::sprite_text_generator::alignment_type;

        TextBox(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& sprites);

        TextBox& set_alignment(alignment_type alignment);
        TextBox& set_line_height(bn::fixed line_height);

        // Sets the background priority applied to every sprite this TextBox
        // generates from now on (see bn::sprite_ptr::set_bg_priority).
        // Useful when the text has to win a layering tie against another
        // background, e.g. DialogueBox's own panel art. -1 (the default)
        // leaves generated sprites at Butano's default priority.
        TextBox& set_bg_priority(int bg_priority);

        void clear();

        // Draws a single line at (x, y).
        TextBox& line(bn::fixed x, bn::fixed y, bn::string_view text);

        // Draws consecutive lines starting at (x, y), stepping by line_height() each line.
        TextBox& lines(bn::fixed x, bn::fixed y, const bn::string_view* texts, int count);

    private:
        bn::sprite_text_generator& _text_generator;
        bn::ivector<bn::sprite_ptr>& _sprites;
        bn::fixed _line_height = 16;
        int _bg_priority = -1;
    };
}

#endif // TEXT_BOX_H
