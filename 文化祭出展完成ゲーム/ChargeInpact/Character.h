#pragma once

class Character
{
protected:
    // 現在のHP
    int hp;

    // 最大HP
    int maxHp;

    // 攻撃力
    int attackPower;

    // 防御力
    int defensePower;

public:
    // 初期化
    void Init(int maxHP, int attack = 10, int defense = 0);

    // ダメージを受ける
    void TakeDamage(int damage);

    // 回復
    void Heal(int amount);

    // 攻撃力を取得
    int GetAttackPower();

    // 防御力を取得
    int GetDefensePower();

    // 現在のHPを取得
    int GetHP();

    // 最大HPを取得
    int GetMaxHP();

    // HPが0かどうかを調べる
    bool IsDefeated();
};