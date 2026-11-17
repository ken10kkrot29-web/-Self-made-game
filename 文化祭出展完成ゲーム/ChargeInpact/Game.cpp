// 画面表示、キー入力、画面切り替え
#include "DxLib.h"
#include "Game.h"
#include "Enemy.h"
#include "Strategy.h"
#include <Windows.h>

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

    case EnemyAbility::CISpeedInterference:
        return L"CI速度妨害";

    default:
        return L"なし";
    }
}

// 能力封印が使用できるか確認
bool Game::CanUseSeal()
{
    // 封印を5回使用していたら使用不可
    if (battle.GetSealUseCount() >= 5)
    {
        return false;
    }

    Enemy& enemy = battle.GetEnemy();

    // 封印できる能力が残っているか確認
    for (int i = 0; i < 4; i++)
    {
        EnemyAbility ability =
            static_cast<EnemyAbility>(i);

        if (enemy.HasAbility(ability) &&
            !enemy.IsAbilitySealed(ability))
        {
            return true;
        }
    }

    return false;
}

// ゲームの初期化
void Game::Init()
{
    scene = 0;
    //// タイトル背景画像を読み込む
    titleBgHandle = LoadGraph(L"images/title_bg.png");

    // 戦闘画面用背景画像を読み込む
    battleBgHandle = LoadGraph(L"images/battle_bg.png");

    select = 0;
    selectedStrategy = 0;

    assistSelect = 1;
    useCIAssist = false;

    oldEnter = false;
    oldUp = false;
    oldDown = false;
    oldEscape = false;
    oldTab = false;
    oldR = false;

    showEnemyInfo = false;

    battle.Init();
    chargeInpact.Init();

    resultSent = false;

    isTie = false;
    isTieWaiting = false;
    tieWaitStartTime = 0;

    trickSelect = 0;
    sealSelect = 0;
    sealCandidateCount = 0;

    resultStartTime = 0;
}

