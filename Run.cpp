#include"DxLib.h"
#include"Run.h"	// ヘッダーファイルをインクルード

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720;	// ウィンドウの幅と高さのピクセル数
const int FPS = 60;

// グローバル変数
int img10Right,img07Right,img04Right, img10Left,img07Left,img04Left,imgStop,img10Segway,img05Segway;	// プレイヤーの画像
int imgRunningMachine;	// ランニングマシンの画像
int imgGround, imgSand, imgPyramid,imgSun,imgMoon;	// 背景の画像
int imgGroundN, imgSandN, imgPyramidN,imgStarN;	// 背景(夜)の画像

int seMetronome, seTempo, seClick;	// 使用する音声

int playerDrawImg;	// プレイヤーの現在の画像
int ground, sand, pyramid,sunMoon;	// 背景の現在の画像
int pushS;	// Sキーが押された、離された瞬間を獲得するための変数

int sandX = 0;
int pyramidX = 0;

int speed = 150;
int drawSpeed = 100;
int timer = 0;
int rhythm = 2;
int run = 500;
int count = 0;

bool sun = true;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("ランニングゲーム");	// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);	// ウィンドウモードで起動
	if (DxLib_Init() == -1)return -1;	// ライブラリ初期化　エラーが起きたら終了
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする

	InitGame();	// 初期化用の関数を呼び出す

	while(1)
	{
		ClearDrawScreen();
		// 背景を表示する処理
		if (sun == true) {
			sunMoon = imgSun;
			ground = imgGround;
			pyramid = imgPyramid;
			sand = imgSand;
			SetBackgroundColor(100, 200, 255);	// 背景色の指定
		}
		else
		{
			sunMoon = imgMoon;
			ground = imgGroundN;
			pyramid = imgPyramidN;
			sand = imgSandN;
			DrawImage(imgStarN, WIDTH/2, HEIGHT/2, 8);	// 星
			SetBackgroundColor(0, 10, 180);	// 背景色の指定
		}
		pyramidX = (pyramidX - speed / 15);	//ピラミッド
		DrawExtendGraph(800+pyramidX/100, 200, 1300 + pyramidX/100, 450, pyramid, true);
		DrawImage(ground, WIDTH / 2, (HEIGHT / 2)+250,8);	// 地面
		sandX = (sandX - speed/50) % WIDTH;	// 砂の部分
		DrawExtendGraph(sandX, 450,WIDTH+sandX,500,sand,true);
		DrawExtendGraph(WIDTH+sandX,450,WIDTH*2+sandX,500,sand,true);
		DrawImage(sunMoon, 200, 120, 5);	// 昼なら太陽、夜なら月を表示する
		timer++;
		drawSpeed = 200 - speed;	// 200からスピード値を引いた時間で画像が一周する
		if		(timer % drawSpeed > (drawSpeed / 6) * 5)playerDrawImg = img04Right;
		else if (timer % drawSpeed > (drawSpeed / 6) * 4)playerDrawImg = img07Right;
		else if (timer % drawSpeed > (drawSpeed / 6) * 3)playerDrawImg = img10Right;
		else if (timer % drawSpeed > (drawSpeed / 6) * 2)playerDrawImg = img04Left;
		else if (timer % drawSpeed > (drawSpeed / 6) * 1)playerDrawImg = img07Left;
		else if (timer % drawSpeed > 0)					 playerDrawImg = img10Left;
		if (timer % drawSpeed == 0)count++;	// プレイヤーの動き毎にカウント
		if (count == 8)	// 8周すると昼と夜を切り替える
		{
			if (rhythm == 2)rhythm = 3, speed = 150,sun=false;
			else rhythm = 2, speed=150,sun=true;
			count = 0;
		}
		
		if (timer % drawSpeed == ((drawSpeed/6)*5.5))PlaySoundMem(seTempo, DX_PLAYTYPE_BACK);	// 定期的に音を鳴らす
		if (rhythm % 2 == 0) {	// リズムが2で割り切れるとき
			if (timer % drawSpeed == ((drawSpeed / 6) * 2.5))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK);
		}
		if (rhythm % 3 == 0) {	// リズムが3で割り切れるとき
			if (timer % drawSpeed == ((drawSpeed / 6) * 1.5))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK);
			if (timer % drawSpeed == ((drawSpeed / 6) * 3.5))PlaySoundMem(seMetronome, DX_PLAYTYPE_BACK);
		}
		run--;
		DrawImage(playerDrawImg, WIDTH / 2 - (500 - run) / 10, HEIGHT / 2, 8);	// 元の画像が小さいので8倍で出力
		//DrawImage(imgRunningMachine, WIDTH / 2, (HEIGHT / 2) + 45, 8);

		if (timer % drawSpeed > (drawSpeed/6) * 5 && pushS == 1)run += drawSpeed*2;
		//if (timer % drawSpeed < (drawSpeed/10) * 1 && pushS == 1)run += drawSpeed * 2;

		if (timer%drawSpeed<(drawSpeed/6)*5&&timer % drawSpeed > 0 && pushS == 1)run -= drawSpeed * 2;


		if(CheckHitKey(KEY_INPUT_S) == 0)
		{
			if (pushS > 0) pushS = -1;
			else pushS = 0;
		}
		else
		{
			pushS++;
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
	// プレイヤーの画像読み込み
	img10Right = LoadGraphWithCheck("image/run1.0Right.png");
	img07Right = LoadGraphWithCheck("image/run0.7Right.png");
	img04Right = LoadGraphWithCheck("image/run0.4Right.png");
	img10Left = LoadGraphWithCheck("image/run1.0Left.png");
	img07Left = LoadGraphWithCheck("image/run0.7Left.png");
	img04Left = LoadGraphWithCheck("image/run0.4Left.png");
	imgStop = LoadGraphWithCheck("image/runStop.png");

	// ランニングマシンの画像読み込み
	imgRunningMachine = LoadGraphWithCheck("image/RunningMachineRed.png");

	// 背景の画像読み込み
	imgGround = LoadGraphWithCheck("image/runGround.png");
	imgSand = LoadGraphWithCheck("image/runSand.png");
	imgPyramid = LoadGraphWithCheck("image/runPyramid.png");
	imgSun = LoadGraphWithCheck("image/runSun.png");
	imgMoon = LoadGraphWithCheck("image/runMoon.png");
	imgGroundN = LoadGraphWithCheck("image/runGroundNight.png");
	imgSandN = LoadGraphWithCheck("image/runSandNight.png");
	imgPyramidN = LoadGraphWithCheck("image/runPyramidNight.png");
	imgStarN = LoadGraphWithCheck("image/runStarNight.png");

	// サウンドの読み込みと音量設定
	seMetronome = LoadSoundMemWithCheck("sound/AS_メトロノーム.mp3");
	seTempo = LoadSoundMemWithCheck("sound/AS_カッ.mp3");
	seClick = LoadSoundMemWithCheck("sound/AS_ピッ.mp3");
}

// 中心座標を指定して画像を表示する関数
void DrawImage(int img,int x,int y,int enlarge)
{
	int w, h;
	GetGraphSize(img, &w, &h);
	//DrawGraph(x - w / 2, y - h / 2, img, true);
	DrawExtendGraph(x - (w/2)*enlarge, y - (h/2)*enlarge, x + (w/2)*enlarge, y + (h/2)*enlarge, img, true);
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