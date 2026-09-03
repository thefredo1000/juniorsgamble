#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"
#include "bn_vector_fwd.h"

#include "text_box.h"

namespace Game
{
    // Reusable paginated dialogue/message box. Shows lines two at a time
    // with an "A:Next" hint, can end on a yes/no question, and can show a
    // one-off notice. Owns its own paging state, so callers just call
    // open()/advance()/close() instead of hand-rolling a line-index state
    // machine per screen.
    class DialogueBox
    {
    public:
        DialogueBox(bn::sprite_text_generator& text_generator, bn::ivector<bn::sprite_ptr>& sprites);

        // Starts a multi-page dialogue, two lines per page.
        void open(const bn::string_view* lines, int line_count);

        // Shows a single, non-paginated notice (still dismissed with A/B).
        void open_notice(bn::string_view line_1, bn::string_view line_2 = bn::string_view());

        // Turns the box into a yes/no question.
        void open_question(bn::string_view prompt, bn::string_view yes_label, bn::string_view no_label);

        [[nodiscard]] bool is_open() const
        {
            return _open;
        }

        [[nodiscard]] bool is_question_open() const
        {
            return _question_open;
        }

        [[nodiscard]] bool has_more_pages() const;

        [[nodiscard]] int question_index() const
        {
            return _question_index;
        }

        void move_question_cursor(int index);

        // Advances to the next page. Returns false without changing
        // anything when the last page was already showing, so the caller
        // can decide what happens next (close(), or open_question()).
        bool advance();

        void close();

    private:
        void redraw_lines();
        void redraw_question();

        TextBox _text_box;

        const bn::string_view* _lines = nullptr;
        int _line_count = 0;
        int _line_index = 0;

        bool _open = false;
        bool _question_open = false;
        int _question_index = 0;

        bn::string_view _question_prompt;
        bn::string_view _question_yes_label;
        bn::string_view _question_no_label;
    };
}

#endif // DIALOGUE_BOX_H
