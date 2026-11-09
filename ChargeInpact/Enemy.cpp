#include "Enemy.h"
#include "DxLib.h"
#include "Strategy.h"

// 敵の初期化
void Enemy::Init(int battleNumber)
{
    this->battleNumber = battleNumber;

    // すべての能力をリセット
    for (int i = 0; i < 6; i++)
    {
        abilities[i] = false;
        sealTurns[i] = 0;
    }

    // 1試合目：攻撃型
    if (battleNumber == 0)
    {
        Character::Init(100, 25, 5);

        abilities[0] = true; // 攻撃
    }

    // 2試合目：防御型
    else if (battleNumber == 1)
    {
        Character::Init(120, 12, 20);

        abilities[0] = true; // 攻撃
        abilities[1] = true; // 防御
        abilities[2] = true; // 回復
    }

    // 3試合目：最終ボス
    else if (battleNumber == 2)
    {
        Character::Init(150, 25, 20);

        abilities[0] = true; // 攻撃
        abilities[1] = true; // 防御
        abilities[2] = true; // 回復
        abilities[5] = true; // CI速度妨害
    }
}

// 敵の行動を選択する
EnemyAbility Enemy::SelectAction(StrategyType playerStrategy)
{
    int weights[6] = { 0, 0, 0, 0, 0, 0 };

    // 1試合目：攻撃型
    if (battleNumber == 0)
    {
        weights[0] = 100;
    }

    // 2試合目：防御型
    else if (battleNumber == 1)
    {
        if (GetHP() <= 60)
        {
            weights[0] = 20; // 攻撃
            weights[1] = 25; // 防御
            weights[2] = 40; // 回復
        }
        else
        {
            weights[0] = 25; // 攻撃
            weights[1] = 35; // 防御
            weights[2] = 25; // 回復
        }
    }

    // 3試合目：最終ボス
    else if (battleNumber == 2)
    {
        if (playerStrategy == StrategyType::Attack)
        {
            weights[0] = 30;
            weights[1] = 30;
            weights[2] = (GetHP() <= 60) ? 20 : 5;
            weights[5] = 15;
        }
        else if (playerStrategy == StrategyType::Defense)
        {
            weights[0] = 45;
            weights[1] = 20;
            weights[2] = (GetHP() <= 60) ? 20 : 5;
            weights[5] = 15;
        }
        else
        {
            weights[0] = 30;
            weights[1] = 25;
            weights[2] = (GetHP() <= 60) ? 25 : 10;
            weights[5] = 15;
        }
    }

    // 使用できる能力の合計重み
    int totalWeight = 0;

    for (int i = 0; i < 6; i++)
    {
        EnemyAbility ability = static_cast<EnemyAbility>(i);

        if (HasAbility(ability) && !IsAbilitySealed(ability))
        {
            totalWeight += weights[i];
        }
    }

    if (totalWeight == 0)
    {
        return EnemyAbility::None;
    }

    // 重みに応じて行動を決定
    int random = GetRand(totalWeight - 1);

    for (int i = 0; i < 6; i++)
    {
        EnemyAbility ability = static_cast<EnemyAbility>(i);

        if (HasAbility(ability) && !IsAbilitySealed(ability))
        {
            random -= weights[i];

            if (random < 0)
            {
                return ability;
            }
        }
    }

    return EnemyAbility::None;
}

// 能力を持っているか確認
bool Enemy::HasAbility(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index < 0 || index >= 6)
    {
        return false;
    }

    return abilities[index];
}

// 能力が封印されているか確認
bool Enemy::IsAbilitySealed(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index < 0 || index >= 6)
    {
        return false;
    }

    return sealTurns[index] > 0;
}

// 能力を封印する
void Enemy::SealAbility(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index >= 0 && index < 6)
    {
        if (abilities[index] && sealTurns[index] == 0)
        {
            sealTurns[index] = 3;
        }
    }
}

// 封印ターンを1減らす
void Enemy::DecreaseSealTurns(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index >= 0 && index < 6)
    {
        if (sealTurns[index] > 0)
        {
            sealTurns[index]--;
        }
    }
}

// 封印残りターンを取得
int Enemy::GetSealTurns(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index < 0 || index >= 6)
    {
        return 0;
    }

    return sealTurns[index];
}