#include"DxLib.h"
#include"Run.h"	// ヘッダーファイルをインクルード

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720;	// ウィンドウの幅と高さのピクセル数
const int FPS = 60;
const int DISTANCE_MAX = 5000;	// 耐えるとクリアとなるまでの時間　ゲーム内表記は5000m
enum { NOON, EVENING, NIGHT, MORNING };	// 時間帯
enum { TITLE, PLAY, OVER, CLEAR };

// グローバル変数
int img10Right,img07Right,img04Right, img10Left,img07Left,img04Left,imgMissLeft,imgR10Right,imgR07Right,imgR04Right,imgStop,img10Segway,img05Segway;	// プレイヤーの画像
int img0Sliding, img1Sliding, img2Sliding, img3Sliding;	// プレイヤーのスライディングアニメーション

int imgPharaoh, imgPharaohMiss,imgPharaohBlack,imgPharaohMBlack,imgPharaohWhite,imgPharaohMWhite,imgPharaohRed,imgPharaohMRed,imgBeam;	// ファラオ(追いかけてくる敵)の画像

int imgGround, imgSand, imgPyramid,imgSun,imgMoon;	// 背景の画像
int imgGroundN, imgSandN, imgPyramidN,imgStarN;	// 背景(夜)
int imgEveningSky,imgEveSun;	// 背景(夕)

int seMetronome, seTempo, seClick,seCharge,seBeam,seMiss,seSliding,seBeamMiss;	// 使用する効果音
//int bgmNoon, bgmEvening, bgmNight, bgmMorning;	// BGM

// 画像代入用
int playerDrawImg;	// プレイヤーの現在の画像
int ground, sand, pyramid,sunMoon;	// 背景の現在の画像
int pharaoh = imgPharaoh;	// ファラオの現在の画像

int pushS;	// Sキーが押された、離された瞬間を獲得するための変数
int pushD;	// Dキー

int sandX = 0;	// 背景を動かす際のX座標
int pyramidX = 0;

//太陽または月の位置、大きさを代入
int celestialX = 0;	// X座標
int celestialY = 0;	// Y座標
int celestialSiz = 0;	// 大きさ

int speed = 0;	// プレイヤーや背景の動くスピード
int drawSpeed = 50;	// プレイヤーのアニメーションがこの範囲で描かれる　200からspeedを引いた値
int timer = 0;
int rhythm = 2;	// drawSpeedごとに音が鳴る頻度
int run = 1500;	// プレイヤーの位置を決める値
int count = 0;	// プレイヤーのアニメーションが何周したかをカウントし時間帯を切り替える
int distance = 0;	// クリアまでの距離を表す
int pharaohCount = 7;	// ファラオがビームを放つ間隔
int pharaohX = 100;	// ファラオのX座標
int zoneChangeSpan = 32;	// ゾーンが切り替わるスパン

int timeZone = NOON;	// 時間帯
int scene = TITLE;

//bool sun = true;	// 時間帯に合わせて太陽を月に切り替える
bool dush = true;	//  前回ミスしていないかどうかを判定する
bool pharaohMiss = false;	// ファラオがミス状態になっているか
bool sliding = false;	// スライディングしているか
bool slidingCount = false;	// スライディング前半後半切り替え用
bool slidingCushion = true;	// slidingCountの挙動をいったん抑える
bool pharaohUp = true;	// ファラオが上に動くか下に動くかを切り替える

