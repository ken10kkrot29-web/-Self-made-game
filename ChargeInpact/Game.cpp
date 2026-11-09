// 画面表示、キー入力、画面切り替え
#include "DxLib.h"
#include "Game.h"

// 敵の能力名を取得
const wchar_t* GetAbilityName(EnemyAbility ability)
{
    switch (ability)
    {
    case EnemyAbility::Attack:
        return L"攻撃";

    case EnemyAbility::Defense:
        return L"防御";

    case EnemyAbility::Recovery:
        return L"回復";

    case EnemyAbility::AttackBoost:
        return L"攻撃強化";

    case EnemyAbility::DefenseBoost:
        return L"防御強化";

    case EnemyAbility::CISpeedInterference:
        return L"CI速度妨害";

    default:
        return L"なし";
    }
}

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

    tournament.Init();
    battle.Init(0);
    chargeInpact.Init();

    resultSent = false;
    isTie = false;

    isTieWaiting = false;
    tieWaitStartTime = 0;
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
            // ゲーム終了
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
            assistSelect = 1;
            scene = 5;
        }
    }

    // CI補助選択画面
    else if (scene == 5)
    {
        // 上キー
        if (up == 1 && oldUp == 0)
        {
            assistSelect--;

            if (assistSelect < 0)
            {
                assistSelect = 1;
            }
        }

        // 下キー
        if (down == 1 && oldDown == 0)
        {
            assistSelect++;

            if (assistSelect > 1)
            {
                assistSelect = 0;
            }
        }

        // ENTERキーで決定
        if (enter == 1 && oldEnter == 0)
        {
            useCIAssist = (assistSelect == 0);

            select = 0;
            scene = 1;
        }
    }

    // 奇策選択画面
    else if (scene == 6)
    {
        // 上キー
        if (up == 1 && oldUp == 0)
        {
            trickSelect--;

            if (trickSelect < 0)
            {
                trickSelect = 1;
            }
        }

        // 下キー
        if (down == 1 && oldDown == 0)
        {
            trickSelect++;

            if (trickSelect > 1)
            {
                trickSelect = 0;
            }
        }

        // ENTERキー
        if (enter == 1 && oldEnter == 0)
        {
            // 選択した奇策を保存
            battle.SetTrickChoice(trickSelect);

            // 回復を選択した場合
            if (trickSelect == 0)
            {
                StartChargeImpact();
            }
            // 能力封印を選択した場合
            else
            {
                // 封印できる能力を調べる
                CreateSealCandidates();

                // 封印できる能力がある場合
                if (sealCandidateCount > 0 &&
                    battle.GetSealUseCount() < 2)
                {
                    sealSelect = 0;
                    scene = 7;
                }
                else
                {
                    // 封印できない場合は回復に変更
                    trickSelect = 0;
                    battle.SetTrickChoice(0);

                    StartChargeImpact();
                }
            }
        }
    }

    // 封印する能力の選択画面
    else if (scene == 7)
    {
        // 上キー
        if (up == 1 && oldUp == 0)
        {
            sealSelect--;

            if (sealSelect < 0)
            {
                sealSelect = sealCandidateCount - 1;
            }
        }

        // 下キー
        if (down == 1 && oldDown == 0)
        {
            sealSelect++;

            if (sealSelect >= sealCandidateCount)
            {
                sealSelect = 0;
            }
        }

        // ENTERキー
        if (enter == 1 && oldEnter == 0)
        {
            // 封印する能力を保存
            battle.SetDisableAction(sealCandidates[sealSelect]);

            // CI開始
            StartChargeImpact();
        }
    }

    // 戦略選択画面
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

        // ENTERキーで戦略を決定
        if (enter == 1 && oldEnter == 0)
        {
            selectedStrategy = select;

            // 選択した戦略を保存
            battle.SetPlayerStrategy((StrategyType)selectedStrategy);

            // 奇策を選んだ場合
            if (selectedStrategy == 2)
            {
                trickSelect = 0;
                scene = 6;
            }
            else
            {
                // それ以外はそのままCI開始
                StartChargeImpact();
            }
        }
    }

        // CI画面
        else if (scene == 2)
        {
            // あいこ後の待機中
            if (isTieWaiting)
            {
                // 2秒経過したら再挑戦
                if (GetNowCount() - tieWaitStartTime >= 2000)
                {
                    isTieWaiting = false;
                    isTie = false;

                    // ゲージを再挑戦できる状態にする
                    chargeInpact.RetryAfterTie();
                }
            }
            else
            {
                // プレイヤーのゲージ更新
                chargeInpact.Update();

                // CPUのゲージ更新
                chargeInpact.UpdateCPU();

                // 両方のゲージが停止したら結果判定
                if (chargeInpact.IsStopped() &&
                    chargeInpact.IsCPUStopped() &&
                    !resultSent)
                {
                    // スコアが同じならあいこ
                    if (chargeInpact.GetScore() == chargeInpact.GetCPUScore())
                    {
                        isTie = true;
                        isTieWaiting = true;

                        // 待機開始時刻を保存
                        tieWaitStartTime = GetNowCount();
                    }
                    else
                    {
                        // 勝敗が決まった場合
                        bool playerWin =
                            chargeInpact.GetScore() > chargeInpact.GetCPUScore();

                        battle.SetChargeImpactResult(playerWin);
                        battle.ResolveBattle();

                        isTie = false;
                        resultSent = true;
                        scene = 3;
                    }
                }
            }
        }

        // 戦闘結果画面
        else if (scene == 3)
        {
            if (enter == 1 && oldEnter == 0)
            {
                // 戦闘が終了している場合
                if (battle.IsBattleEnd())
                {
                    // 敵を倒していたらプレイヤーの勝利
                    bool playerWin = battle.GetEnemy().IsDefeated();

                    // トーナメントに結果を送る
                    tournament.SetBattleResult(playerWin);

                    // トーナメントが終了した場合
                    if (tournament.IsFinished())
                    {
                        scene = 4;
                    }
                    else
                    {
                        // 次の試合を開始
                        battle.Init(tournament.GetBattleCount());
                        chargeInpact.Init();

                        assistSelect = 1;
                        select = 0;

                        scene = 5;
                    }
                }
                else
                {
                    // まだ戦闘が続いている場合
                    scene = 1;
                    chargeInpact.Init();
                }

                select = 0;
                resultSent = false;
            }
        }

        // トーナメント結果画面
        else if (scene == 4)
        {
            if (enter == 1 && oldEnter == 0)
            {
                tournament.Init();
                battle.Init();
                chargeInpact.Init();

                scene = 0;
                select = 0;
            }
        }

        // 前回のキー状態を保存
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
            L"STRATEGY バトル",
            GetColor(255, 255, 255));

        DrawString(660, 405,
            L" ENTER to start",
            GetColor(255, 255, 0));
    }

    // CI補助選択画面
    else if (scene == 5)
    {
        DrawString(650, 220,
            L"CI補助を選択してください",
            GetColor(255, 255, 255));

        DrawString(750, 380,
            assistSelect == 0 ? L"> ON" : L"  ON",
            GetColor(100, 255, 100));

        DrawString(750, 480,
            assistSelect == 1 ? L"> OFF" : L"  OFF",
            GetColor(255, 255, 255));

        DrawString(600, 650,
            L"ON : プレイヤーのCI速度が20%低下",
            GetColor(200, 200, 200));

        DrawString(650, 850,
            L"上 / 下 : 選択    ENTER : 決定",
            GetColor(200, 200, 200));
    }

    // 奇策選択画面
    else if (scene == 6)
    {
        DrawString(650, 220,
            L"奇策を選択してください",
            GetColor(255, 255, 255));

        DrawString(750, 380,
            trickSelect == 0 ? L"> HP回復" : L"  HP回復",
            GetColor(100, 255, 100));

        DrawString(750, 480,
            trickSelect == 1 ? L"> 能力封印" : L"  能力封印",
            GetColor(255, 220, 100));

        DrawFormatString(650, 600,
            GetColor(255, 255, 255),
            L"封印使用回数 : %d / 2",
            battle.GetSealUseCount());

        DrawString(650, 850,
            L"上 / 下 : 選択    ENTER : 決定",
            GetColor(200, 200, 200));
    }

    // 封印する能力の選択画面
    else if (scene == 7)
    {
        DrawString(650, 180,
            L"封印する敵の能力を選択",
            GetColor(255, 255, 255));

        // 封印できる能力を表示
        for (int i = 0; i < sealCandidateCount; i++)
        {
            DrawString(700, 300 + i * 80,
                GetAbilityName(sealCandidates[i]),
                GetColor(255, 255, 255));

            // 選択中の能力にマークを付ける
            if (i == sealSelect)
            {
                DrawString(650, 300 + i * 80,
                    L">",
                    GetColor(255, 255, 0));
            }
        }

        DrawString(650, 850,
            L"上 / 下 : 選択    ENTER : 決定",
            GetColor(200, 200, 200));
    }

    // 戦略選択画面
    else if (scene == 1)
    {
        DrawString(300, 180,
            L"戦略を選択してください",
            GetColor(255, 255, 255));

        // 現在の試合数を表示
        DrawFormatString(300, 250,
            GetColor(255, 255, 0),
            L"バトル : %d / 3",
            tournament.GetBattleCount() + 1);

        DrawFormatString(300, 290,
            GetColor(255, 255, 255),
            L"勝利数 : %d",
            tournament.GetWinCount());

        DrawString(360, 360,
            select == 0 ? L"> 攻撃" : L"  攻撃",
            GetColor(255, 100, 100));

        DrawString(360, 472,
            select == 1 ? L"> 防御" : L"  防御",
            GetColor(100, 200, 255));

        DrawString(360, 585,
            select == 2 ? L"> 奇策" : L"  奇策",
            GetColor(255, 220, 100));

        DrawString(300, 788,
            L"上 / 下（矢印キー） : 選択",
            GetColor(200, 200, 200));

        DrawString(300, 855,
            L"ESC : ゲーム終了",
            GetColor(200, 200, 200));
    }

    // CI画面
    else if (scene == 2)
    {
        DrawString(300, 100,
            L"CHARGE IMPACT",
            GetColor(255, 255, 255));

        // プレイヤー側のゲージ
        chargeInpact.Draw();

        // CPU側のゲージ
        chargeInpact.DrawCPU();

        // あいこ時のメッセージ
        if (isTie)
        {
            SetFontSize(44);

            DrawString(480, 600,
                L"あいこだ！！",
                GetColor(255, 255, 0));

            DrawString(390, 670,
                L"もう一度勝負して決着をつけよう！！",
                GetColor(255, 255, 255));

            SetFontSize(36);
        }

        DrawString(100, 400,
            L"SPACE : ゲージを止める",
            GetColor(255, 255, 0));

        DrawFormatString(100, 500,
            GetColor(255, 255, 255),
            L"Player HP : %d",
            battle.GetPlayer().GetHP());

        DrawFormatString(100, 550,
            GetColor(255, 255, 255),
            L"Enemy HP : %d",
            battle.GetEnemy().GetHP());
    }

    // 戦闘結果画面
    else if (scene == 3)
    {
        // タイトル
        DrawString(820, 120,
            L"バトル結果",
            GetColor(255, 255, 255));

        DrawFormatString(850, 190,
            GetColor(255, 255, 0),
            L"BATTLE : %d / 3",
            tournament.GetBattleCount() + 1);


        // プレイヤー側（左）
        DrawString(350, 300,
            L"PLAYER",
            GetColor(100, 200, 255));

        DrawFormatString(300, 390,
            GetColor(255, 255, 255),
            L"PLAYER HP : %d",
            battle.GetPlayer().GetHP());

        DrawFormatString(300, 480,
            GetColor(255, 255, 255),
            L"CI SCORE : %d",
            chargeInpact.GetScore());


        // エネミー側（右）
        DrawString(1350, 300,
            L"ENEMY",
            GetColor(255, 100, 100));

        DrawFormatString(1250, 390,
            GetColor(255, 255, 255),
            L"ENEMY HP : %d",
            battle.GetEnemy().GetHP());

        DrawFormatString(1250, 480,
            GetColor(255, 255, 255),
            L"CI SCORE : %d",
            chargeInpact.GetCPUScore());


        // CIの勝敗
        if (chargeInpact.GetScore() >= chargeInpact.GetCPUScore())
        {
            DrawString(300, 560,
                L"CI WIN!",
                GetColor(255, 255, 0));

            DrawString(1250, 560,
                L"CI LOSE...",
                GetColor(255, 100, 100));
        }
        else
        {
            DrawString(300, 560,
                L"CI LOSE...",
                GetColor(255, 100, 100));

            DrawString(1250, 560,
                L"CI WIN!",
                GetColor(255, 255, 0));
        }


        // バトルの勝敗
        if (battle.GetEnemy().IsDefeated())
        {
            SetFontSize(60);

            DrawString(760, 670,
                L"PLAYERの勝ち！！",
                GetColor(255, 255, 0));

            SetFontSize(36);
        }
        else if (battle.GetPlayer().IsDefeated())
        {
            SetFontSize(60);

            DrawString(760, 670,
                L"ENEMYの勝ち！！",
                GetColor(255, 100, 100));

            SetFontSize(36);
        }
        else
        {
            DrawString(800, 680,
                L"次の戦略を選択",
                GetColor(255, 255, 255));
        }

        DrawString(800, 800,
            L"ENTER : 続ける",
            GetColor(200, 200, 200));
    }

    // トーナメント結果画面
    else if (scene == 4)
    {
        DrawString(300, 200,
            L"トーナメント結果",
            GetColor(255, 255, 255));

        DrawFormatString(300, 350,
            GetColor(255, 255, 255),
            L"勝利数 : %d",
            tournament.GetWinCount());

        if (tournament.IsWinner())
        {
            DrawString(300, 500,
                L"トーナメントクリア！",
                GetColor(255, 255, 0));
        }
        else
        {
            DrawString(300, 500,
                L"残念！",
                GetColor(255, 100, 100));
        }

        DrawString(300, 700,
            L"ENTER : タイトルに戻る",
            GetColor(200, 200, 200));
    }
}

