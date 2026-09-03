#include "dialogue_box.h"

#include "bn_regular_bg_items_text_box.h"

namespace Game
{
    namespace
    {
        // Text layout inside the text_box.bmp panel (a 240x48 art piece
        // flush with the bottom of the screen, spanning relative y 32..80):
        // two rows, each centered in its own half of the panel.
        constexpr bn::fixed box_x = -110;
        constexpr bn::fixed line_1_y = 46;
        constexpr bn::fixed line_2_y = 64;

        // Question row: a ">" cursor sits just left of each option, options
        // spaced 72px apart.
        constexpr bn::fixed cursor_indent = 8;
        constexpr bn::fixed option_spacing = 72;

        // Below the default (3) sprite/background priority, so the panel
        // wins over world-map sprites (NPCs, player) sitting at the
        // default. The dialogue's own text sprites are pinned to the same
        // priority so they still win their tie against the panel (equal
        // priority always resolves in favor of the sprite on GBA hardware).
        constexpr int panel_priority = 2;
    }

    DialogueBox::DialogueBox(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& sprites) :
        _background(bn::regular_bg_items::text_box.create_bg(8, 48)),
        _text_box(text_generator, sprites)
    {
        _background.set_visible(false);
        _background.set_priority(panel_priority);
        _text_box.set_bg_priority(panel_priority);
    }

    void DialogueBox::open(const bn::string_view* lines, int line_count)
    {
        _lines = lines;
        _line_count = line_count;
        _line_index = 0;
        _open = true;
        _question_open = false;
        _background.set_visible(true);

        redraw_lines();
    }

    void DialogueBox::open_notice(bn::string_view line_1, bn::string_view line_2)
    {
        _lines = nullptr;
        _line_count = 0;
        _open = true;
        _question_open = false;
        _background.set_visible(true);

        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, line_1_y, line_1);

        if(line_2.size() > 0)
        {
            _text_box.line(box_x, line_2_y, line_2);
        }
    }

    void DialogueBox::open_question(bn::string_view prompt, bn::string_view yes_label, bn::string_view no_label)
    {
        _question_prompt = prompt;
        _question_yes_label = yes_label;
        _question_no_label = no_label;
        _question_index = 0;
        _open = true;
        _question_open = true;
        _background.set_visible(true);

        redraw_question();
    }

    bool DialogueBox::has_more_pages() const
    {
        return _line_index + 2 < _line_count;
    }

    void DialogueBox::move_question_cursor(int index)
    {
        if(_question_index == index)
        {
            return;
        }

        _question_index = index;
        redraw_question();
    }

    bool DialogueBox::advance()
    {
        if(! has_more_pages())
        {
            return false;
        }

        _line_index += 2;
        redraw_lines();
        return true;
    }

    void DialogueBox::close()
    {
        _open = false;
        _question_open = false;
        _lines = nullptr;
        _line_count = 0;
        _line_index = 0;
        _question_index = 0;
        _background.set_visible(false);
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::CENTER);
    }

    void DialogueBox::redraw_lines()
    {
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, line_1_y, _lines[_line_index]);

        if(_line_index + 1 < _line_count)
        {
            _text_box.line(box_x, line_2_y, _lines[_line_index + 1]);
        }
    }

    void DialogueBox::redraw_question()
    {
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, line_1_y, _question_prompt);
        _text_box.line(box_x + cursor_indent, line_2_y, _question_yes_label);
        _text_box.line(box_x + cursor_indent + option_spacing, line_2_y, _question_no_label);

        const bn::fixed cursor_x = box_x + (_question_index == 0 ? 0 : option_spacing);
        _text_box.line(cursor_x, line_2_y, ">");
    }
}
