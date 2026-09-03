#ifndef MONEY_H
#define MONEY_H

namespace Game
{
    constexpr int starting_money = 100;

    int load_money();
    void save_money(int amount);
}

#endif // MONEY_H
