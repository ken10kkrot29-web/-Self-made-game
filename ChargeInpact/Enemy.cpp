#include "Enemy.h"
#include "DxLib.h"
#include "Strategy.h"

// 敵の初期化
void Enemy::Init(int battleNumber)
{
    Character::Init(100);

    // すべての能力を一度リセット
    for (int i = 0; i < 6; i++)
    {
        abilities[i] = false;
        sealTurns[i] = 0;
    }

    // 1試合目：攻撃型
    if (battleNumber == 0)
    {
        abilities[0] = true;
        abilities[3] = true;
    }

    // 2試合目：防御型
    else if (battleNumber == 1)
    {
        abilities[1] = true;
        abilities[2] = true;
        abilities[4] = true;
    }

    // 3試合目：最終ボス
    else if (battleNumber == 2)
    {
        for (int i = 0; i < 6; i++)
        {
            abilities[i] = true;
        }
    }
}

// 敵の行動を選択する
EnemyAbility Enemy::SelectAction(StrategyType playerStrategy)
{
    // 使用できる能力を保存する配列
    EnemyAbility availableAbilities[6];

    // 使用できる能力の数
    int count = 0;

    // 6種類の能力を順番に確認
    for (int i = 0; i < 6; i++)
    {
        EnemyAbility ability = static_cast<EnemyAbility>(i);

        // 能力を持っていて、封印されていない場合
        if (HasAbility(ability) && !IsAbilitySealed(ability))
        {
            availableAbilities[count] = ability;
            count++;
        }
    }

    // 使用できる能力がない場合
    if (count == 0)
    {
        return EnemyAbility::None;
    }

    // 使用できる能力からランダムに選択
    int random = GetRand(count - 1);

    return availableAbilities[random];
}

// 攻撃力を取得
int Enemy::GetAttackPower()
{
    return 10;
}

// 能力を持っているか確認
bool Enemy::HasAbility(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    return abilities[index];
}

// 能力が封印されているか確認
bool Enemy::IsAbilitySealed(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    return sealTurns[index] > 0;
}

// 能力を封印する
void Enemy::SealAbility(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (abilities[index] && sealTurns[index] == 0)
    {
        sealTurns[index] = 3;
    }
}

// 封印ターンを1減らす
void Enemy::DecreaseSealTurns(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    if (sealTurns[index] > 0)
    {
        sealTurns[index]--;
    }
}

// 封印残りターンを取得
int Enemy::GetSealTurns(EnemyAbility ability)
{
    int index = static_cast<int>(ability);

    return sealTurns[index];
}