#include "DxLib.h"
#include "Game.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    // フルスクリーンで起動
    ChangeWindowMode(FALSE);
    SetGraphMode(1920,1080,32);
    SetUseDirectInputFlag(FALSE);

    // DxLibの初期化
    if (DxLib_Init() == -1)
    {
        return -1;
    }

    SetDrawScreen(DX_SCREEN_BACK);

    // Gameクラスのインスタンスを作成
    Game game;

    // ゲームの初期化
    game.Init();

    // メインループ
    while (ProcessMessage() == 0)
    {
        // ゲームの更新
        if (!game.Update())
        {
            break;
        }

        // ゲームの描画
        game.Draw();

        ScreenFlip();
    }

    // DxLibの終了
    DxLib_End();

    return 0;
}