//バーの位置や速度などの変数と関数を宣言amfe
#pragma once

class ChargeInpact
{

//：クラスの外から直接変更させたくない変数を書く。
private:
	// ゲージの位置
	float barX;

	// ゲージの移動方向（1なら右、-1なら左）
	int direction;

	// ゲージの移動速度
	float speed;

	// ゲージが停止しているか
	bool isStopped;

	// ゲージの開始時刻
	int startTime;

	// ゲージのスコア
	int score;

	// 残り時間（秒）
	float remainingTime;

//public：クラスの外からでも呼び出せる関数を書く。
public:
	// ゲージを初期化する
	void Init();

	// ゲージを更新する
	void Update();

	// ゲージを描画する
	void Draw();

	// ゲージが停止しているか取得する
	bool IsStopped();

	// スコアを取得する
	int GetScore();

};
