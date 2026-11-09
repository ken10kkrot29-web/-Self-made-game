#pragma once
#include "Character.h"
#include "Strategy.h"

// 敵が持つ能力の種類
enum class EnemyAbility
{
    Attack,             // 攻撃
    Defense,            // 防御
    Recovery,           // 回復
    AttackBoost,        // 攻撃強化
    DefenseBoost,       // 防御強化
    CISpeedInterference,// CI速度妨害
    None                // 行動なし
};

class Enemy : public Character
{
private:
    // 能力を持っているか
    bool abilities[6];

    // 能力の封印残りターン
    int sealTurns[6];

public:
    void Init(int battleNumber);

    EnemyAbility SelectAction(StrategyType playerStrategy);
    int GetAttackPower();

    // 能力を持っているか確認
    bool HasAbility(EnemyAbility ability);

    // 能力が封印されているか確認
    bool IsAbilitySealed(EnemyAbility ability);

    // 能力を封印する
    void SealAbility(EnemyAbility ability);

    // 封印ターンを1減らす
    void DecreaseSealTurns(EnemyAbility ability);

    // 封印残りターンを取得
    int GetSealTurns(EnemyAbility ability);
};