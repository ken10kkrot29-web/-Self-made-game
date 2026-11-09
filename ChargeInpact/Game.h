// Game.h：ゲーム全体を管理するクラスの宣言
#pragma once
#include "BattleSystem.h"
#include "ChargeInpact.h"
#include "Tournament.h"

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
    // 現在の画面
    int scene = 0;

    Tournament tournament;

    // 選択中の戦略
    int select = 0;

    // CI補助の選択
    int assistSelect = 1;

    // CI補助を使用するか
    bool useCIAssist = false;

    // 決定した戦略
    int selectedStrategy = 0;

    // 戦闘システム
    BattleSystem battle;

    // チャージインパクト
    ChargeInpact chargeInpact;

    // CIの結果を戦闘システムに渡したか
    bool resultSent;

    // 前回のキー入力状態
    bool oldEnter = false;
    bool oldUp = false;
    bool oldDown = false;
    bool oldEscape = false;

    // あいこが発生したか
    bool isTie;

    // あいこ後の再挑戦を待っているか
    bool isTieWaiting;

    // あいこになった時刻
    int tieWaitStartTime;

    // 奇策の選択
    int trickSelect = 0;

    // 封印する能力の選択
    int sealSelect = 0;

    // 封印できる能力の一覧
    EnemyAbility sealCandidates[6];

    // 封印できる能力の数
    int sealCandidateCount = 0;

    // 封印できる能力を調べる
    void CreateSealCandidates();

    // チャージインパクトを開始する
    void StartChargeImpact();
};