// SEを一回鳴らすための旗
bool seFlag1 = true;
bool seFlag2 = true;
bool seFlag3_1 = true;
bool seFlag3_2 = true;
bool seFlag4_1 = true;
bool seFlag4_2 = true;
bool slidingMissSeFlag = true;


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("逃走ゲーム");	// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);	// ウィンドウモードで起動
	if (DxLib_Init() == -1)return -1;	// ライブラリ初期化　エラーが起きたら終了
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする
	InitGame();	// 初期化用の関数を呼び出す
	//PlaySoundMem(bgmNoon, DX_PLAYTYPE_LOOP);	// BGMループ出力

	while(1)
	{
		ClearDrawScreen();
		switch (scene)
		{
		case TITLE:	// タイトル画面
			BackGround();
			DrawText(WIDTH*0.15, HEIGHT*0.7, "Press S key to start", true, 0xffffff, 80);
			DrawImageEnlarge(imgStop, WIDTH / 2 - (500 - run) / 10, HEIGHT / 2, 8, 8);
			if (CheckHitKey(KEY_INPUT_S))
			{
				InitVariable();
				scene = PLAY;
			}
			break;

		case PLAY:	// ゲームプレイ画面
			BackGround();
			
			timer++;
			drawSpeed = 200 - speed;	// 200からスピード値を引いた時間で画像が一周する
			if (sliding == false) {
				if (timer % drawSpeed > (drawSpeed / 6) * 5)playerDrawImg = imgR04Right;
				else if (timer % drawSpeed > (drawSpeed / 6) * 4)playerDrawImg = imgR07Right;
				else if (timer % drawSpeed > (drawSpeed / 6) * 3)playerDrawImg = imgR10Right;
				else if (timer % drawSpeed > (drawSpeed / 6) * 2)playerDrawImg = img04Left;
				else if (timer % drawSpeed > (drawSpeed / 6) * 1)playerDrawImg = img07Left;
				else if (timer % drawSpeed > 0)					 playerDrawImg = img10Left;
			}

			if (slidingCount == false && sliding == true && slidingCushion == true) {	// スライディング前半　すぐに次に切り替わるので一瞬で終わる
				if (timer % drawSpeed > (drawSpeed / 6) * 1)	 playerDrawImg = img1Sliding;
				else if (timer % drawSpeed > 0)					 playerDrawImg = img0Sliding;
			}
			if (slidingCount == false && sliding == true && slidingCushion == false) {	// スライディング中盤　ずっとスライディング状態
				playerDrawImg = img1Sliding;
			}
			if (slidingCount == true && sliding == true) {	// スライディング終盤　元のモーションへ戻る
				if (timer % drawSpeed > (drawSpeed / 6) * 5)	 playerDrawImg = img04Right;
				else if (timer % drawSpeed > (drawSpeed / 6) * 3)playerDrawImg = img3Sliding;
				else if (timer % drawSpeed > (drawSpeed / 6) * 1)playerDrawImg = img2Sliding;
				else if (timer % drawSpeed < (drawSpeed / 6) * 1)	 playerDrawImg = img1Sliding;
			}
			if (timer % drawSpeed == 0)
			{
				count++;	// プレイヤーの動き一周毎にカウント

				seFlag1 = true, seFlag2 = true, seFlag3_1 = true, seFlag3_2 = true, seFlag4_1 = true, seFlag4_2 = true, slidingMissSeFlag = true;	// Seフラッグのリセット

				if (pharaohCount > 3) {
					if (pharaohUp == true)pharaohUp = false;	// ファラオの上下を切り替え
					else if (pharaohUp == false)pharaohUp = true;
				}
				if (sliding == true && slidingCount == false && slidingCushion == true)slidingCushion = false;
				else if (sliding == true && slidingCount == false)slidingCount = true;
				else if (sliding == true && slidingCount == true)sliding = false, playerDrawImg = img04Right, slidingCushion = true;
				pharaohCount--;
				if (pharaohCount == 2 || pharaohCount == 1)	PlaySoundMem(seCharge, DX_PLAYTYPE_BACK);
				if (pharaohCount == 0)	PlaySoundMem(seBeam, DX_PLAYTYPE_BACK);
			}


			if (dush == false && timer % drawSpeed < (drawSpeed / 6) * 2 && slidingCount == false)playerDrawImg = imgMissLeft;

			if (count == zoneChangeSpan)	// 【仮】16周すると時間帯を切り替える
			{
				timeZone++;
				if (timeZone == 4)timeZone = 0;
				count = 0;
				if (timeZone == NOON)rhythm = 2, speed = 150,zoneChangeSpan/=2; //StopSoundMem(bgmMorning),PlaySoundMem(bgmNoon, DX_PLAYTYPE_LOOP);
				if (timeZone == EVENING)rhythm = 3, speed = 130; //StopSoundMem(bgmNoon), PlaySoundMem(bgmEvening, DX_PLAYTYPE_LOOP);
				if (timeZone == NIGHT)rhythm = 3, speed = 160;//StopSoundMem(bgmEvening), PlaySoundMem(bgmNight, DX_PLAYTYPE_LOOP);
				if (timeZone == MORNING)rhythm = 4, speed = 140; //StopSoundMem(bgmNight), PlaySoundMem(bgmMorning, DX_PLAYTYPE_LOOP);
				if (timeZone == NOON && zoneChangeSpan < 2)scene=TITLE;
			}

			if (seFlag1 == true && timer % drawSpeed >= ((drawSpeed / 6) * 5.5)) {	// 定期的に音を鳴らす
				if (sliding == true)PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK);
				else PlaySoundMem(seTempo, DX_PLAYTYPE_BACK);
				seFlag1 = false;
			}
			if (rhythm % 2 == 0) {	// リズムが2で割り切れるとき
				if (seFlag2 == true && timer % drawSpeed >= ((drawSpeed / 6) * 2.5))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK), seFlag2 = false;
			}
			if (rhythm % 3 == 0) {	// リズムが3で割り切れるとき
				if (seFlag3_1 == true && timer % drawSpeed >= ((drawSpeed / 6) * 1.5))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK), seFlag3_1 = false;
				if (seFlag3_2 == true && timer % drawSpeed >= ((drawSpeed / 6) * 3.5))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK), seFlag3_2 = false;
			}
			if (rhythm % 4 == 0) {	// リズムが4で割り切れるとき
				if (seFlag4_1 == true && timer % drawSpeed >= ((drawSpeed / 6) * 1))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK), seFlag4_1 = false;
				if (seFlag4_2 == true && timer % drawSpeed >= ((drawSpeed / 6) * 4))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK), seFlag4_2 = false;
			}


			if (timeZone == NIGHT)SetDrawBright(100, 100, 100);
			DrawImageEnlarge(playerDrawImg, WIDTH / 2 - (500 - run) / 10, HEIGHT / 2, 8, 8);	// 元の画像が小さいので8倍で出力

			// ファラオ　プレイヤーのアニメーション一周ごとに切り替わるboolで上下させる
			SetDrawBright(255, 255, 255);

			if (pharaohMiss == true)
			{
				if (pharaohCount > 2)	pharaoh = imgPharaohMiss, pharaohX = 100;
				if (pharaohCount == 2)	pharaoh = imgPharaohMBlack, pharaohX = 80;
				if (pharaohCount == 1)	pharaoh = imgPharaohMWhite, pharaohX = 60;
				if (pharaohCount == 0)	pharaoh = imgPharaohMRed, pharaohX = 40;
				if (pharaohCount == -1)	pharaoh = imgPharaohMiss, pharaohX = 100, pharaohCount = 7;
			}
			else
			{
				if (pharaohCount > 2)	pharaoh = imgPharaoh, pharaohX = 100;
				if (pharaohCount == 2)	pharaoh = imgPharaohBlack, pharaohX = 80;
				if (pharaohCount == 1)	pharaoh = imgPharaohWhite, pharaohX = 60;
				if (pharaohCount == 0)	pharaoh = imgPharaohRed, pharaohX = 40;
				if (pharaohCount == -1)	pharaoh = imgPharaoh, pharaohX = 100, pharaohCount = 7;
			}

			if (pharaohCount > 3) {
				if (pharaohUp == true)	DrawImageEnlarge(pharaoh, pharaohX, HEIGHT / 2, 8, 8);
				else					DrawImageEnlarge(pharaoh, pharaohX, HEIGHT / 2 + 20, 8, 8);
			}
			else {
				if (pharaohUp == true) {	// ビーム準備に入ると位置を固定させる
					DrawImageEnlarge(pharaoh, pharaohX, HEIGHT / 2 + 20, 8, 8);
					if (pharaohCount == 0) {
						int w, h;
						GetGraphSize(imgBeam, &w, &h);
						DrawImageEnlarge(imgBeam, pharaohX, HEIGHT / 2 + 20, 20, 8);	// ビーム発射
						//DrawExtendGraph(pharaohX - (w / 2) * 20, HEIGHT/2+20 - (h / 2) * 8, pharaohX + (w / 2) * 20, HEIGHT/2+20 + (h / 2) * 8, imgBeam, true);
					}
				}
				else {
					DrawImageEnlarge(pharaoh, pharaohX, HEIGHT / 2, 8, 8);
					if (pharaohCount == 0) {
						int w, h;
						GetGraphSize(imgBeam, &w, &h);
						DrawImageEnlarge(imgBeam, pharaohX, HEIGHT / 2, 20, 8);
						//DrawExtendGraph(pharaohX - (w / 2) * 20, HEIGHT / 2 - (h / 2) * 8, pharaohX + (w / 2) * 20, HEIGHT / 2 + (h / 2) * 8, imgBeam, true);
					}
				}
			}


			// 正しいタイミングでSキーが押された瞬間に一回だけ進める
			if (timer % drawSpeed > (drawSpeed / 6) * 5 && pushS == 1 && dush == false)run += drawSpeed * 2, dush = true, pharaohMiss = false;
			// 正しいタイミングでDキーが押された瞬間にスライディングを行う
			if (pharaohCount == 1 && timer % drawSpeed > (drawSpeed / 6) * 5 && pushD == 1 && dush == false)sliding = true, dush = true, pharaohMiss = false, PlaySoundMem(seSliding, DX_PLAYTYPE_BACK);


			//if (timer % drawSpeed > (drawSpeed / 6) * 5 && pushD==-1&& slidingCount == true)slidingCount=false, run += drawSpeed * 2,dush = true, pharaoh = imgPharaoh;

			// Sキーが押されていなかった場合に少し後退させる
			if (timer % drawSpeed == 0 && dush == false && sliding == false && slidingCount == false)pharaohMiss = true, PlaySoundMem(seMiss, DX_PLAYTYPE_BACK);

			// スライディング失敗のアクション
			if (pharaohCount == 0 && sliding == false) {
				run -= 10, pharaohMiss = true;
				if (slidingMissSeFlag == true)PlaySoundMem(seBeamMiss, DX_PLAYTYPE_BACK), slidingMissSeFlag = false;
			}

			// 少し経った後にスライディングを終わらせる
			if (timer % drawSpeed < (drawSpeed / 6) * 5 && timer % drawSpeed >(drawSpeed / 6) * 2 && sliding == false)slidingCount = false;


			// 少し経った後に失敗モーションを終わらせる
			if (timer % drawSpeed < (drawSpeed / 6) * 5 && timer % drawSpeed >(drawSpeed / 6) * 2 && dush == true)dush = false;

			// 正しくないタイミングでキーが押された瞬間に少し後退させる
			if (timer % drawSpeed < (drawSpeed / 6) * 5 && timer % drawSpeed > 0 && pushS == 1)run -= drawSpeed * 2, pharaohMiss = true, PlaySoundMem(seMiss, DX_PLAYTYPE_BACK);
			if (timer % drawSpeed < (drawSpeed / 6) * 5 && timer % drawSpeed > 0 && pushD == 1 && pharaohCount > 0)run -= drawSpeed * 2, pharaohMiss = true, PlaySoundMem(seMiss, DX_PLAYTYPE_BACK);

			if (pharaoh == imgPharaohMiss)run -= 5;	// プレイヤーがミスを行った場合成功するまで後ろに引っ張る
			else if (run > 1000)run -= 2;	// プレイヤーが一定より前にいる場合後ろに引っ張り調整する
			else if (run > -1000) run -= 1;

			if (playerDrawImg == img1Sliding)run += 5;
			if (playerDrawImg == img2Sliding)run += 3;

			if (distance > 0)distance--;	// 残り距離を縮める
			if (distance > 500)DrawText(WIDTH / 2 - 150, HEIGHT - 200, "Finish:%dm", distance, 0xff1010, 50);	// ゴールまでを表すテキスト　ゴールに近づけば非表示になる
			if (distance < 0)distance = 0;

			// Sキーが押された瞬間かどうかを判定する
			if (CheckHitKey(KEY_INPUT_S) == 0)
			{
				if (pushS > 0) pushS = -1;
				else pushS = 0;
			}
			else
			{
				pushS++;
			}

			// Dキーが押された瞬間かどうかを判定する
			if (CheckHitKey(KEY_INPUT_D) == 0)
			{
				if (pushD > 0) pushD = -1;
				else pushD = 0;
			}
			else
			{
				pushD++;
			}

			if (run < -3500)
			{
				scene = TITLE;
				break;
			}
		break;
		}

		ScreenFlip();	// 裏画面の内容を表画面に反映させる
		WaitTimer(1000 / FPS);	// 一定時間待つ
		if (ProcessMessage() == -1)break;	// Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break;	// ESCキーが押されたら終了
	}
	DxLib_End();	// DXライブラリ使用の終了処理
	return 0;	// ソフトの終了
}

