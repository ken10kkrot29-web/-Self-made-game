#include "Enemy.h"
#include "DxLib.h"
#include "Strategy.h"

void Enemy::Init()
{
    Character::Init(100);
}

int Enemy::SelectAction(StrategyType playerStrategy)
{
    int random = GetRand(2);

    // プレイヤーが攻撃を選んだ場合
    if (playerStrategy == StrategyType::Attack)
    {
        return random;
    }

    // プレイヤーが防御を選んだ場合
    if (playerStrategy == StrategyType::Defense)
    {
        return GetRand(1);
    }

    // プレイヤーが奇策を選んだ場合
    return GetRand(2);
}

int Enemy::GetAttackPower()
{
    return 10;
}