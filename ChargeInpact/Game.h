//Game.h：ゲーム全体を管理するクラスの宣言
#pragma once
#include "BattleSystem.h"
#include "ChargeInpact.h"

class Game
{
public:
    // ゲームの初期化
    void Init();

    // ゲームの更新
    bool Update();

    // ゲームの描画
    void Draw();

private:
    // 画面の状態
    int scene = 0;

    // 選択中の戦術
    int select = 0;

    // 確定した戦術
    int selectedStrategy = 0;

    // 戦闘システム
    BattleSystem battle;

    // チャージインパクト
    ChargeInpact chargeInpact;

    // CIの結果を戦闘処理に渡したか
    bool resultSent;

    // 前回のキー入力状態
    bool oldEnter = false;
    bool oldUp = false;
    bool oldDown = false;
    bool oldEscape = false;
};