void InitGame(void)
{	

	distance = DISTANCE_MAX;
	// プレイヤーの画像読み込み
	img10Right = LoadGraphWithCheck("image/run1.0Right.png");
	img07Right = LoadGraphWithCheck("image/run0.7Right.png");
	img04Right = LoadGraphWithCheck("image/run0.4Right.png");
	img10Left = LoadGraphWithCheck("image/run1.0Left.png");
	img07Left = LoadGraphWithCheck("image/run0.7Left.png");
	img04Left = LoadGraphWithCheck("image/run0.4Left.png");
	imgR10Right = LoadGraphWithCheck("image/runR_1.0Right.png");
	imgR07Right = LoadGraphWithCheck("image/runR_0.7Right.png");
	imgR04Right = LoadGraphWithCheck("image/runR_0.4Right.png");
	imgMissLeft = LoadGraphWithCheck("image/runMissLeft.png");
	imgStop = LoadGraphWithCheck("image/runStop.png");
	img0Sliding = LoadGraphWithCheck("image/run0Sliding.png");
	img1Sliding = LoadGraphWithCheck("image/run1Sliding.png");
	img2Sliding = LoadGraphWithCheck("image/run2Sliding.png");
	img3Sliding = LoadGraphWithCheck("image/run3Sliding.png");

	// ファラオ（追いかけてくる敵）の画像読み込み
	imgPharaoh = LoadGraphWithCheck("image/runPharaohNormal.png");
	imgPharaohMiss = LoadGraphWithCheck("image/runPharaohMiss.png");
	imgPharaohBlack = LoadGraphWithCheck("image/runPharaohBlackCharge.png");
	imgPharaohWhite = LoadGraphWithCheck("image/runPharaohWhiteCharge.png");
	imgPharaohRed = LoadGraphWithCheck("image/runPharaohRedCharge.png");
	imgBeam = LoadGraphWithCheck("image/runPharaohBeam.png");
	imgPharaohMBlack = LoadGraphWithCheck("image/runPharaohBlackMiss.png");
	imgPharaohMWhite = LoadGraphWithCheck("image/runPharaohWhiteMiss.png");
	imgPharaohMRed = LoadGraphWithCheck("image/runPharaohRedMiss.png");

	// 背景の画像読み込み
	imgGround = LoadGraphWithCheck("image/runGround.png");
	imgSand = LoadGraphWithCheck("image/runSand.png");
	imgPyramid = LoadGraphWithCheck("image/runPyramid.png");
	imgSun = LoadGraphWithCheck("image/runSun.png");
	imgMoon = LoadGraphWithCheck("image/runMoon.png");

	// 夜
	imgGroundN = LoadGraphWithCheck("image/runGroundNight.png");
	imgSandN = LoadGraphWithCheck("image/runSandNight.png");
	imgPyramidN = LoadGraphWithCheck("image/runPyramidNight.png");
	imgStarN = LoadGraphWithCheck("image/runStarNight.png");

	// 夕
	imgEveningSky = LoadGraphWithCheck("image/runEvening.png");
	imgEveSun = LoadGraphWithCheck("image/runEveningSun.png");

	// サウンドの読み込みと音量設定
	seMetronome = LoadSoundMemWithCheck("sound/AS_メトロノーム.mp3");
	seTempo = LoadSoundMemWithCheck("sound/AS_カッ.mp3");
	seClick = LoadSoundMemWithCheck("sound/AS_足音.wav");
	seCharge = LoadSoundMemWithCheck("sound/AS_警告音.mp3");
	seBeam = LoadSoundMemWithCheck("sound/AS_発射音.mp3");
	seMiss = LoadSoundMemWithCheck("sound/AS_ポヨン.wav");
	seSliding = LoadSoundMemWithCheck("sound/AS_シュー.wav");
	seBeamMiss = LoadSoundMemWithCheck("sound/AS_BeamMiss.wav");

	//ChangeVolumeSoundMem(64, seMetronome);
	//ChangeVolumeSoundMem(64, seTempo);
	//ChangeVolumeSoundMem(64, seSliding);
	//ChangeVolumeSoundMem(100, seMiss);

	//bgmNoon = LoadSoundMemWithCheck("sound/AS_BGM_エジプト.wav");
	//bgmEvening = LoadSoundMemWithCheck("sound/AS_BGM_オーケストラ.wav");
	//bgmNight = LoadSoundMemWithCheck("sound/AS_BGM_ギターポップス.wav");
	//bgmMorning = LoadSoundMemWithCheck("sound/AS_BGM_スラップハウス.wav");
}

