//画面表示、キー入力、画面切り替え
#include "DxLib.h"
#include "Game.h"

// ゲームの初期化
void Game::Init()
{
    scene = 0;
    select = 0;
    selectedStrategy = 0;

    oldEnter = false;
    oldUp = false;
    oldDown = false;
    oldEscape = false;
}

// ゲームの更新
bool Game::Update()
{
    // キー入力を取得
    int enter = CheckHitKey(KEY_INPUT_RETURN);
    int up = CheckHitKey(KEY_INPUT_UP);
    int down = CheckHitKey(KEY_INPUT_DOWN);
    int escape = CheckHitKey(KEY_INPUT_ESCAPE);

    // ESCキーの処理
    if (escape == 1 && oldEscape == 0)
    {
        if (scene == 0)
        {
            //ゲーム終了で使われる
            return false;
        }
        else
        {
            scene = 0;
        }
    }

    // タイトル画面
    if (scene == 0)
    {
        if (enter == 1 && oldEnter == 0)
        {
            scene = 1;
        }
    }

    // 戦術選択画面
    else if (scene == 1)
    {
        // 上キー
        if (up == 1 && oldUp == 0)
        {
            select--;

            if (select < 0)
            {
                select = 2;
            }
        }

        // 下キー
        if (down == 1 && oldDown == 0)
        {
            select++;

            if (select > 2)
            {
                select = 0;
            }
        }

        // ENTERキーで戦術を確定
        if (enter == 1 && oldEnter == 0)
        {
            selectedStrategy = select;
            scene = 2;
        }
    }

    // 戦術の結果画面
    else if (scene == 2)
    {
        if (enter == 1 && oldEnter == 0)
        {
            scene = 1;
        }
    }

    // 今回のキー状態を保存
    oldEnter = enter;
    oldUp = up;
    oldDown = down;
    oldEscape = escape;

    return true;
}

// ゲームの描画
void Game::Draw()
{
    ClearDrawScreen();

    // 文字の大きさを設定
    SetFontSize(36);

    // タイトル画面
    if (scene == 0)
    {
        DrawString(660, 225,
            "STRATEGY BATTLE",
            GetColor(255, 255, 255));

        DrawString(660, 405,
            "Press ENTER to start",
            GetColor(255, 255, 0));
    }

    // 戦術選択画面
    else if (scene == 1)
    {
        DrawString(300, 180,
            "Select your strategy",
            GetColor(255, 255, 255));

        DrawString(360, 360,
            select == 0 ? "> Attack" : "  Attack",
            GetColor(255, 100, 100));

        DrawString(360, 472,
            select == 1 ? "> Defense" : "  Defense",
            GetColor(100, 200, 255));

        DrawString(360, 585,
            select == 2 ? "> Trick" : "  Trick",
            GetColor(255, 220, 100));

        DrawString(300, 788,
            "UP / DOWN : Select",
            GetColor(200, 200, 200));

        DrawString(300, 855,
            "ESC : Back to Title",
            GetColor(200, 200, 200));
    }

    // 戦術の結果画面
    else if (scene == 2)
    {
        DrawString(300, 225,
            "Strategy Confirmed!",
            GetColor(255, 255, 255));

        if (selectedStrategy == 0)
        {
            DrawString(300, 405,
                "ATTACK SELECTED",
                GetColor(255, 100, 100));
        }
        else if (selectedStrategy == 1)
        {
            DrawString(300, 405,
                "DEFENSE SELECTED",
                GetColor(100, 200, 255));
        }
        else if (selectedStrategy == 2)
        {
            DrawString(300, 405,
                "TRICK SELECTED",
                GetColor(255, 220, 100));
        }

        DrawString(300, 630,
            "ENTER : Back to Select",
            GetColor(200, 200, 200));
    }
}
