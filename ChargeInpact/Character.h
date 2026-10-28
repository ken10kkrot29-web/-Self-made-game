#pragma once

class Character
{
protected:
	// 現在のHP
	int hp;

	// 最大HP
	int maxHp;

public:
	// 初期化
	void Init(int maxHP);

	// ダメージを受ける
	void TakeDamage(int damage);

	// HPを回復する
	void Heal(int amount);

	// 現在のHPを取得する
	int GetHP();

	// 最大HPを取得する
	int GetMaxHP();

	// HPが0かどうかを調べる
	bool IsDefeated();
};