void InitVariable(void)
{

	 sandX = 0;	// 背景を動かす際のX座標
	pyramidX = 0;
	 speed = 150;	// プレイヤーや背景の動くスピード
	drawSpeed = 50;	// プレイヤーのアニメーションがこの範囲で描かれる　200からspeedを引いた値
	timer = 0;
	rhythm = 2;	// drawSpeedごとに音が鳴る頻度
	run = 1500;	// プレイヤーの位置を決める値
	count = 0;	// プレイヤーのアニメーションが何周したかをカウントし時間帯を切り替える
	distance = 0;	// クリアまでの距離を表す
	pharaohCount = 7;	// ファラオがビームを放つ間隔
	pharaohX = 100;	// ファラオのX座標
	zoneChangeSpan = 32;
	timeZone = NOON;	// 時間帯
	
	dush = true;	//  前回ミスしていないかどうかを判定する
	pharaohMiss = false;	// ファラオがミス状態になっているか
	sliding = false;	// スライディングしているか
	slidingCount = false;	// スライディング前半後半切り替え用
	slidingCushion = true;	// slidingCountの挙動をいったん抑える
	pharaohUp = true;	// ファラオが上に動くか下に動くかを切り替える

	// SEを一回鳴らすための旗
	seFlag1 = true;
	seFlag2 = true;
	seFlag3_1 = true;
	seFlag3_2 = true;
	seFlag4_1 = true;
	seFlag4_2 = true;
	slidingMissSeFlag = true;

}

