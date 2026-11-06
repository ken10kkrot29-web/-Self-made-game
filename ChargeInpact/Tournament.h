#pragma once

class Tournament
{
private:
	int winCount;       // 勝利数
	int battleCount;    // 戦闘回数
	bool isFinished;    // トーナメントが終了したか
	bool isWinner;      // 優勝したか

public:
	void Init();

		// 1試合の結果を受け取る
		void SetBattleResult(bool playerWin);

	int GetWinCount();
	int GetBattleCount();

	bool IsFinished();
	bool IsWinner();

};
