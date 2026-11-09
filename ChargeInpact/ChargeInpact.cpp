//バーの移動・停止・制限時間などを実装
#include "DxLib.h"
#include "ChargeInpact.h"

// ゲージの左端,右端（X軸、Y軸）座標
const int BAR_LEFT = 100;
const int BAR_RIGHT = 540;
const int BAR_Y = 240;
const int BAR_WIDTH = BAR_RIGHT - BAR_LEFT;
// CPUゲージの左端・右端
const int CPU_BAR_LEFT = 1250;
const int CPU_BAR_RIGHT = 1690;
const int CPU_BAR_WIDTH = CPU_BAR_RIGHT - CPU_BAR_LEFT;

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

    // 制限時間は10秒
    if (GetNowCount() - startTime >= 10000)
    {
            
        isStopped = true;
        score = 0;
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
        L"Score:%d", score);

    // 残り時間の状態
    if (isStopped)
    {
        DrawString(100, 320, L"STOP!", GetColor(255, 255, 255));
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
    }
}

// CPU用のゲージ描画
void ChargeInpact::DrawCPU()
{
    // プレイヤーと同じ高さにする
    int cpuY = BAR_Y;

    // ゲージ背景
    DrawBox(CPU_BAR_LEFT, cpuY - 10,
        CPU_BAR_RIGHT, cpuY + 10,
        GetColor(80, 80, 80), TRUE);

    // スコア50の位置
    DrawLine((CPU_BAR_LEFT + CPU_BAR_RIGHT) / 2,
        cpuY - 25,
        (CPU_BAR_LEFT + CPU_BAR_RIGHT) / 2,
        cpuY + 25,
        GetColor(0, 255, 0));

    // 現在の位置
    DrawBox((int)cpuBarX - 3, cpuY - 18,
        (int)cpuBarX + 3, cpuY + 18,
        GetColor(50, 150, 255), TRUE);

    // CPUスコア
    DrawFormatString(CPU_BAR_LEFT, 290,
        GetColor(255, 255, 255),
        L"ENEMY Score:%d", cpuScore);

    if (cpuIsStopped)
    {
        DrawString(CPU_BAR_LEFT, 320,
            L"ENEMY STOP!",
            GetColor(255, 255, 255));
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
    oldSpace = CheckHitKey(KEY_INPUT_SPACE);

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