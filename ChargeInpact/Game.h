//Game.h：ゲーム全体を管理するクラスの宣言
#pragma once

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

    // 前回のキー入力状態
    int oldEnter = 0;
    int oldUp = 0;
    int oldDown = 0;
    int oldEscape = 0;
};
