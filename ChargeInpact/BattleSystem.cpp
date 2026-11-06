#include "BattleSystem.h"

void BattleSystem::Init()
{
    player.Init();
    enemy.Init();

    playerStrategy = StrategyType::Attack;

    playerWinCI = false;
    enemyAction = 0;

    damage = 20;
    healAmount = 5;

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

// 勝敗処理
void BattleSystem::ResolveBattle()
{
    damage = 20;
    healAmount = 5;

    // 敵が防御を選んでいるか
    bool enemyDefending = (enemyAction == 1);

    // プレイヤーがCIに勝った場合
    if (playerWinCI)
    {
        switch (playerStrategy)
        {
        case StrategyType::Attack:

            // 敵が防御していたらダメージ半減
            if (enemyDefending)
            {
                enemy.TakeDamage(damage);
            }
            else
            {
                enemy.TakeDamage(damage * 2);
            }

            break;

        case StrategyType::Defense:
            // 防御なので攻撃しない
            break;

        case StrategyType::Trick:
            // HPを5回復
            player.Heal(healAmount);
            break;
        }

        // 敵が倒れていなければ敵が行動
        if (!enemy.IsDefeated())
        {
            if (enemyAction == 0)
            {
                int enemyDamage = enemy.GetAttackPower();

                // プレイヤーが防御していたら半減
                if (playerStrategy == StrategyType::Defense)
                {
                    enemyDamage /= 2;
                }

                player.TakeDamage(enemyDamage);
            }
            else if (enemyAction == 2)
            {
                // 敵の奇策：HP回復
                enemy.Heal(healAmount);
            }
        }
    }

    // 敵がCIに勝った場合
    else
    {
        // 先に敵が行動
        if (enemyAction == 0)
        {
            player.TakeDamage(enemy.GetAttackPower());
        }
        else if (enemyAction == 2)
        {
            enemy.Heal(healAmount);
        }

        // プレイヤーが倒れていなければ行動
        if (!player.IsDefeated())
        {
            switch (playerStrategy)
            {
            case StrategyType::Attack:

                // 敵が防御していたら半減
                if (enemyDefending)
                {
                    enemy.TakeDamage(damage / 2);
                }
                else
                {
                    enemy.TakeDamage(damage);
                }

                break;

            case StrategyType::Defense:
                // CI敗北なので防御効果なし
                break;

            case StrategyType::Trick:
                // CI敗北なので奇策失敗
                break;
            }
        }
    }

    // HPが0になったか確認
    if (player.IsDefeated() || enemy.IsDefeated())
    {
        isBattleEnd = true;
    }
}