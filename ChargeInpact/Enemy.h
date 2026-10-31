#pragma once
#include "Character.h"
#include "Strategy.h"

class Enemy : public Character
{
public:
    void Init();

    int SelectAction(StrategyType playerStrategy);
    int GetAttackPower();
};