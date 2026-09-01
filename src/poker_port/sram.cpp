#include "sram.h"

#include "bn_core.h"
#include "bn_sram.h"

namespace Game
{

    struct SramData
    {
        bn::array<char, 8> format_tag;
        int money = 0;
    };

    int read_sram()
    {
        SramData cart_sram_data;
        bn::sram::read(cart_sram_data);

        bn::array<char, 8> expected_format_tag = {'m', 'o', 'n', 'e', 'y'};

        if (cart_sram_data.format_tag == expected_format_tag)
        {
            return cart_sram_data.money;
        }
        else
        {
            cart_sram_data.format_tag = expected_format_tag;
            cart_sram_data.money = 100;

            bn::sram::clear(bn::sram::size());
            bn::sram::write(cart_sram_data);
        }

        return cart_sram_data.money;
    }

    void write_sram(int money)
    {

        SramData cart_sram_data;

        bn::array<char, 8> expected_format_tag = {'m', 'o', 'n', 'e', 'y'};

        cart_sram_data.format_tag = expected_format_tag;
        cart_sram_data.money = money;

        bn::sram::clear(bn::sram::size());

        bn::sram::write(cart_sram_data);
    }
}