void BackGround(void)
{
	// 背景を表示する処理
	if (timeZone == NOON)
	{
		sunMoon = imgSun;
		ground = imgGround;
		pyramid = imgPyramid;
		sand = imgSand;

		celestialX = 600;
		celestialY = 80;
		celestialSiz = 5;
		SetBackgroundColor(100, 200, 255);	// 背景色の指定
	}

	if (timeZone == EVENING)
	{
		sunMoon = imgEveSun;
		ground = imgGround;
		pyramid = imgPyramid;
		sand = imgSand;

		celestialX = 1100;
		celestialY = 420;
		celestialSiz = 8;
		DrawImageEnlarge(imgEveningSky, WIDTH / 2, HEIGHT / 2, 8, 8);	// 夕方の空
		SetBackgroundColor(255, 100, 0);	// 背景色の指定
	}

	if (timeZone == NIGHT)
	{
		sunMoon = imgMoon;
		ground = imgGroundN;
		pyramid = imgPyramidN;
		sand = imgSandN;

		celestialX = 200;
		celestialY = 120;
		celestialSiz = 3;
		DrawImageEnlarge(imgStarN, WIDTH / 2, HEIGHT / 2, 8, 8);	// 星
		SetBackgroundColor(0, 10, 180);	// 背景色の指定
	}

	if (timeZone == MORNING)
	{
		sunMoon = imgSun;
		ground = imgGround;
		pyramid = imgPyramid;
		sand = imgSand;

		celestialX = 200;
		celestialY = 100;
		celestialSiz = 5;
		SetBackgroundColor(100, 180, 255);	// 背景色の指定
	}
	DrawImageEnlarge(sunMoon, celestialX, celestialY, celestialSiz, celestialSiz);	// 昼なら太陽、夜なら月を表示する
	if (timeZone == NIGHT)SetDrawBright(150, 150, 150);
	if (timeZone == EVENING)SetDrawBright(255, 100, 0);

	pyramidX = (pyramidX - speed / 15);	//ピラミッド
	DrawExtendGraph(800 + pyramidX / 100, 200, 1300 + pyramidX / 100, 450, pyramid, true);

	DrawImageEnlarge(ground, WIDTH / 2, (HEIGHT / 2) + 250, 8, 8);	// 地面

	sandX = (sandX - speed / 50) % WIDTH;	// 砂の部分
	DrawExtendGraph(sandX, 450, WIDTH + sandX, 500, sand, true);
	DrawExtendGraph(WIDTH + sandX, 450, WIDTH * 2 + sandX, 500, sand, true);
}

// 中心座標を指定して画像を表示する関数
void DrawImageEnlarge(int img,int x,int y,int enlargeX,int enlargeY)
{
	int w, h;
	GetGraphSize(img, &w, &h);
	//DrawGraph(x - w / 2, y - h / 2, img, true);
	DrawExtendGraph(x - (w/2)*enlargeX, y - (h/2)*enlargeY, x + (w/2)*enlargeX, y + (h/2)*enlargeY, img, true);
}

// 影をつけた文字列を表示する関数
void DrawText(int x, int y, const char* txt, int val, int col, int siz)
{
	SetFontSize(siz);	// フォントの大きさを指定
	DrawFormatString(x + 1, y + 1, 0x000000, txt, val);	// 黒で文字列を表示
	DrawFormatString(x, y, col, txt, val);
}

int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "画像読み込みに失敗", MB_OK | MB_ICONSTOP); }
	return res;
}
int LoadSoundMemWithCheck(const char* file)
{
	int res = LoadSoundMem(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "音声読み込みに失敗", MB_OK | MB_ICONSTOP); }
	return res;
}