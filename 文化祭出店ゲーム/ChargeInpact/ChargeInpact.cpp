//バーの移動・停止・制限時間などを実装
#include "DxLib.h"
#include "ChargeInpact.h"

// プレイヤーゲージの座標
const int BAR_LEFT = 80;
const int BAR_RIGHT = 620;
const int BAR_Y = 420;
const int BAR_WIDTH = BAR_RIGHT - BAR_LEFT;

// CPUゲージの座標
const int CPU_BAR_LEFT = 1180;
const int CPU_BAR_RIGHT = 1720;
const int CPU_BAR_WIDTH = CPU_BAR_RIGHT - CPU_BAR_LEFT;


// 2色を混ぜて中間色を作る
int BlendColor(int start, int end, float rate)
{
    int r1 = (start >> 16) & 255;
    int g1 = (start >> 8) & 255;
    int b1 = start & 255;

    int r2 = (end >> 16) & 255;
    int g2 = (end >> 8) & 255;
    int b2 = end & 255;

    int r = r1 + (int)((r2 - r1) * rate);
    int g = g1 + (int)((g2 - g1) * rate);
    int b = b1 + (int)((b2 - b1) * rate);

    return GetColor(r, g, b);
}

// スコアに応じたゲージの色を取得
int GetCIGaugeColor(float percent)
{
    int yellow = GetColor(255, 235, 30);
    int orange = GetColor(255, 150, 0);
    int red = GetColor(255, 45, 20);
    int gold = GetColor(255, 220, 50);

    if (percent >= 99.5f)
    {
        return gold;
    }

    if (percent <= 50.0f)
    {
        return BlendColor(
            GetColor(255, 255, 40),
            yellow,
            percent / 50.0f
        );
    }
    else if (percent <= 75.0f)
    {
        return BlendColor(
            yellow,
            orange,
            (percent - 50.0f) / 25.0f
        );
    }
    else
    {
        return BlendColor(
            orange,
            red,
            (percent - 75.0f) / 24.0f
        );
    }
}

// グラデーションゲージを描画
void DrawCIGaugeBar(int left, int right, int y, bool reverse)
{
    int width = right - left;

    for (int x = left; x < right; x++)
    {
        float percent =
            (float)(x - left) * 100.0f / width;

        // CPU側は左右反転
        if (reverse)
        {
            percent = 100.0f - percent;
        }

        int color = GetCIGaugeColor(percent);

        DrawLine(
            x, y - 15,
            x, y + 15,
            color
        );
    }

    // 上側の光沢
    DrawBox(
        left, y - 15,
        right, y - 8,
        GetColor(255, 255, 255),
        TRUE
    );

    // 下側の影
    DrawBox(
        left, y + 11,
        right, y + 15,
        GetColor(80, 50, 20),
        TRUE
    );
}

