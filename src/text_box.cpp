#include "text_box.h"

#include "bn_vector.h"

namespace Game
{
    TextBox::TextBox(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& sprites) :
        _text_generator(text_generator),
        _sprites(sprites)
    {
    }

    TextBox& TextBox::set_alignment(alignment_type alignment)
    {
        _text_generator.set_alignment(alignment);
        return *this;
    }

    TextBox& TextBox::set_line_height(bn::fixed line_height)
    {
        _line_height = line_height;
        return *this;
    }

    TextBox& TextBox::set_bg_priority(int bg_priority)
    {
        _bg_priority = bg_priority;
        return *this;
    }

    void TextBox::clear()
    {
        _sprites.clear();
    }

    TextBox& TextBox::line(bn::fixed x, bn::fixed y, bn::string_view text)
    {
        const int previous_size = _sprites.size();
        _text_generator.generate(x, y, text, _sprites);

        if(_bg_priority >= 0)
        {
            for(int index = previous_size; index < _sprites.size(); ++index)
            {
                _sprites[index].set_bg_priority(_bg_priority);
            }
        }

        return *this;
    }

    TextBox& TextBox::lines(bn::fixed x, bn::fixed y, const bn::string_view* texts, int count)
    {
        for(int index = 0; index < count; ++index)
        {
            line(x, y + (_line_height * index), texts[index]);
        }

        return *this;
    }
}
