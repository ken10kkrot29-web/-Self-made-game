#include "Character.h"

// HPを初期化
void Character::Init(int maxHP)
{
    maxHp = maxHP;
    hp = maxHp;
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