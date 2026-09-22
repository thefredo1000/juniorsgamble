#ifndef MONEY_H
#define MONEY_H

namespace Game
{
    constexpr int starting_money = 100;
    constexpr int family_member_goal = 5;

    enum class story_stage
    {
        none,
        dream,
        casino,
        complete
    };

    struct run_save_data
    {
        int money = starting_money;
        int rescued_family_members = 0;
        story_stage stage = story_stage::none;
        bool has_story_checkpoint = false;
        bool story_finished = false;
    };

    [[nodiscard]] run_save_data load_run_save();
    void save_run_save(const run_save_data& save_data);
    void start_new_story_run();
    void complete_story_checkpoint();
    [[nodiscard]] bool can_continue_story();

    int load_money();
    void save_money(int amount);
}

#endif // MONEY_H
