#pragma once

#include <string>

#include "Player.h"
#include "Enemy.h"
#include "Strategy.h"

class BattleSystem
{
private:

    // プレイヤー
    Player player;

    // 敵
    Enemy enemy;

    // プレイヤーの戦略
    StrategyType playerStrategy;

    // CIの勝敗
    bool playerWinCI;

    // 実際に使用する敵の行動
    EnemyAbility enemyAction;

    // 戦略ごとに事前決定した敵の行動
    // 0：攻撃を選んだ場合
    // 1：防御を選んだ場合
    // 2：奇策を選んだ場合
    EnemyAbility enemyActionOptions[3];

    // 敵の行動を事前決定したか
    bool isTurnPrepared;

    // 奇策の選択
    // 0：回復
    // 1：能力封印
    int trickChoice;

    // 封印する敵の能力
    EnemyAbility disableAction;

    // この戦闘で能力封印を使用した回数
    int sealUseCount;

    // 戦闘が終了したか
    bool isBattleEnd;

    // 戦闘履歴（直近3件）
    std::wstring battleLog[3];

    // 戦闘履歴を追加する
    void AddBattleLog(const std::wstring& message);

public:

    // 初期化
    void Init();

    // プレイヤーと敵を取得
    Player& GetPlayer();
    Enemy& GetEnemy();

    // プレイヤーの戦略
    void SetPlayerStrategy(StrategyType strategy);
    StrategyType GetPlayerStrategy();

    // 敵の行動予測ヒントを取得
    const wchar_t* GetEnemyActionHint(StrategyType strategy);

    // 次のターンの敵の行動を事前決定
    void PrepareTurn();

    // ターン開始
    void StartTurn();

    // CIの結果を受け取る
    void SetChargeImpactResult(bool playerWin);

    // 戦闘処理
    void ResolveBattle();

    // 戦闘終了判定
    bool IsBattleEnd();

    // 奇策の種類を設定
    void SetTrickChoice(int choice);

    // 封印する敵の能力を設定
    void SetDisableAction(EnemyAbility action);

    // 敵がCI速度妨害を使用しているか
    bool IsCISpeedInterferenceActive();

    // 封印を使用した回数を取得
    int GetSealUseCount();

    // 戦闘履歴を取得
    const wchar_t* GetBattleLog(int index);
};