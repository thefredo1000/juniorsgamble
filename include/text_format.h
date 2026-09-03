#ifndef TEXT_FORMAT_H
#define TEXT_FORMAT_H

#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_utility.h"

// Minimal "{}" placeholder interpolation for dialogue/HUD text, e.g.:
//
//   Game::text::format<32>("You have {} left", money);
//   Game::text::format<32>("{} vs {}", player_name, opponent_name);
//
// Each "{}" in the format string is replaced, in order, by the next
// argument (stringified with bn::to_string). Extra placeholders with no
// matching argument are left as-is; extra arguments with no matching
// placeholder are ignored.
namespace Game::text
{
    namespace detail
    {
        template<int MaxSize>
        void append_value(bn::string<MaxSize>& out, const bn::string_view& value)
        {
            out.append(value);
        }

        template<int MaxSize, typename Type>
        void append_value(bn::string<MaxSize>& out, const Type& value)
        {
            out.append(bn::to_string<32>(value));
        }

        template<int MaxSize>
        void format_append(bn::string<MaxSize>& out, bn::string_view fmt)
        {
            out.append(fmt);
        }

        template<int MaxSize, typename Type, typename... Rest>
        void format_append(bn::string<MaxSize>& out, bn::string_view fmt, const Type& value, const Rest&... rest)
        {
            const auto placeholder = fmt.find("{}");

            if(placeholder == bn::string_view::npos)
            {
                out.append(fmt);
                return;
            }

            out.append(fmt.substr(0, placeholder));
            append_value(out, value);
            format_append(out, fmt.substr(placeholder + 2), rest...);
        }
    }

    template<int MaxSize, typename... Args>
    [[nodiscard]] bn::string<MaxSize> format(bn::string_view fmt, const Args&... args)
    {
        bn::string<MaxSize> result;
        detail::format_append(result, fmt, args...);
        return result;
    }
}

#endif // TEXT_FORMAT_H
