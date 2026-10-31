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

    bool isBattleEnd;

public:
    void Init();

    void Update();

    Player& GetPlayer();
    Enemy& GetEnemy();

    //プレイヤーが選択した戦略を保存・取得
    void SetPlayerStrategy(StrategyType strategy);
    StrategyType GetPlayerStrategy();

    void StartTurn();

    //チャージインパクトの結果受け取る（結果に応じた処理を返す）
    void SetChargeImpactResult(bool playerWin);
    void ResolveBattle();

    bool IsBattleEnd();
};