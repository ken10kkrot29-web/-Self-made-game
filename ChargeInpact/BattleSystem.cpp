#include "BattleSystem.h"

void BattleSystem::Init(int battleNumber)
{
    player.Init();
    enemy.Init(battleNumber);

    playerStrategy = StrategyType::Attack;

    playerWinCI = false;
    enemyAction = EnemyAbility::None;

    damage = 15;
    healAmount = 10;

    isBattleEnd = false;

    enemyAttackBoost = false;
    enemyDefenseBoost = false;

    trickChoice = 0;
    disableAction = EnemyAbility::None;
    disableTurns = 0;
    sealUseCount = 0;
    // 戦闘履歴を初期化
    for (int i = 0; i < 3; i++)
    {
        battleLog[i] = L"";
    }
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

    // 敵がCI速度妨害を使用した場合
    if (enemyAction == EnemyAbility::CISpeedInterference)
    {
        AddBattleLog(L"敵がチャージインパクトを妨害！");
    }

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
    // 敵が防御しているか
    bool enemyDefending =
        (enemyAction == EnemyAbility::Defense);

    // プレイヤーの攻撃ダメージを計算
    int playerDamage =
        player.GetAttackPower() - enemy.GetDefensePower();

    // 最低ダメージを5にする
    if (playerDamage < 5)
    {
        playerDamage = 5;
    }

    // 敵の攻撃ダメージを計算
    int enemyDamage =
        enemy.GetAttackPower() - player.GetDefensePower();

    // 最低ダメージを5にする
    if (enemyDamage < 5)
    {
        enemyDamage = 5;
    }

    // プレイヤーがCIに勝った場合
    if (playerWinCI)
    {
        switch (playerStrategy)
        {
        case StrategyType::Attack:

            // CI勝利で攻撃力を2倍にする
            playerDamage *= 2;

            // 敵が防御していた場合はダメージを半減
            if (enemyDefending)
            {
                playerDamage /= 2;
                AddBattleLog(L"敵は攻撃を防御した！");
            }

            enemy.TakeDamage(playerDamage);

            AddBattleLog(
                std::wstring(L"プレイヤーが攻撃！敵に")
                + std::to_wstring(playerDamage)
                + L"ダメージ！"
            );

            break;

        case StrategyType::Defense:
            break;

        case StrategyType::Trick:

            // 回復を選択した場合
            if (trickChoice == 0)
            {
                player.Heal(15);
                AddBattleLog(L"プレイヤーが回復した！");
            }

            // 能力封印を選択した場合
            else if (trickChoice == 1)
            {
                if (sealUseCount < 2)
                {
                    if (enemy.HasAbility(disableAction) &&
                        !enemy.IsAbilitySealed(disableAction))
                    {
                        enemy.SealAbility(disableAction);
                        sealUseCount++;

                        AddBattleLog(L"敵の能力を封印した！");
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

                // プレイヤーが防御していた場合
                if (playerStrategy == StrategyType::Defense)
                {
                    enemyDamage /= 2;
                }

                player.TakeDamage(enemyDamage);

                AddBattleLog(
                    std::wstring(L"敵が攻撃！プレイヤーに")
                    + std::to_wstring(enemyDamage)
                    + L"ダメージ！"
                );

                break;

            case EnemyAbility::Recovery:
                enemy.Heal(15);
                AddBattleLog(L"敵が回復した！");
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

            AddBattleLog(
                std::wstring(L"敵が攻撃！プレイヤーに")
                + std::to_wstring(enemyDamage)
                + L"ダメージ！"
            );

            break;

        case EnemyAbility::Recovery:
            enemy.Heal(15);
            AddBattleLog(L"敵が回復した！");
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

                // 敵が防御していた場合
                if (enemyDefending)
                {
                    playerDamage /= 2;
                    AddBattleLog(L"敵は攻撃を防御した！");
                }

                enemy.TakeDamage(playerDamage);

                AddBattleLog(
                    std::wstring(L"プレイヤーが攻撃！敵に")
                    + std::to_wstring(playerDamage)
                    + L"ダメージ！"
                );

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

// 戦闘履歴を追加する
void BattleSystem::AddBattleLog(const std::wstring& message)
{
    // 古い履歴を1つずつずらす
    battleLog[0] = battleLog[1];
    battleLog[1] = battleLog[2];

    // 最新の履歴を保存
    battleLog[2] = message;
}

// 戦闘履歴を取得
const wchar_t* BattleSystem::GetBattleLog(int index)
{
    if (index < 0 || index >= 3)
    {
        return L"";
    }

    return battleLog[index].c_str();
}

// 敵の攻撃強化状態を取得
bool BattleSystem::IsEnemyAttackBoostActive()
{
    return enemyAttackBoost;
}

// 敵の防御強化状態を取得
bool BattleSystem::IsEnemyDefenseBoostActive()
{
    return enemyDefenseBoost;
}