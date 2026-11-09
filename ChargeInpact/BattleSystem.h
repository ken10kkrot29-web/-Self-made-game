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

    EnemyAbility enemyAction;

    // 敵の攻撃強化状態
    bool enemyAttackBoost;

    // 敵の防御強化状態
    bool enemyDefenseBoost;

    // 奇策の選択（0：回復、1：行動封印）
    int trickChoice;

    // 封印する敵の行動（0：攻撃、1：防御、2：奇策）
    EnemyAbility disableAction;

    // 封印の残りターン
    int disableTurns;

    // この試合で能力封印を使用した回数
    int sealUseCount;

    bool isBattleEnd;

public:
    void Init(int battleNumber = 0);

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
    void SetDisableAction(EnemyAbility action);

    // 敵がCI速度妨害を使用しているか
    bool IsCISpeedInterferenceActive();

    // この試合で封印を使用した回数を取得
    int GetSealUseCount();

    bool IsBattleEnd();
};