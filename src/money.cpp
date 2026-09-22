#include "money.h"

#include <cstdint>

#include "bn_array.h"
#include "bn_sram.h"

namespace Game
{
    namespace
    {
        struct SramData
        {
            bn::array<char, 8> format_tag;
            int money = starting_money;
            int rescued_family_members = 0;
            uint8_t stage = uint8_t(story_stage::none);
            uint8_t has_story_checkpoint = false;
            uint8_t story_finished = false;
            uint8_t reserved_0 = 0;
        };

        constexpr bn::array<char, 8> current_format_tag = {'r', 'u', 'n', 's', 'a', 'v', 'e'};
        constexpr bn::array<char, 8> legacy_format_tag = {'m', 'o', 'n', 'e', 'y'};

        [[nodiscard]] run_save_data default_save_data()
        {
            return {};
        }

        [[nodiscard]] SramData read_sram_data()
        {
            SramData cart_sram_data;
            bn::sram::read(cart_sram_data);
            return cart_sram_data;
        }

        [[nodiscard]] run_save_data from_sram_data(const SramData& cart_sram_data)
        {
            const story_stage stage = cart_sram_data.stage <= uint8_t(story_stage::complete) ?
                                              story_stage(cart_sram_data.stage) : story_stage::none;

            return {
                cart_sram_data.money,
                cart_sram_data.rescued_family_members,
                stage,
                bool(cart_sram_data.has_story_checkpoint),
                bool(cart_sram_data.story_finished)
            };
        }

        [[nodiscard]] SramData to_sram_data(const run_save_data& save_data)
        {
            SramData cart_sram_data;
            cart_sram_data.format_tag = current_format_tag;
            cart_sram_data.money = save_data.money;
            cart_sram_data.rescued_family_members = save_data.rescued_family_members;
            cart_sram_data.stage = uint8_t(save_data.stage);
            cart_sram_data.has_story_checkpoint = save_data.has_story_checkpoint;
            cart_sram_data.story_finished = save_data.story_finished;
            return cart_sram_data;
        }
    }

    run_save_data load_run_save()
    {
        const SramData cart_sram_data = read_sram_data();

        if(cart_sram_data.format_tag == current_format_tag)
        {
            return from_sram_data(cart_sram_data);
        }

        if(cart_sram_data.format_tag == legacy_format_tag)
        {
            run_save_data migrated_save_data = default_save_data();
            migrated_save_data.money = cart_sram_data.money;
            save_run_save(migrated_save_data);
            return migrated_save_data;
        }

        return default_save_data();
    }

    void save_run_save(const run_save_data& save_data)
    {
        const SramData cart_sram_data = to_sram_data(save_data);
        bn::sram::write(cart_sram_data);
    }

    void start_new_story_run()
    {
        run_save_data save_data = default_save_data();
        save_data.stage = story_stage::dream;
        save_data.has_story_checkpoint = true;
        save_run_save(save_data);
    }

    void complete_story_checkpoint()
    {
        run_save_data save_data = load_run_save();

        if(! save_data.has_story_checkpoint)
        {
            save_data.has_story_checkpoint = true;
        }

        if(save_data.stage == story_stage::none)
        {
            save_data.stage = story_stage::dream;
        }

        if(save_data.stage == story_stage::dream)
        {
            save_data.stage = story_stage::casino;
            save_run_save(save_data);
            return;
        }

        if(save_data.rescued_family_members < family_member_goal)
        {
            ++save_data.rescued_family_members;
        }

        if(save_data.rescued_family_members >= family_member_goal)
        {
            save_data.rescued_family_members = family_member_goal;
            save_data.stage = story_stage::complete;
            save_data.story_finished = true;
            save_data.has_story_checkpoint = false;
        }
        else
        {
            save_data.stage = story_stage::casino;
        }

        save_run_save(save_data);
    }

    bool can_continue_story()
    {
        const run_save_data save_data = load_run_save();
        return save_data.has_story_checkpoint && save_data.stage != story_stage::none && ! save_data.story_finished;
    }

    int load_money()
    {
        return load_run_save().money;
    }

    void save_money(int amount)
    {
        run_save_data save_data = load_run_save();
        save_data.money = amount;
        save_run_save(save_data);
    }
}
