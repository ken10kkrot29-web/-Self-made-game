//バーの移動・停止・制限時間などを実装
#include "DxLib.h"
#include "ChargeInpact.h"

// ゲージの左端,右端（X軸、Y軸）座標
const int BAR_LEFT = 100;
const int BAR_RIGHT = 540;
const int BAR_Y = 240;
const int BAR_WIDTH = BAR_RIGHT - BAR_LEFT;

//ゲージの初期化内容をここに記載
void ChargeInpact::Init()
{
    barX = (float)BAR_LEFT;
    direction = 1;
    speed = 3.0f;
    isStopped = false;
    //GetNowCount() = 動後の経過時間をミリ秒単位表す関数
    //GetNowCount() = DxLibに最初から用意されている関数
    startTime = GetNowCount();
    //残り時間
    remainingTime = 10.0f;
    score = 0;
}

void ChargeInpact::Update()
{
    //停止ボタンを押していたらそれ以上の処理をしない
    if (isStopped)
    {
        return;
    }
    // 制限時間は10秒
    if (GetNowCount() - startTime >= 10000)
    {
            
        isStopped = true;
        score = 0;
        return;
    }

    // スペースキーが押されたら停止
    if (CheckHitKey(KEY_INPUT_SPACE))
    {
        isStopped = true;

        // ゲージの位置を0～100のスコアに変換
        score = (int)((barX - BAR_LEFT) * 100.0f / BAR_WIDTH);

        if (score < 0)
        {
            score = 0;
        }

        if (score > 100)
        {
            score = 100;
        }

        return;
    }

    // ゲージを移動する
    barX += speed * direction;

    // 右端に到達したら左向きにする
    if (barX >= BAR_RIGHT)
    {
        barX = (float)BAR_RIGHT;
        direction = -1;
    }

    // 左端に到達したら右向きにする
    if (barX <= BAR_LEFT)
    {
        barX = (float)BAR_LEFT;
        direction = 1;
    }

}

void ChargeInpact::Draw()
{
    // ゲージの背景
    DrawBox(BAR_LEFT, BAR_Y - 10, BAR_RIGHT, BAR_Y + 10,
        GetColor(80, 80, 80), TRUE);

        // スコア50の位置
        DrawLine((BAR_LEFT + BAR_RIGHT) / 2, BAR_Y - 25,
            (BAR_LEFT + BAR_RIGHT) / 2, BAR_Y + 25,
            GetColor(0, 255, 0));

    // スコア75の位置
    DrawLine(BAR_LEFT + BAR_WIDTH * 3 / 4, BAR_Y - 25,
        BAR_LEFT + BAR_WIDTH * 3 / 4, BAR_Y + 25,
        GetColor(255, 255, 0));

    // 現在のゲージ位置
    DrawBox((int)barX - 3, BAR_Y - 18,
        (int)barX + 3, BAR_Y + 18,
        GetColor(255, 50, 50), TRUE);

    // スコア表示
    DrawFormatString(100, 290, GetColor(255, 255, 255),
        "Score:%d", score);

    // 残り時間の状態
    if (isStopped)
    {
        DrawString(100, 320, "STOP!", GetColor(255, 255, 255));
    }

}

bool ChargeInpact::IsStopped()
{
    return isStopped;
}

int ChargeInpact::GetScore()
{
    return score;
}
