#include "Tournament.h"

// トーナメントを初期化する
void Tournament::Init()
{
    winCount = 0;
    battleCount = 0;

        isFinished = false;
    isWinner = false;

}

// 戦闘結果を受け取る
void Tournament::SetBattleResult(bool playerWin)
{
    if (isFinished) return;

        battleCount++;

    if (playerWin)
    {
        winCount++;

        // 3回勝利したら優勝
        if (winCount >= 3)
        {
            isFinished = true;
            isWinner = true;
        }
    }
    else
    {
        // 1回でも負けたら終了
        isFinished = true;
        isWinner = false;
    }

}

int Tournament::GetWinCount()
{
    return winCount;
}

int Tournament::GetBattleCount()
{
    return battleCount;
}

bool Tournament::IsFinished()
{
    return isFinished;
}

bool Tournament::IsWinner()
{
    return isWinner;
}
