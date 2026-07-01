#include"DxLib.h"
#include"Run.h"	// ヘッダーファイルをインクルード

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720;	// ウィンドウの幅と高さのピクセル数
const int FPS = 60;

// グローバル変数
int img10Right,img07Right,img04Right, img10Left,img07Left,img04Left,imgStop;	// プレイヤーの画像
int imgRunningMachine;	// ランニングマシンの画像

int playerDrawImg;	// プレイヤーの現在の画像
int pushS;	// Sキーが押された、離された瞬間を獲得するための変数

int speed = 150;
int drawSpeed = 100;
int timer = 0;
int run = 500;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("ランニングゲーム");	// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);	// ウィンドウモードで起動
	if (DxLib_Init() == -1)return -1;	// ライブラリ初期化　エラーが起きたら終了
	SetBackgroundColor(100, 200, 100);	// 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする

	InitGame();	// 初期化用の関数を呼び出す

	while(1)
	{
		ClearDrawScreen();
		timer++;
		//if (timer % 300 == 1)speed = 50+GetRand(125);
		drawSpeed = 200 - speed;	// 200からスピード値を引いた時間で画像が一周する
		if		(timer % drawSpeed > (drawSpeed / 10) * 9)playerDrawImg = img10Right;
		else if (timer % drawSpeed > (drawSpeed / 10) * 8)playerDrawImg = img07Right;
		else if (timer % drawSpeed > (drawSpeed / 10) * 7)playerDrawImg = img04Right;
		else if (timer % drawSpeed > (drawSpeed / 10) * 6)playerDrawImg = img04Left;
		else if (timer % drawSpeed > (drawSpeed / 10) * 5)playerDrawImg = img07Left;
		else if (timer % drawSpeed > (drawSpeed / 10) * 4)playerDrawImg = img10Left;
		else if (timer % drawSpeed > (drawSpeed / 10) * 3)playerDrawImg = img07Left;
		else if (timer % drawSpeed > (drawSpeed / 10) * 2)playerDrawImg = img04Left;
		else if (timer % drawSpeed > (drawSpeed / 10) * 1)playerDrawImg = img04Right;
		else if (timer % drawSpeed > 0)playerDrawImg = img07Right;
		run-=speed/50;
		DrawImage(playerDrawImg, WIDTH / 2-(500-run)/10, HEIGHT / 2,8);	// 元の画像が小さいので8倍で出力
		DrawImage(imgRunningMachine, WIDTH / 2, (HEIGHT / 2)+35,8);

		if (timer % drawSpeed > (drawSpeed/9) * 8 && pushS == 1)run += speed/2;
		//if (timer % drawSpeed < (drawSpeed/10) * 1 && pushS == 1)run += drawSpeed * 2;

		if (timer%drawSpeed<(drawSpeed/10)*8&&timer % drawSpeed > (drawSpeed/10) * 1 && pushS == 1)run -= drawSpeed * 3;

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
	img10Right = LoadGraphWithCheck("image/Run1rightPlayer.png");
	img07Right = LoadGraphWithCheck("image/Run0.7rightPlayer.png");
	img04Right = LoadGraphWithCheck("image/Run0.4rightPlayer.png");
	img10Left = LoadGraphWithCheck("image/Run1leftPlayer.png");
	img07Left = LoadGraphWithCheck("image/Run0.7leftPlayer.png");
	img04Left = LoadGraphWithCheck("image/Run0.4leftPlayer.png");
	imgStop = LoadGraphWithCheck("image/Run0StopPlayer.png");

	// ランニングマシンの画像読み込み
	imgRunningMachine = LoadGraphWithCheck("image/RunningMachineRed.png");
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