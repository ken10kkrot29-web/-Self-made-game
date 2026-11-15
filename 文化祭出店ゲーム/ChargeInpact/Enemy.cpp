#include "Enemy.h"
#include "DxLib.h"
#include "Strategy.h"

// 敵の初期化
void Enemy::Init()
{
    // すべての能力をリセット
    for (int i = 0; i < 4; i++)
    {
        abilities[i] = false;
        sealTurns[i] = 0;
    }

    // 敵のステータス
    Character::Init(100, 40, 15);

    // 4種類すべての能力を持たせる
    for (int i = 0; i < 4; i++)
    {
        abilities[i] = true;
    }
}

// 敵の行動を選択する
EnemyAbility Enemy::SelectAction(StrategyType playerStrategy)
{
    int weights[4] = { 0, 0, 0, 0 };

    // 基本の行動確率
    weights[0] = 40; // 攻撃
    weights[1] = 25; // 防御
    weights[2] = 15; // 回復
    weights[3] = 20;  // CI速度妨害

    // プレイヤーが攻撃型なら防御を重視
    if (playerStrategy == StrategyType::Attack)
    {
        weights[1] += 15;
    }

    // プレイヤーが防御型なら攻撃を重視
    else if (playerStrategy == StrategyType::Defense)
    {
        weights[0] += 15;
    }

    // HPが少ない場合は回復を重視
    if (GetHP() <= 60)
    {
        weights[2] += 30;
    }

    // 使用できる能力の合計重み
    int totalWeight = 0;

    for (int i = 0; i < 4; i++)
    {
        EnemyAbility ability = static_cast<EnemyAbility>(i);

        if (HasAbility(ability) && !IsAbilitySealed(ability))
        {
            totalWeight += weights[i];
        }
    }

    // 使用できる能力がない場合
    if (totalWeight == 0)
    {
        return EnemyAbility::None;
    }

    // 重みに応じて行動を決定
    int random = GetRand(totalWeight - 1);

    for (int i = 0; i < 4; i++)
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

    if (index < 0 || index >= 4)
    {
        return false;
    }

    return abilities[index];
}

// 能力が封印されているか確認
bool Enemy::IsAbilitySealed(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index < 0 || index >= 4)
    {
        return false;
    }

    return sealTurns[index] > 0;
}

// 能力を封印する
void Enemy::SealAbility(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (index >= 0 && index < 4)
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

    if (index >= 0 && index < 4)
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

    if (index < 0 || index >= 4)
    {
        return 0;
    }

    return sealTurns[index];
}