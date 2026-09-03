#include "dialogue_box.h"

namespace Game
{
    namespace
    {
        constexpr bn::fixed box_x = -112;
        constexpr bn::fixed separator_y = 34;
        constexpr bn::fixed line_1_y = 50;
        constexpr bn::fixed line_2_y = 66;
        constexpr bn::string_view separator = "----------------------";
    }

    DialogueBox::DialogueBox(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& sprites) :
        _text_box(text_generator, sprites)
    {
    }

    void DialogueBox::open(const bn::string_view* lines, int line_count)
    {
        _lines = lines;
        _line_count = line_count;
        _line_index = 0;
        _open = true;
        _question_open = false;

        redraw_lines();
    }

    void DialogueBox::open_notice(bn::string_view line_1, bn::string_view line_2)
    {
        _lines = nullptr;
        _line_count = 0;
        _open = true;
        _question_open = false;

        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, separator_y, separator);
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
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::CENTER);
    }

    void DialogueBox::redraw_lines()
    {
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, separator_y, separator);
        _text_box.line(box_x, line_1_y, _lines[_line_index]);

        if(_line_index + 1 < _line_count)
        {
            _text_box.line(box_x, line_2_y, _lines[_line_index + 1]);
        }

        _text_box.line(64, 74, "A:Next");
    }

    void DialogueBox::redraw_question()
    {
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, separator_y, separator);
        _text_box.line(box_x, line_1_y, _question_prompt);
        _text_box.line(-96, line_2_y, _question_yes_label);
        _text_box.line(-24, line_2_y, _question_no_label);

        const bn::fixed cursor_x = _question_index == 0 ? -110 : -38;
        _text_box.line(cursor_x, line_2_y, ">");
    }
}
