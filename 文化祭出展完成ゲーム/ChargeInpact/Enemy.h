#pragma once
#include "Character.h"
#include "Strategy.h"

// 敵の能力の種類
enum class EnemyAbility
{
    Attack,              // 攻撃
    Defense,             // 防御
    Recovery,            // 回復
    CISpeedInterference, // CI速度妨害
    None                 // 行動なし（内部処理用）
};

class Enemy : public Character
{
private:
    bool abilities[4];
    int sealTurns[4];

public:
    void Init();

    EnemyAbility SelectAction(StrategyType playerStrategy);

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