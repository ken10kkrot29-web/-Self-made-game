#include "Character.h"

// ステータスを初期化
void Character::Init(int maxHP, int attack, int defense)
{
    maxHp = maxHP;
    hp = maxHp;

    attackPower = attack;
    defensePower = defense;
}

// ダメージを受ける
void Character::TakeDamage(int damage)
{
    if (damage < 0)
    {
        return;
    }

    hp -= damage;

    // HPがマイナスにならないようにする
    if (hp < 0)
    {
        hp = 0;
    }
}

// HPを回復する
void Character::Heal(int amount)
{
    if (amount < 0)
    {
        return;
    }

    hp += amount;

    // 最大HPを超えないようにする
    if (hp > maxHp)
    {
        hp = maxHp;
    }
}

// 攻撃力を取得
int Character::GetAttackPower()
{
    return attackPower;
}

// 防御力を取得
int Character::GetDefensePower()
{
    return defensePower;
}

// 現在のHPを取得
int Character::GetHP()
{
    return hp;
}

// 最大HPを取得
int Character::GetMaxHP()
{
    return maxHp;
}

// HPが0かどうかを調べる
bool Character::IsDefeated()
{
    return hp <= 0;
}