// ゲージ停止時の光の輪を描画
void DrawCIStopEffect(int x, int y, int startTime, int color)
{
    // 演出開始時刻が設定されていない場合は終了
    if (startTime <= 0)
    {
        return;
    }

    // 停止してからの経過時間
    int elapsed = GetNowCount() - startTime;

    // 演出時間は600ミリ秒
    if (elapsed >= 600)
    {
        return;
    }

    // 時間経過に合わせて透明度を下げる
    int alpha = 255 - (elapsed * 255 / 600);

    // 時間経過に合わせて光の輪を広げる
    int radius = 15 + elapsed / 10;

    // 半透明で描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

    // 外側の光の輪
    DrawCircle(
        x, y,
        radius + 8,
        color,
        FALSE
    );

    // 内側の光の輪
    DrawCircle(
        x, y,
        radius,
        GetColor(255, 255, 255),
        FALSE
    );

    // 描画モードを元に戻す
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

//ゲージの初期化内容をここに記載
void ChargeInpact::Init()
{
    barX = (float)BAR_LEFT;
    direction = 1;
    speed = 3.0f;
    baseSpeed = speed;
    playerSpeedRate = 1.0f;
    tieCount = 0;
    isStopped = false;
    oldSpace = false;
    //GetNowCount() = 動後の経過時間をミリ秒単位表す関数
    //GetNowCount() = DxLibに最初から用意されている関数
    startTime = GetNowCount();
    //残り時間
    remainingTime = 10.0f;
    score = 0;

    // ゲージ停止時の判定を初期化
    judgement = 0;

    // 判定表示時刻を初期化
    judgementStartTime = 0;

    // CPU側の判定を初期化
    cpuJudgement = 0;

    // CPU側の判定表示時刻を初期化
    cpuJudgementStartTime = 0;

    // CPU側の初期化
    // CPUは右端からスタート
    cpuBarX = (float)CPU_BAR_RIGHT;
    cpuDirection = -1;
    cpuSpeed = speed;
    cpuIsStopped = false;
    cpuScore = 0;

    // CPUの停止目標を設定
    cpuTargetScore = GetRand(100);
}

void ChargeInpact::Update()
{
    // スペースキーの状態を取得
    int space = CheckHitKey(KEY_INPUT_SPACE);

    // 押した瞬間だけ判定
    bool spacePressed = (space == 1 && oldSpace == false);

    // 前回のキー状態を保存
    oldSpace = space;

    // 停止済みなら処理を終了
    if (isStopped)
    {
        return;
    }

    // 開始してからの経過時間（ミリ秒）
    int elapsedTime = GetNowCount() - startTime;

    // 残り時間を計算
    remainingTime = (10000 - elapsedTime) / 1000.0f;

    // 0秒より小さくならないようにする
    if (remainingTime < 0.0f)
    {
        remainingTime = 0.0f;
    }

    // 10秒経過したら自動停止
    if (elapsedTime >= 10000)
    {
        isStopped = true;
        score = 0;

        // 時間切れはGOOD判定
        judgement = 0;
        judgementStartTime = GetNowCount();

        return;
    }

    // スペースキーを押した瞬間だけ停止
    if (spacePressed)
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

        // スコアに応じて判定を決める
        if (score == 100)
        {
            judgement = 3;
        }
        else if (score >= 75)
        {
            judgement = 2;
        }
        else if (score >= 50)
        {
            judgement = 1;
        }
        else
        {
            judgement = 0;
        }

        // 判定アニメーションの開始時刻
        judgementStartTime = GetNowCount();

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
    // 現在の時間
    int now = GetNowCount();

    // ゲージの背景パネル
    DrawBox(
        BAR_LEFT - 15, BAR_Y - 45,
        BAR_RIGHT + 15, BAR_Y + 65,
        GetColor(20, 25, 40), TRUE
    );

    // パネルの枠
    DrawBox(
        BAR_LEFT - 15, BAR_Y - 45,
        BAR_RIGHT + 15, BAR_Y + 65,
        GetColor(100, 130, 170), FALSE
    );

    // ゲージの背景
    DrawBox(
        BAR_LEFT, BAR_Y - 15,
        BAR_RIGHT, BAR_Y + 15,
        GetColor(45, 50, 65), TRUE
    );

    // グラデーションゲージ
    DrawCIGaugeBar(
        BAR_LEFT,
        BAR_RIGHT,
        BAR_Y,
        false
    );

    // 50％の位置
    int fiftyX = BAR_LEFT + BAR_WIDTH / 2;

    DrawLine(
        fiftyX, BAR_Y - 28,
        fiftyX, BAR_Y + 28,
        GetColor(0, 200, 255)
    );

    // 75％の位置
    int seventyFiveX = BAR_LEFT + BAR_WIDTH * 3 / 4;

    DrawLine(
        seventyFiveX, BAR_Y - 28,
        seventyFiveX, BAR_Y + 28,
        GetColor(255, 180, 0)
    );

    // 100％の位置
    DrawLine(
        BAR_RIGHT, BAR_Y - 28,
        BAR_RIGHT, BAR_Y + 28,
        GetColor(255, 220, 0)
    );

    // 目盛りの説明
    DrawString(
        BAR_LEFT - 5, BAR_Y + 35,
        L"0% START",
        GetColor(180, 180, 180)
    );

    DrawString(
        fiftyX - 20, BAR_Y + 35,
        L"50%",
        GetColor(0, 200, 255)
    );

    DrawString(
        seventyFiveX - 20, BAR_Y + 35,
        L"75%",
        GetColor(255, 180, 0)
    );

    DrawString(
        BAR_RIGHT - 45, BAR_Y + 35,
        L"100% MAX",
        GetColor(255, 220, 0)
    );

    // ゲージの移動マーカー
    int markerX = (int)barX;

    // 停止時の光の輪
    if (isStopped)
    {
        DrawCIStopEffect(
            markerX,
            BAR_Y,
            judgementStartTime,
            GetColor(255, 180, 50)
        );
    }

    // マーカーの光
    DrawBox(
        markerX - 12, BAR_Y - 30,
        markerX + 12, BAR_Y + 30,
        GetColor(150, 60, 30), TRUE
    );

    // 停止時はさらに明るくする
    if (isStopped)
    {
        DrawBox(
            markerX - 9, BAR_Y - 27,
            markerX + 9, BAR_Y + 27,
            GetColor(255, 200, 80), TRUE
        );
    }

    // マーカー本体
    DrawBox(
        markerX - 4, BAR_Y - 25,
        markerX + 4, BAR_Y + 25,
        GetColor(255, 70, 70), TRUE
    );

    // マーカーの中心線
    DrawLine(
        markerX, BAR_Y - 25,
        markerX, BAR_Y + 25,
        GetColor(255, 255, 255)
    );

    // スコア表示
    DrawFormatString(
        BAR_LEFT, BAR_Y + 80,
        GetColor(255, 255, 255),
        L"SCORE : %d", score
    );

    // 停止後の判定表示
    if (isStopped)
    {
        const wchar_t* judgeName = L"GOOD";
        unsigned int judgeColor = GetColor(100, 255, 100);

        // 判定の種類を取得
        switch (judgement)
        {
        case 0:
            judgeName = L"GOOD";
            judgeColor = GetColor(100, 255, 100);
            break;

        case 1:
            judgeName = L"VERY GOOD";
            judgeColor = GetColor(80, 200, 255);
            break;

        case 2:
            judgeName = L"GREAT";
            judgeColor = GetColor(255, 160, 40);
            break;

        case 3:
            judgeName = L"PERFECT!!";
            judgeColor = GetColor(255, 220, 0);
            break;
        }

        // 判定表示のアニメーション
        int elapsed = now - judgementStartTime;
        int pulse = elapsed % 800;

        double scale =
            1.0 + (pulse < 400 ? pulse : 800 - pulse) / 400.0 * 0.15;

        DrawExtendString(500, 210, scale, scale, judgeName, judgeColor);

        DrawString(
            BAR_LEFT, BAR_Y + 110,
            L"STOP!",
            GetColor(255, 255, 255)
        );
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

// CIゲージの速度を設定する
void ChargeInpact::SetSpeed(float newSpeed)
{
    speed = newSpeed;
    cpuSpeed = newSpeed;

    // 試合開始時の基本速度を保存
    baseSpeed = newSpeed;

    // 速度倍率を初期化
    playerSpeedRate = 1.0f;
}

// プレイヤー側の速度倍率を設定
void ChargeInpact::SetPlayerSpeedRate(float rate)
{
    playerSpeedRate = rate;

    // プレイヤー側だけ速度を変更
    speed = baseSpeed * playerSpeedRate;
}

// CPU用のゲージ更新
void ChargeInpact::UpdateCPU()
{
    if (cpuIsStopped)
    {
        return;
    }

    // CPUのゲージを移動
    cpuBarX += cpuSpeed * cpuDirection;

    // 左端に到達したら右向き
    if (cpuBarX <= CPU_BAR_LEFT)
    {
        cpuBarX = (float)CPU_BAR_LEFT;
        cpuDirection = 1;
    }

    // 右端に到達したら左向き
    if (cpuBarX >= CPU_BAR_RIGHT)
    {
        cpuBarX = (float)CPU_BAR_RIGHT;
        cpuDirection = -1;
    }

    // 右から左に進むほどスコアが上がる
    int currentScore =
        (int)((CPU_BAR_RIGHT - cpuBarX) * 100.0f / CPU_BAR_WIDTH);

    // 目標スコアに到達したら停止
    if (currentScore >= cpuTargetScore)
    {
        cpuIsStopped = true;
        cpuScore = currentScore;

        // CPUのスコアに応じて判定を決める
        if (cpuScore == 100)
        {
            cpuJudgement = 3;
        }
        else if (cpuScore >= 75)
        {
            cpuJudgement = 2;
        }
        else if (cpuScore >= 50)
        {
            cpuJudgement = 1;
        }
        else
        {
            cpuJudgement = 0;
        }

        // CPUの演出開始時刻を保存
        cpuJudgementStartTime = GetNowCount();
    }
}

// CPU用のゲージ描画
void ChargeInpact::DrawCPU()
{
    int cpuY = BAR_Y;

    // 背景パネル
    DrawBox(
        CPU_BAR_LEFT - 15, cpuY - 45,
        CPU_BAR_RIGHT + 15, cpuY + 65,
        GetColor(20, 25, 40), TRUE
    );

    // パネルの枠
    DrawBox(
        CPU_BAR_LEFT - 15, cpuY - 45,
        CPU_BAR_RIGHT + 15, cpuY + 65,
        GetColor(100, 130, 170), FALSE
    );

    // ゲージの背景
    DrawBox(
        CPU_BAR_LEFT, cpuY - 15,
        CPU_BAR_RIGHT, cpuY + 15,
        GetColor(45, 50, 65), TRUE
    );

    // CPU側のグラデーションゲージ
    DrawCIGaugeBar(
        CPU_BAR_LEFT,
        CPU_BAR_RIGHT,
        cpuY,
        true
    );

    // 50％の位置
    int fiftyX = (CPU_BAR_LEFT + CPU_BAR_RIGHT) / 2;

    DrawLine(
        fiftyX, cpuY - 28,
        fiftyX, cpuY + 28,
        GetColor(0, 200, 255)
    );

    // CPUは右から左に進むため、75％は左側
    int seventyFiveX =
        CPU_BAR_RIGHT - CPU_BAR_WIDTH * 3 / 4;

    DrawLine(
        seventyFiveX, cpuY - 28,
        seventyFiveX, cpuY + 28,
        GetColor(255, 180, 0)
    );

    // 100％の位置
    DrawLine(
        CPU_BAR_LEFT, cpuY - 28,
        CPU_BAR_LEFT, cpuY + 28,
        GetColor(255, 220, 0)
    );

    // 目盛りの説明
    DrawString(
        CPU_BAR_RIGHT - 55, cpuY + 35,
        L"0% START",
        GetColor(180, 180, 180)
    );

    DrawString(
        fiftyX - 20, cpuY + 35,
        L"50%",
        GetColor(0, 200, 255)
    );

    DrawString(
        seventyFiveX - 20, cpuY + 35,
        L"75%",
        GetColor(255, 180, 0)
    );

    // 100％ MAXをバーの左側に配置
    DrawString(
        CPU_BAR_LEFT - 115, cpuY + 35,
        L"MAX 100%",
        GetColor(255, 220, 0)
    );

    // CPUゲージのマーカー
    int markerX = (int)cpuBarX;

    // CPU停止時の光の輪
    if (cpuIsStopped)
    {
        DrawCIStopEffect(
            markerX,
            cpuY,
            cpuJudgementStartTime,
            GetColor(50, 160, 255)
        );
    }

    // マーカーの光
    DrawBox(
        markerX - 12, cpuY - 30,
        markerX + 12, cpuY + 30,
        GetColor(30, 100, 180), TRUE
    );

    // 停止時はさらに明るくする
    if (cpuIsStopped)
    {
        DrawBox(
            markerX - 9, cpuY - 27,
            markerX + 9, cpuY + 27,
            GetColor(100, 200, 255), TRUE
        );
    }

    // マーカー本体
    DrawBox(
        markerX - 4, cpuY - 25,
        markerX + 4, cpuY + 25,
        GetColor(50, 150, 255), TRUE
    );

    // マーカーの中心線
    DrawLine(
        markerX, cpuY - 25,
        markerX, cpuY + 25,
        GetColor(255, 255, 255)
    );

    // CPUスコア
    DrawFormatString(
        CPU_BAR_LEFT, cpuY + 80,
        GetColor(255, 255, 255),
        L"ENEMY SCORE : %d", cpuScore
    );

    // CPU停止後の判定表示
    if (cpuIsStopped)
    {
        const wchar_t* judgeName = L"GOOD";
        unsigned int judgeColor = GetColor(100, 255, 100);

        // 判定の種類を取得
        switch (cpuJudgement)
        {
        case 0:
            judgeName = L"GOOD";
            judgeColor = GetColor(100, 255, 100);
            break;

        case 1:
            judgeName = L"VERY GOOD";
            judgeColor = GetColor(80, 200, 255);
            break;

        case 2:
            judgeName = L"GREAT";
            judgeColor = GetColor(255, 160, 40);
            break;

        case 3:
            judgeName = L"PERFECT!!";
            judgeColor = GetColor(255, 220, 0);
            break;
        }

        // 判定文字のアニメーション
        int elapsed = GetNowCount() - cpuJudgementStartTime;
        int pulse = elapsed % 800;

        double scale =
            1.0 + (pulse < 400 ? pulse : 800 - pulse) / 400.0 * 0.15;

        // 判定文字を表示
        DrawExtendString(
            1250, 210,
            scale, scale,
            judgeName,
            judgeColor
        );

        DrawString(
            CPU_BAR_LEFT, cpuY + 110,
            L"ENEMY STOP!",
            GetColor(255, 255, 255)
        );
    }
}

// CPUスコアを取得
int ChargeInpact::GetCPUScore()
{
    return cpuScore;
}

// CPUの停止状態を取得
bool ChargeInpact::IsCPUStopped()
{
    return cpuIsStopped;
}

// CPUの停止スコア範囲を設定
void ChargeInpact::SetCPUTargetRange(int minScore, int maxScore)
{
    cpuMinScore = minScore;
    cpuMaxScore = maxScore;

    cpuTargetScore = GetRand(cpuMaxScore - cpuMinScore) + cpuMinScore;
}

// あいこ後の再挑戦処理
void ChargeInpact::RetryAfterTie()
{
    // あいこの回数を増やす
    tieCount++;

    // あいこの回数に応じて速度を上げる
    float currentSpeed = baseSpeed + tieCount * 2.0f;

    // プレイヤー側は補助設定を反映
    speed = currentSpeed * playerSpeedRate;

    // CPU側は通常速度
    cpuSpeed = currentSpeed;

    // プレイヤーゲージを初期位置に戻す
    barX = (float)BAR_LEFT;
    direction = 1;
    isStopped = false;
    score = 0;
    // 判定を初期化
    judgement = 0;
    judgementStartTime = 0;
    oldSpace = CheckHitKey(KEY_INPUT_SPACE);

    // CPU側の判定を初期化
    cpuJudgement = 0;
    cpuJudgementStartTime = 0;

    // CPUゲージを初期位置に戻す
    cpuBarX = (float)CPU_BAR_RIGHT;
    cpuDirection = -1;
    cpuIsStopped = false;
    cpuScore = 0;

    // 試合ごとの難易度を維持してCPUの停止目標を再設定
    cpuTargetScore = GetRand(cpuMaxScore - cpuMinScore) + cpuMinScore;

    // 制限時間をリセット
    startTime = GetNowCount();
    remainingTime = 10.0f;
}

// 残り時間を取得
float ChargeInpact::GetRemainingTime()
{
    return remainingTime;
}