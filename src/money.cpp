#include "money.h"

#include "bn_array.h"
#include "bn_sram.h"

namespace Game
{
    namespace
    {
        struct SramData
        {
            bn::array<char, 8> format_tag;
            int money = 0;
        };

        constexpr bn::array<char, 8> format_tag = {'m', 'o', 'n', 'e', 'y'};
    }

    int load_money()
    {
        SramData cart_sram_data;
        bn::sram::read(cart_sram_data);

        if(cart_sram_data.format_tag == format_tag)
        {
            return cart_sram_data.money;
        }

        save_money(starting_money);
        return starting_money;
    }

    void save_money(int amount)
    {
        SramData cart_sram_data;
        cart_sram_data.format_tag = format_tag;
        cart_sram_data.money = amount;

        bn::sram::write(cart_sram_data);
    }
}
