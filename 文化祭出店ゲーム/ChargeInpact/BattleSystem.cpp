#include "BattleSystem.h"

// 初期化
void BattleSystem::Init()
{
    player.Init();
    enemy.Init();

    playerStrategy = StrategyType::Attack;

    playerWinCI = false;
    enemyAction = EnemyAbility::None;

    // 敵の行動候補を初期化
    for (int i = 0; i < 3; i++)
    {
        enemyActionOptions[i] = EnemyAbility::None;
    }

    isTurnPrepared = false;

    trickChoice = 0;
    disableAction = EnemyAbility::None;

    sealUseCount = 0;

    isBattleEnd = false;

    // 戦闘履歴を初期化
    for (int i = 0; i < 3; i++)
    {
        battleLog[i] = L"";
    }
}

// プレイヤーの戦略を設定
void BattleSystem::SetPlayerStrategy(StrategyType strategy)
{
    playerStrategy = strategy;
}

// プレイヤーの戦略を取得
StrategyType BattleSystem::GetPlayerStrategy()
{
    return playerStrategy;
}

// 次のターンの敵の行動を事前決定
void BattleSystem::PrepareTurn()
{
    // 戦闘ごとに3種類の戦略に対する行動を決める
    for (int i = 0; i < 3; i++)
    {
        StrategyType strategy =
            static_cast<StrategyType>(i);

        enemyActionOptions[i] =
            enemy.SelectAction(strategy);
    }

    isTurnPrepared = true;
}

// プレイヤーを取得
Player& BattleSystem::GetPlayer()
{
    return player;
}

// 敵を取得
Enemy& BattleSystem::GetEnemy()
{
    return enemy;
}

// 戦闘が終了したか確認
bool BattleSystem::IsBattleEnd()
{
    return isBattleEnd;
}

// CIの勝敗を設定
void BattleSystem::SetChargeImpactResult(bool playerWin)
{
    playerWinCI = playerWin;
}

// ターン開始
void BattleSystem::StartTurn()
{
    // すでに戦闘が終了している場合は何もしない
    if (isBattleEnd)
    {
        return;
    }

    // 行動がまだ決まっていない場合
    if (!isTurnPrepared)
    {
        PrepareTurn();
    }

    // 選択した戦略に対応する敵の行動を取得
    int index = static_cast<int>(playerStrategy);

    if (index >= 0 && index < 3)
    {
        enemyAction = enemyActionOptions[index];
    }
    else
    {
        enemyAction = EnemyAbility::None;
    }

    // 敵がCI速度妨害を使用した場合
    if (enemyAction == EnemyAbility::CISpeedInterference)
    {
        AddBattleLog(L"敵がチャージインパクトを妨害！");
    }

    // 封印ターンを1減らす
    for (int i = 0; i < 4; i++)
    {
        EnemyAbility ability =
            static_cast<EnemyAbility>(i);

        enemy.DecreaseSealTurns(ability);
    }

    // このターンの行動決定は完了
    isTurnPrepared = false;
}

// 勝敗処理
void BattleSystem::ResolveBattle()
{
    // 戦闘終了後は処理しない
    if (isBattleEnd)
    {
        return;
    }

    // 敵が防御しているか
    bool enemyDefending =
        (enemyAction == EnemyAbility::Defense);

    // プレイヤーの攻撃ダメージ
    int playerDamage =
        player.GetAttackPower() - enemy.GetDefensePower();

    // 最低ダメージを5にする
    if (playerDamage < 5)
    {
        playerDamage = 5;
    }

    // 敵の攻撃ダメージ
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

            // CI勝利で攻撃力を2倍
            playerDamage *= 2;

            // 敵が防御していた場合
            if (enemyDefending)
            {
                playerDamage /= 2;

                AddBattleLog(L"敵は攻撃を防御した！");
            }

            // 敵にダメージ
            enemy.TakeDamage(playerDamage);

            AddBattleLog(
                std::wstring(L"プレイヤーが攻撃！敵に")
                + std::to_wstring(playerDamage)
                + L"ダメージ！"
            );

            break;

        case StrategyType::Defense:

            // 防御の場合は攻撃しない
            break;

        case StrategyType::Trick:

            // HP回復
            if (trickChoice == 0)
            {
                player.Heal(15);

                AddBattleLog(L"プレイヤーが回復した！");
            }

            // 能力封印
            else if (trickChoice == 1)
            {
                // 封印は最大5回
                if (sealUseCount < 5)
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

        // 敵が倒れていなければ敵の行動
        if (!enemy.IsDefeated())
        {
            switch (enemyAction)
            {
            case EnemyAbility::Attack:

                // プレイヤーが防御している場合
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

            // プレイヤーが防御している場合
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

                // 敵が防御している場合
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

                // 防御の場合は攻撃しない
                break;

            case StrategyType::Trick:

                // CIに負けた場合、奇策は発動しない
                break;
            }
        }
    }

    // HPが0になったか確認
    if (player.IsDefeated() || enemy.IsDefeated())
    {
        isBattleEnd = true;

        if (enemy.IsDefeated())
        {
            AddBattleLog(L"敵を倒した！プレイヤーの勝利！");
        }
        else
        {
            AddBattleLog(L"プレイヤーが倒れた！敵の勝利！");
        }
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

// 封印する能力を設定
void BattleSystem::SetDisableAction(EnemyAbility action)
{
    disableAction = action;
}

// 封印の使用回数を取得
int BattleSystem::GetSealUseCount()
{
    return sealUseCount;
}

// 戦闘履歴を追加
void BattleSystem::AddBattleLog(const std::wstring& message)
{
    // 古い履歴をずらす
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

// 敵の行動を取得
const wchar_t* BattleSystem::GetEnemyActionHint(StrategyType strategy)
{
    int index = static_cast<int>(strategy);

    if (index < 0 || index >= 3)
    {
        return L"敵の行動は決まっていない";
    }

    EnemyAbility action = enemyActionOptions[index];

    switch (action)
    {
    case EnemyAbility::Attack:
        return L"敵は「攻撃」を選択している！";

    case EnemyAbility::Defense:
        return L"敵は「防御」を選択している！";

    case EnemyAbility::Recovery:
        return L"敵は「回復」を選択している！";

    case EnemyAbility::CISpeedInterference:
        return L"敵は「CI妨害」を選択している！";

    default:
        return L"敵の行動は決まっていない";
    }
}