// ゲームの更新
bool Game::Update()
{
    // キー入力を取得
    int enter = CheckHitKey(KEY_INPUT_Z);
    int up = CheckHitKey(KEY_INPUT_W);
    int down = CheckHitKey(KEY_INPUT_S);
    int escape = CheckHitKey(KEY_INPUT_ESCAPE);
    int tab = CheckHitKey(KEY_INPUT_TAB);
    int r = CheckHitKey(KEY_INPUT_R);

    // TABキーで敵情報の表示を切り替える
    if (tab == 1 && oldTab == false)
    {
        showEnemyInfo = !showEnemyInfo;
    }

    // SPACEキー
    if (escape == 1 && oldEscape == false)
    {
        if (scene == 0)
        {
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
        if (enter == 1 && oldEnter == false)
        {
            assistSelect = 1;
            scene = 5;
        }
    }

    // CI補助選択画面
    else if (scene == 5)
    {
        if (up == 1 && oldUp == false)
        {
            assistSelect--;

            if (assistSelect < 0)
            {
                assistSelect = 1;
            }
        }

        if (down == 1 && oldDown == false)
        {
            assistSelect++;

            if (assistSelect > 1)
            {
                assistSelect = 0;
            }
        }

        if (enter == 1 && oldEnter == false)
        {
            useCIAssist = (assistSelect == 0);

            select = 0;

            // 敵の行動を事前決定
            battle.PrepareTurn();

            scene = 1;
        }
    }

    // 奇策選択画面
    else if (scene == 6)
    {
        // Rキーで戦略選択画面に戻る
        if (r == 1 && oldR == false)
        {
            scene = 1;
        }
        else
        {
            bool canUseSeal = CanUseSeal();

            // 封印が使えない場合は回復に固定
            if (!canUseSeal)
            {
                trickSelect = 0;
            }

            if (up == 1 && oldUp == false)
            {
                if (canUseSeal)
                {
                    trickSelect--;

                    if (trickSelect < 0)
                    {
                        trickSelect = 1;
                    }
                }
            }

            if (down == 1 && oldDown == false)
            {
                if (canUseSeal)
                {
                    trickSelect++;

                    if (trickSelect > 1)
                    {
                        trickSelect = 0;
                    }
                }
            }

            if (enter == 1 && oldEnter == false)
            {
                battle.SetTrickChoice(trickSelect);

                // HP回復
                if (trickSelect == 0)
                {
                    StartChargeImpact();
                }
                // 能力封印
                else if (trickSelect == 1 && canUseSeal)
                {
                    CreateSealCandidates();

                    if (sealCandidateCount > 0)
                    {
                        sealSelect = 0;
                        scene = 7;
                    }
                    else
                    {
                        trickSelect = 0;
                        battle.SetTrickChoice(0);

                        StartChargeImpact();
                    }
                }
            }
        }
    }

    // 封印する能力の選択画面
    else if (scene == 7)
    {
        // Rキーで奇策選択画面に戻る
        if (r == 1 && oldR == false)
        {
            scene = 6;
        }
        else
        {
            if (up == 1 && oldUp == false)
            {
                sealSelect--;

                if (sealSelect < 0)
                {
                    sealSelect = sealCandidateCount - 1;
                }
            }

            if (down == 1 && oldDown == false)
            {
                sealSelect++;

                if (sealSelect >= sealCandidateCount)
                {
                    sealSelect = 0;
                }
            }

            if (enter == 1 && oldEnter == false)
            {
                battle.SetDisableAction(sealCandidates[sealSelect]);

                StartChargeImpact();
            }
        }
    }

    // 戦略選択画面
    else if (scene == 1)
    {
        if (up == 1 && oldUp == false)
        {
            select--;

            if (select < 0)
            {
                select = 2;
            }
        }

        if (down == 1 && oldDown == false)
        {
            select++;

            if (select > 2)
            {
                select = 0;
            }
        }

        if (enter == 1 && oldEnter == false)
        {
            selectedStrategy = select;

            battle.SetPlayerStrategy(
                static_cast<StrategyType>(selectedStrategy));

            // 奇策を選択
            if (selectedStrategy == 2)
            {
                trickSelect = 0;
                scene = 6;
            }
            else
            {
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

                chargeInpact.RetryAfterTie();
            }
        }
        // まだ勝敗が決まっていない場合
        else if (!resultSent)
        {
            // プレイヤーのゲージ更新
            chargeInpact.Update();

            // CPUのゲージ更新
            chargeInpact.UpdateCPU();

            // 両方のゲージが停止したら判定
            if (chargeInpact.IsStopped() &&
                chargeInpact.IsCPUStopped())
            {
                // あいこ
                if (chargeInpact.GetScore() ==
                    chargeInpact.GetCPUScore())
                {
                    isTie = true;
                    isTieWaiting = true;

                    tieWaitStartTime = GetNowCount();
                }
                else
                {
                    // CIの勝敗を決定
                    bool playerWin =
                        chargeInpact.GetScore() >
                        chargeInpact.GetCPUScore();

                    battle.SetChargeImpactResult(playerWin);

                    // 戦闘処理
                    battle.ResolveBattle();

                    isTie = false;
                    resultSent = true;

                    // 勝敗表示を開始した時間を記録
                    resultStartTime = GetNowCount();
                }
            }
        }

        // 勝敗表示から1.5秒経過したら結果画面へ
        if (resultSent &&
            GetNowCount() - resultStartTime >= 1500)
        {
            // 結果画面に入った時間を記録
            resultStartTime = GetNowCount();

            scene = 3;
        }
    }

    // 戦闘結果画面
    else if (scene == 3)
    {
        // ENTERキー
        if (enter == 1 && oldEnter == false)
        {
            // 戦闘が終了している場合
            if (battle.IsBattleEnd())
            {
                // 新しい戦闘を開始できる状態にする
                battle.Init();
                chargeInpact.Init();

                assistSelect = 1;
                select = 0;

                showEnemyInfo = false;

                resultSent = false;

                scene = 0;
            }
            else
            {
                // まだ戦闘が続いている場合
                select = 0;

                chargeInpact.Init();

                resultSent = false;

                // 次のターンの敵の行動を事前決定
                battle.PrepareTurn();

                scene = 1;
            }
        }

        // 戦闘終了後、10秒経過した場合
        if (battle.IsBattleEnd() &&
            GetNowCount() - resultStartTime >= 10000)
        {
            battle.Init();
            chargeInpact.Init();

            assistSelect = 1;
            select = 0;

            showEnemyInfo = false;

            resultSent = false;

            scene = 0;
        }
    }

    // 前回のキー状態を保存
    oldEnter = enter;
    oldUp = up;
    oldDown = down;
    oldEscape = escape;
    oldTab = tab;
    oldR = r;

    return true;
}

// ゲームの描画
void Game::Draw()
{
    ClearDrawScreen();

    SetFontSize(36);


    int backgroundHandle = -1;

    // タイトル以外の画面に背景を表示
    if (scene == 1 ||
        scene == 2 ||
        scene == 3 ||
        scene == 5 ||
        scene == 6 ||
        scene == 7)
    {
        if (battleBgHandle != -1)
        {
            // 背景画像を画面いっぱいに描画
            DrawExtendGraph(
                0, 0,
                1920, 1080,
                LoadGraph(L"images/battle_bg.png"),
                FALSE
            );
        }
    }

    // タイトル画面
    if (scene == 0)
    {
        // 背景画像を画面いっぱいに描画
        if (titleBgHandle != -1)
        {

            //背景画像を読み込ませるなら
                backgroundHandle = LoadGraph(L"images/title_bg.png");
                // 背景画像を画面いっぱいに表示
                DrawExtendGraph(
                    0, 0,
                    1920, 1080,
                    backgroundHandle,
                    TRUE
                );
        
        }

        // ゲームタイトル
        SetFontSize(65);

        DrawString(700, 400, 
            L"STRATEGY バトル",
            GetColor(255, 255, 255));

        // スタート案内
        SetFontSize(45);

        DrawString(640, 600,
            L"Zキーを押してゲームスタート",
            GetColor(255, 230, 50));

        SetFontSize(36);
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

        DrawString(650, 600,
            L"ON : プレイヤーのCI速度が20%低下",
            GetColor(200, 200, 200));

        DrawString(700, 850,
            L"Wキー/Sキー:選択　Zキー:決定",
            GetColor(200, 200, 200));
    }

    // 奇策選択画面
    else if (scene == 6)
    {
        bool canUseSeal = CanUseSeal();

        DrawString(650, 220,
            L"奇策を選択してください",
            GetColor(255, 255, 255));

        // HP回復
        DrawString(750, 380,
            trickSelect == 0 ? L"> HP回復" : L"  HP回復",
            GetColor(100, 255, 100));

        // 能力封印
        if (canUseSeal)
        {
            DrawString(750, 480,
                trickSelect == 1 ? L"> 能力封印" : L"  能力封印",
                GetColor(255, 220, 100));

            DrawString(1050, 480,
                L"使用可能",
                GetColor(100, 255, 100));
        }
        else
        {
            DrawString(750, 480,
                L"  能力封印",
                GetColor(120, 120, 120));

            DrawString(1050, 480,
                L"使用不可",
                GetColor(255, 80, 80));
        }

        DrawFormatString(770, 600,
            GetColor(255, 255, 255),
            L"封印使用回数 : %d / 5",
            battle.GetSealUseCount());

        DrawString(600, 850,
            L"Wキー/Sキー:選択　Zキー:決定  Rキー:戻る",
            GetColor(200, 200, 200));
    }

    // 封印する能力の選択画面
    else if (scene == 7)
    {
        DrawString(650, 180,
            L"封印する敵の能力を選択",
            GetColor(255, 255, 255));

        for (int i = 0; i < sealCandidateCount; i++)
        {
            DrawString(700, 300 + i * 80,
                GetAbilityName(sealCandidates[i]),
                GetColor(255, 255, 255));

            if (i == sealSelect)
            {
                DrawString(650, 300 + i * 80,
                    L">",
                    GetColor(255, 255, 0));
            }
        }

        DrawString(600, 850,
            L"Wキー/Sキー:選択　Zキー:決定  Rキー:戻る",
            GetColor(200, 200, 200));
    }

    // 戦略選択画面
    else if (scene == 1)
    {
        DrawString(300, 235,
            L"戦略を選択してください",
            GetColor(255, 255, 255));

        DrawString(360, 360,
            select == 0 ? L"> 攻撃" : L"  攻撃",
            GetColor(255, 100, 100));

        DrawString(360, 472,
            select == 1 ? L"> 防御" : L"  防御",
            GetColor(100, 200, 255));

        DrawString(360, 585,
            select == 2 ? L"> 奇策" : L"  奇策",
            GetColor(255, 220, 100));

        // 敵の行動表示
        SetFontSize(30);

        DrawString(300, 680,
            L"【敵の行動】",
            GetColor(255, 220, 100));

        DrawString(300, 730,
            battle.GetEnemyActionHint(
                static_cast<StrategyType>(select)),
            GetColor(255, 255, 255));

        SetFontSize(36);

        // プレイヤーと敵のHP表示
        SetFontSize(42);

        // プレイヤーHP（左側・青色）
        DrawFormatString(240, 100,
            GetColor(100, 200, 255),
            L"PLAYER HP : %d / %d",
            battle.GetPlayer().GetHP(),
            battle.GetPlayer().GetMaxHP());

        // 敵HP（右側・赤色）
        DrawFormatString(1250, 100,
            GetColor(255, 100, 100),
            L"ENEMY HP : %d / %d",
            battle.GetEnemy().GetHP(),
            battle.GetEnemy().GetMaxHP());

        // 文字サイズを元に戻す
        SetFontSize(36);

        DrawString(1050, 430,
            L"W / S : 選択",
            GetColor(200, 200, 200));

        DrawString(1050, 500,
            L"ESC : タイトルに戻る",
            GetColor(200, 200, 200));

        DrawString(1050, 570,
            L"TAB : 敵の詳細情報を表示 / 非表示",
            GetColor(200, 200, 200));

        // 敵の詳細情報
        if (showEnemyInfo)
        {
            SetFontSize(24);

            Enemy& enemy = battle.GetEnemy();

            // 敵の情報
            DrawString(1050, 640,
                L"【敵の情報】",
                GetColor(255, 100, 100));

            DrawString(1050, 680,
                L"攻撃・防御・回復・CI妨害を備えた",
                GetColor(255, 255, 255));

            DrawString(1050, 715,
                L"バランス型の敵。状況に応じて行動する。",
                GetColor(255, 255, 255));

            // 敵の所持能力
            DrawString(1050, 775,
                L"【敵の所持能力】",
                GetColor(255, 220, 100));

            for (int i = 0; i < 4; i++)
            {
                EnemyAbility ability =
                    static_cast<EnemyAbility>(i);

                if (enemy.HasAbility(ability))
                {
                    int y = 815 + i * 35;

                    DrawString(1050, y,
                        GetAbilityName(ability),
                        GetColor(255, 255, 255));

                    if (enemy.IsAbilitySealed(ability))
                    {
                        DrawFormatString(1250, y,
                            GetColor(255, 100, 100),
                            L"封印中（残り%dターン）",
                            enemy.GetSealTurns(ability));
                    }
                    else
                    {
                        DrawString(1250, y,
                            L"使用可能",
                            GetColor(100, 255, 100));
                    }
                }
            }

            // 戦闘履歴
            DrawString(1050, 970,
                L"【直近の戦闘履歴】",
                GetColor(100, 200, 255));

            // 実際に表示した履歴の数
            int drawCount = 0;

            for (int i = 0; i < 3; i++)
            {
                const wchar_t* log =
                    battle.GetBattleLog(i);

                // 履歴が存在する場合
                if (log[0] != L'\0')
                {
                    DrawString(1050, 1005 + drawCount * 25,
                        log,
                        GetColor(220, 220, 220));

                    drawCount++;
                }
            }

            // 履歴が1件もない場合
            if (drawCount == 0)
            {
                DrawString(1050, 1005,
                    L"まだ戦闘履歴はありません",
                    GetColor(180, 180, 180));
            }

            SetFontSize(36);
        }
    }

    // CI画面
else if (scene == 2)
{
    // タイトル
    SetFontSize(48);

    DrawString(700, 140,
        L"CHARGE IMPACT",
        GetColor(255, 255, 255));

    SetFontSize(36);

    // プレイヤー側のゲージ
    chargeInpact.Draw();

    // CPU側のゲージ
    chargeInpact.DrawCPU();

    // あいこ時のメッセージ
    if (isTie)
    {
        SetFontSize(44);

        DrawString(800, 700,
            L"あいこだ！！",
            GetColor(255, 255, 0));

        DrawString(500, 850,
            L"もう一度ゲージを止めて決着をつけよう！！",
            GetColor(255, 255, 255));

        SetFontSize(36);
    }

    // 操作説明
    DrawExtendString(600, 610,
        1.2f, 1.2f,
        L"SPACEキーを押してゲージを止めろ！！",
        GetColor(255, 255, 0));

    // HP表示
    SetFontSize(42);

    DrawFormatString(350, 700,
        GetColor(100, 200, 255),
        L"PLAYER HP : %d",
        battle.GetPlayer().GetHP());

    DrawFormatString(1250, 700,
        GetColor(255, 100, 100),
        L"ENEMY HP : %d",
        battle.GetEnemy().GetHP());

    // 残り時間表示
    if (!resultSent && !isTie)
    {
        SetFontSize(48);

        DrawFormatString(730, 770,
            GetColor(255, 255, 100),
            L"残り時間 : %.1f 秒",
            chargeInpact.GetRemainingTime());
    }

    SetFontSize(36);

    // CI終了後の勝敗表示
    if (resultSent)
    {
        SetFontSize(60);

        // プレイヤーがCIに勝った場合
        if (chargeInpact.GetScore() >
            chargeInpact.GetCPUScore())
        {
            DrawString(300, 850,
                L"CI WIN!!",
                GetColor(255, 255, 0));

            DrawString(1250, 850,
                L"CI LOSE...",
                GetColor(255, 100, 100));
        }
        // プレイヤーがCIに負けた場合
        else
        {
            DrawString(300, 850,
                L"CI LOSE...",
                GetColor(255, 100, 100));

            DrawString(1250, 850,
                L"CI WIN!!",
                GetColor(255, 255, 0));
        }

        SetFontSize(36);
    }
}
    // 戦闘結果画面
    else if (scene == 3)
    {
        DrawString(820, 120,
            L"バトル結果",
            GetColor(255, 255, 255));

        // プレイヤー側
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

        // 敵側
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
        if (chargeInpact.GetScore() >
            chargeInpact.GetCPUScore())
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

        // 戦闘の勝敗
        if (battle.GetEnemy().IsDefeated())
        {
            SetFontSize(60);

            DrawString(760, 670,
                L"PLAYERの勝ち！！",
                GetColor(255, 255, 0));

            SetFontSize(36);

            DrawString(800, 800,
                L"Zキー : タイトルに戻る",
                GetColor(200, 200, 200));
        }
        else if (battle.GetPlayer().IsDefeated())
        {
            SetFontSize(60);

            DrawString(760, 670,
                L"ENEMYの勝ち！！",
                GetColor(255, 100, 100));

            SetFontSize(36);

            DrawString(800, 800,
                L"Zキー : タイトルに戻る",
                GetColor(200, 200, 200));
        }
        else
        {
            DrawString(800, 680,
                L"次の戦略を選択",
                GetColor(255, 255, 255));

            DrawString(800, 800,
                L"Zキー : 続ける",
                GetColor(200, 200, 200));
        }
    }
}

// 封印できる能力を調べる
void Game::CreateSealCandidates()
{
    sealCandidateCount = 0;

    // 敵の4種類の能力を確認
    for (int i = 0; i < 4; i++)
    {
        EnemyAbility ability =
            static_cast<EnemyAbility>(i);

        // 敵が持っていて封印されていない能力
        if (battle.GetEnemy().HasAbility(ability) &&
            !battle.GetEnemy().IsAbilitySealed(ability))
        {
            sealCandidates[sealCandidateCount] = ability;

            sealCandidateCount++;
        }
    }
}

// チャージインパクトを開始
void Game::StartChargeImpact()
{
    // 敵の行動を決定
    battle.StartTurn();

    // CIゲージを初期化
    chargeInpact.Init();

    // CIの基本速度
    chargeInpact.SetSpeed(40.0f);

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
        // プレイヤーのCI速度を10%低下
        playerSpeedRate *= 1.2f;
    }

    // プレイヤーのCI速度に反映
    chargeInpact.SetPlayerSpeedRate(playerSpeedRate);

    // CPUの難易度
    chargeInpact.SetCPUTargetRange(75, 100);

    resultSent = false;

    isTie = false;
    isTieWaiting = false;

    // CI画面へ
    scene = 2;
}