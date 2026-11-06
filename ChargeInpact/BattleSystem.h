#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Strategy.h"

class BattleSystem
{
private:
    Player player;
    Enemy enemy;
    StrategyType playerStrategy;

    bool playerWinCI;
    int damage;
    int healAmount;

    int enemyAction;

    // 奇策の選択（0：回復、1：行動封印）
    int trickChoice;

    // 封印する敵の行動（0：攻撃、1：防御、2：奇策）
    int disableAction;

    // 封印の残りターン
    int disableTurns;

    bool isBattleEnd;

public:
    void Init();

    void Update();

    Player& GetPlayer();
    Enemy& GetEnemy();

    // プレイヤーが選択した戦略を保存・取得
    void SetPlayerStrategy(StrategyType strategy);
    StrategyType GetPlayerStrategy();

    void StartTurn();

    // チャージインパクトの結果を受け取り、戦闘処理に反映する
    void SetChargeImpactResult(bool playerWin);
    void ResolveBattle();

    // 奇策の種類を設定
    void SetTrickChoice(int choice);

    // 封印する敵の行動を設定
    void SetDisableAction(int action);

    bool IsBattleEnd();
};