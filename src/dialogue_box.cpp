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

        // Pokemon-like text crawl: one character per frame reads clearly while
        // still feeling responsive.
        constexpr int characters_per_tick = 1;
        constexpr int punctuation_pause_frames = 5;

        [[nodiscard]] int line_size(bn::string_view line)
        {
            return line.size();
        }

        [[nodiscard]] bool is_pause_punctuation(char character)
        {
            switch(character)
            {
                case '.':
                case '!':
                case '?':
                case ';':
                case ':':
                    return true;
                default:
                    return false;
            }
        }
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
        _visible_characters = 0;
        _pause_frames_remaining = 0;
        _open = true;
        _question_open = false;
        _background.set_visible(true);

        load_current_page_lines();
        redraw_lines();
    }

    void DialogueBox::open_notice(bn::string_view line_1, bn::string_view line_2)
    {
        _lines = nullptr;
        _line_count = 0;
        _line_index = 0;
        _current_line_1 = line_1;
        _current_line_2 = line_2;
        _visible_characters = 0;
        _pause_frames_remaining = 0;
        _open = true;
        _question_open = false;
        _background.set_visible(true);

        redraw_lines();
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

    bool DialogueBox::is_current_page_fully_visible() const
    {
        return _visible_characters >= line_size(_current_line_1) + line_size(_current_line_2);
    }

    void DialogueBox::tick()
    {
        if(! _open || _question_open || is_current_page_fully_visible())
        {
            return;
        }

        if(_pause_frames_remaining > 0)
        {
            --_pause_frames_remaining;
            return;
        }

        _visible_characters += characters_per_tick;

        const int current_page_size = line_size(_current_line_1) + line_size(_current_line_2);

        if(_visible_characters > current_page_size)
        {
            _visible_characters = current_page_size;
        }

        if(_visible_characters > 0 && _visible_characters < current_page_size)
        {
            const int revealed_index = _visible_characters - 1;
            const char revealed_character = revealed_index < line_size(_current_line_1) ?
                    _current_line_1[revealed_index] :
                    _current_line_2[revealed_index - line_size(_current_line_1)];

            if(is_pause_punctuation(revealed_character))
            {
                _pause_frames_remaining = punctuation_pause_frames;
            }
        }

        redraw_lines();
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
        if(! is_current_page_fully_visible())
        {
            reveal_current_page();
            return true;
        }

        if(! has_more_pages())
        {
            return false;
        }

        _line_index += 2;
        _visible_characters = 0;
        _pause_frames_remaining = 0;
        load_current_page_lines();
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
        _current_line_1 = bn::string_view();
        _current_line_2 = bn::string_view();
        _visible_characters = 0;
        _pause_frames_remaining = 0;
        _question_index = 0;
        _background.set_visible(false);
        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::CENTER);
    }

    void DialogueBox::load_current_page_lines()
    {
        _current_line_1 = _line_count > 0 ? _lines[_line_index] : bn::string_view();

        if(_line_index + 1 < _line_count)
        {
            _current_line_2 = _lines[_line_index + 1];
        }
        else
        {
            _current_line_2 = bn::string_view();
        }
    }

    void DialogueBox::redraw_lines()
    {
        const int first_line_visible = _visible_characters < _current_line_1.size() ?
            _visible_characters : line_size(_current_line_1);
        const int second_line_visible = _visible_characters > line_size(_current_line_1) ?
            _visible_characters - line_size(_current_line_1) : 0;

        _text_box.clear();
        _text_box.set_alignment(TextBox::alignment_type::LEFT);
        _text_box.line(box_x, line_1_y,
                       bn::string_view(_current_line_1.data(), first_line_visible));

        if(second_line_visible > 0)
        {
            const int clamped_second_line_visible = second_line_visible < line_size(_current_line_2) ?
                second_line_visible : line_size(_current_line_2);
            _text_box.line(box_x, line_2_y,
                           bn::string_view(_current_line_2.data(), clamped_second_line_visible));
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

    void DialogueBox::reveal_current_page()
    {
        _visible_characters = line_size(_current_line_1) + line_size(_current_line_2);
        redraw_lines();
    }
}
