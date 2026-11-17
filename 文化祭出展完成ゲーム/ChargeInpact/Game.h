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

    // 現在の画面
    // 0：タイトル
    // 1：戦略選択
    // 2：CI
    // 3：戦闘結果
    // 5：CI補助選択
    // 6：奇策選択
    // 7：封印する能力の選択
    int scene = 0;

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
    bool resultSent = false;

    // 前回のキー入力状態
    bool oldEnter = false;
    bool oldUp = false;
    bool oldDown = false;
    bool oldEscape = false;
    bool oldTab = false;
    bool oldR = false;

    // 敵の詳細情報を表示するか
    bool showEnemyInfo = false;

    // タイトル画面の背景画像
    int titleBgHandle = -1;

    // 戦闘画面用背景画像
    int battleBgHandle = -1;

    // あいこが発生したか
    bool isTie = false;

    // あいこ後の再挑戦を待っているか
    bool isTieWaiting = false;

    // あいこになった時刻
    int tieWaitStartTime = 0;

    // 奇策の選択
    int trickSelect = 0;

    // 封印する能力の選択
    int sealSelect = 0;

    // 封印できる能力の一覧
    EnemyAbility sealCandidates[4];

    // 封印できる能力の数
    int sealCandidateCount = 0;

    // 結果画面に入った時刻
    int resultStartTime = 0;

    // 封印できる能力を調べる
    void CreateSealCandidates();

    // チャージインパクトを開始する
    void StartChargeImpact();

    // 能力封印が使用できるか確認
    bool CanUseSeal();
};