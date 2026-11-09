#include "BattleSystem.h"

void BattleSystem::Init(int battleNumber)
{
    player.Init();
    enemy.Init(battleNumber);

    playerStrategy = StrategyType::Attack;

    playerWinCI = false;
    enemyAction = EnemyAbility::None;

    damage = 20;
    healAmount = 5;

    isBattleEnd = false;

    enemyAttackBoost = false;
    enemyDefenseBoost = false;

    trickChoice = 0;
    disableAction = EnemyAbility::None;
    disableTurns = 0;
    sealUseCount = 0;
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

// プレイヤーが戦略を選択した後
void BattleSystem::StartTurn()
{
    // 敵の行動を選択
    enemyAction = enemy.SelectAction(playerStrategy);

    // 6種類の能力の封印ターンを減らす
    for (int i = 0; i < 6; i++)
    {
        EnemyAbility ability = static_cast<EnemyAbility>(i);

        enemy.DecreaseSealTurns(ability);
    }
}

// 勝敗処理
void BattleSystem::ResolveBattle()
{
    damage = 20;
    healAmount = 5;

    // 敵が防御しているか
    bool enemyDefending =
        (enemyAction == EnemyAbility::Defense);

    // 敵の攻撃力
    int enemyDamage = enemy.GetAttackPower();

    // 攻撃強化が有効なら攻撃力を5増やす
    if (enemyAttackBoost)
    {
        enemyDamage += 10;
    }

    // プレイヤーがCIに勝った場合
    if (playerWinCI)
    {
        switch (playerStrategy)
        {
        case StrategyType::Attack:

            int playerDamage;

            // 敵が防御していた場合
            if (enemyDefending)
            {
                playerDamage = damage;
            }
            else
            {
                playerDamage = damage * 2;
            }

            // 敵が防御強化していた場合
            if (enemyDefenseBoost)
            {
                playerDamage -= 5;

                if (playerDamage < 0)
                {
                    playerDamage = 0;
                }

                enemyDefenseBoost = false;
            }

            enemy.TakeDamage(playerDamage);

            // 防御強化によるダメージ軽減
            if (enemyDefenseBoost && !enemy.IsDefeated())
            {
                // 今回は敵の防御強化を次の攻撃まで保持
            }

            break;

        case StrategyType::Defense:
            break;

        case StrategyType::Trick:

            // 回復を選択した場合
            if (trickChoice == 0)
            {
                player.Heal(15);
            }

            // 能力封印を選択した場合
            else if (trickChoice == 1)
            {
                // 封印回数が2回未満の場合
                if (sealUseCount < 2)
                {
                    // 敵が持っていて、封印されていない能力か確認
                    if (enemy.HasAbility(disableAction) &&
                        !enemy.IsAbilitySealed(disableAction))
                    {
                        enemy.SealAbility(disableAction);

                        // 封印使用回数を増やす
                        sealUseCount++;
                    }
                }
            }

            break;
        }

        // 敵が倒れていなければ敵が行動
        if (!enemy.IsDefeated())
        {
            switch (enemyAction)
            {
            case EnemyAbility::Attack:

                if (playerStrategy == StrategyType::Defense)
                {
                    enemyDamage /= 2;
                }

                player.TakeDamage(enemyDamage);
                break;

            case EnemyAbility::Recovery:
                enemy.Heal(15);
                break;

            case EnemyAbility::AttackBoost:
                enemyAttackBoost = true;
                break;

            case EnemyAbility::DefenseBoost:
                enemyDefenseBoost = true;
                break;

            default:
                break;
            }
        }
    }

    // 敵がCIに勝った場合
    else
    {
        // 先に敵が行動
        switch (enemyAction)
        {
        case EnemyAbility::Attack:

            if (playerStrategy == StrategyType::Defense)
            {
                enemyDamage /= 2;
            }

            player.TakeDamage(enemyDamage);
            
            // 攻撃強化を解除
            enemyAttackBoost = false;

            break;

        case EnemyAbility::Recovery:
            enemy.Heal(15);
            break;

        case EnemyAbility::AttackBoost:
            enemyAttackBoost = true;
            break;

        case EnemyAbility::DefenseBoost:
            enemyDefenseBoost = true;
            break;

        default:
            break;
        }

        // プレイヤーが倒れていなければ行動
        if (!player.IsDefeated())
        {
            switch (playerStrategy)
            {
            case StrategyType::Attack:

                int playerDamage;

                // 敵が防御していた場合
                if (enemyDefending)
                {
                    playerDamage = damage / 2;
                }
                else
                {
                    playerDamage = damage;
                }

                // 敵が防御強化していた場合
                if (enemyDefenseBoost)
                {
                    playerDamage -= 5;

                    if (playerDamage < 0)
                    {
                        playerDamage = 0;
                    }

                    enemyDefenseBoost = false;
                }

                enemy.TakeDamage(playerDamage);

                break;

            case StrategyType::Defense:
                break;

            case StrategyType::Trick:
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

// 敵がCI速度妨害を使用しているか
bool BattleSystem::IsCISpeedInterferenceActive()
{
    return enemyAction == EnemyAbility::CISpeedInterference;
}

// 奇策の種類を設定
void BattleSystem::SetTrickChoice(int choice)
{
    trickChoice = choice;
}

// 封印する敵の能力を設定
void BattleSystem::SetDisableAction(EnemyAbility action)
{
    disableAction = action;
}

// 封印を使用した回数を取得
int BattleSystem::GetSealUseCount()
{
    return sealUseCount;
}