// 封印できる能力を調べる
void Game::CreateSealCandidates()
{
    sealCandidateCount = 0;

    // 敵の6種類の能力を確認
    for (int i = 0; i < 6; i++)
    {
        EnemyAbility ability = static_cast<EnemyAbility>(i);

        // 敵が持っていて、封印されていない能力だけ登録
        if (battle.GetEnemy().HasAbility(ability) &&
            !battle.GetEnemy().IsAbilitySealed(ability))
        {
            sealCandidates[sealCandidateCount] = ability;
            sealCandidateCount++;
        }
    }
}

// チャージインパクトを開始する
void Game::StartChargeImpact()
{
    // 敵の行動を決定
    battle.StartTurn();

    // CIゲージを初期化
    chargeInpact.Init();

    // 試合数を取得
    int battleNumber = tournament.GetBattleCount();

    // 試合数に応じてCI速度を変更
    if (battleNumber == 0)
    {
        chargeInpact.SetSpeed(4.5f);
    }
    else if (battleNumber == 1)
    {
        chargeInpact.SetSpeed(10.0f);
    }
    else if (battleNumber == 2)
    {
        chargeInpact.SetSpeed(15.0f);
    }

    // プレイヤーのCI速度倍率
    float playerSpeedRate = 1.0f;

    // CI補助
    if (useCIAssist)
    {
        playerSpeedRate *= 0.8f;
    }

    // 敵のCI速度妨害
    if (battle.IsCISpeedInterferenceActive())
    {
        playerSpeedRate *= 1.2f;
    }

    // プレイヤーのCI速度に反映
    chargeInpact.SetPlayerSpeedRate(playerSpeedRate);

    // CPUの難易度
    if (battleNumber == 0)
    {
        chargeInpact.SetCPUTargetRange(40, 60);
    }
    else if (battleNumber == 1)
    {
        chargeInpact.SetCPUTargetRange(60, 80);
    }
    else if (battleNumber == 2)
    {
        chargeInpact.SetCPUTargetRange(75, 100);
    }

    resultSent = false;

    // CI画面へ
    scene = 2;
}