#include "BattleSystem.h"

void BattleSystem::Init()
{
    player.Init();
    enemy.Init();
    playerStrategy = StrategyType::Attack;

    isBattleEnd = false;
}

void BattleSystem::SetPlayerStrategy(StrategyType strategy)
{
    playerStrategy = strategy;
}

StrategyType BattleSystem::GetPlayerStrategy()
{
    return playerStrategy;
}

void BattleSystem::Update()
{
    if (player.IsDefeated() || enemy.IsDefeated())
    {
        isBattleEnd = true;
    }
}

Player& BattleSystem::GetPlayer()
{
    return player;
}

Enemy& BattleSystem::GetEnemy()
{
    return enemy;
}

bool BattleSystem::IsBattleEnd()
{
    return isBattleEnd;
}

void BattleSystem::SetChargeImpactResult(bool playerWin)
{
    playerWinCI = playerWin;
}

//プレイヤーが戦略を選択した後
void BattleSystem::StartTurn()
{
    enemyAction = enemy.SelectAction(playerStrategy);
}

//勝敗処理
void BattleSystem::ResolveBattle()
{
    damage = 20;
    healAmount = 5;

    // プレイヤーの戦略に応じた処理
    switch (playerStrategy)
    {
    case StrategyType::Attack:

        if (playerWinCI)
        {
            enemy.TakeDamage(damage * 2);
        }
        else if (enemyAction == 0)
        {
            player.TakeDamage(enemy.GetAttackPower());
        }

        break;

    case StrategyType::Defense:

        if (enemyAction == 0)
        {
            int enemyDamage = enemy.GetAttackPower();

            if (playerWinCI)
            {
                enemyDamage /= 2;
            }

            player.TakeDamage(enemyDamage);
        }

        break;

    case StrategyType::Trick:

        if (playerWinCI)
        {
            player.Heal(healAmount);
        }
        else if (enemyAction == 0)
        {
            player.TakeDamage(enemy.GetAttackPower());
        }

        break;
    }

    // HPが0になったか確認
    if (player.IsDefeated() || enemy.IsDefeated())
    {
        isBattleEnd = true;
    }
}