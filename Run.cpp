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

int imgPharaoh, imgPharaohMiss;	// ファラオ(追いかけてくる敵)の画像

int imgGround, imgSand, imgPyramid,imgSun,imgMoon;	// 背景の画像
int imgGroundN, imgSandN, imgPyramidN,imgStarN;	// 背景(夜)の画像

int seMetronome, seTempo, seClick;	// 使用する音声

// 画像代入用
int playerDrawImg;	// プレイヤーの現在の画像
int ground, sand, pyramid,sunMoon;	// 背景の現在の画像
int pharaoh = imgPharaoh;	// ファラオの現在の画像

int pushS;	// Sキーが押された、離された瞬間を獲得するための変数

int sandX = 0;	// 背景を動かす際のX座標
int pyramidX = 0;

//太陽または月の位置、大きさを代入
int celestialX = 0;	// X座標
int celestialY = 0;	// Y座標
int celestialSiz = 0;	// 大きさ

int speed = 150;	// プレイヤーや背景の動くスピード
int drawSpeed = 50;	// プレイヤーのアニメーションがこの範囲で描かれる　200からspeedを引いた値
int timer = 0;
int rhythm = 2;	// drawSpeedごとに音が鳴る頻度
int run = 1500;	// プレイヤーの位置を決める値
int count = 0;	// プレイヤーのアニメーションが何周したかをカウントし時間帯を切り替える
int distance = 0;	// クリアまでの距離を表す
int timeZone = EVENING;	// 時間帯

//bool sun = true;	// 時間帯に合わせて太陽を月に切り替えるbool
bool dush = true;	//  前回ミスしていないかどうかを判定する
bool pharaohUp = true;	// ファラオが上に動くか下に動くかを切り替える

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetWindowText("逃走ゲーム");	// ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	// ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true);	// ウィンドウモードで起動
	if (DxLib_Init() == -1)return -1;	// ライブラリ初期化　エラーが起きたら終了
	SetDrawScreen(DX_SCREEN_BACK);	// 描画面を裏画面にする

	InitGame();	// 初期化用の関数を呼び出す

	while(1)
	{
		ClearDrawScreen();

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
			sunMoon = imgSun;
			ground = imgGround;
			pyramid = imgPyramid;
			sand = imgSand;

			celestialX = 1000;
			celestialY = 200;
			celestialSiz = 6;
			SetBackgroundColor(255, 165, 0);	// 背景色の指定
		}
		
		if(timeZone==NIGHT)
		{
			sunMoon = imgMoon;
			ground = imgGroundN;
			pyramid = imgPyramidN;
			sand = imgSandN;

			celestialX = 200;
			celestialY = 120;
			celestialSiz = 3;
			DrawImage(imgStarN, WIDTH/2, HEIGHT/2, 8);	// 星
			SetBackgroundColor(0, 10, 180);	// 背景色の指定
		}
		DrawImage(sunMoon, celestialX, celestialY, celestialSiz);	// 昼なら太陽、夜なら月を表示する
		if(timeZone==NIGHT)SetDrawBright(150, 150, 150);

		pyramidX = (pyramidX - speed / 15);	//ピラミッド
		DrawExtendGraph(800+pyramidX/100, 200, 1300 + pyramidX/100, 450, pyramid, true);

		DrawImage(ground, WIDTH / 2, (HEIGHT / 2)+250,8);	// 地面

		sandX = (sandX - speed/50) % WIDTH;	// 砂の部分
		DrawExtendGraph(sandX, 450,WIDTH+sandX,500,sand,true);
		DrawExtendGraph(WIDTH+sandX,450,WIDTH*2+sandX,500,sand,true);


		timer++;
		drawSpeed = 200 - speed;	// 200からスピード値を引いた時間で画像が一周する
		if		(timer % drawSpeed > (drawSpeed / 6) * 5)playerDrawImg = imgR04Right;
		else if (timer % drawSpeed > (drawSpeed / 6) * 4)playerDrawImg = imgR07Right;
		else if (timer % drawSpeed > (drawSpeed / 6) * 3)playerDrawImg = imgR10Right;
		else if (timer % drawSpeed > (drawSpeed / 6) * 2)playerDrawImg = img04Left;
		else if (timer % drawSpeed > (drawSpeed / 6) * 1)playerDrawImg = img07Left;
		else if (timer % drawSpeed > 0)					 playerDrawImg = img10Left;
		if (timer % drawSpeed == 0)
		{
			distance -= GetRand(5);
			count++;	// プレイヤーの動き一周毎にカウント
			if (pharaohUp == true)pharaohUp = false;	// ファラオの上下を切り替え
			else if(pharaohUp==false)pharaohUp = true;
		}


		if (dush == false && timer % drawSpeed < (drawSpeed / 6) * 2)playerDrawImg = imgMissLeft;

		if (count == 16)	// 【仮】16周すると昼と夜を切り替える
		{
			if (rhythm == 2)rhythm = 3, speed = 150,timeZone=NIGHT;
			else rhythm = 2, speed=150,timeZone=NOON;
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
		

		if (timeZone==NIGHT)SetDrawBright(100, 100, 100);
		DrawImage(playerDrawImg, WIDTH / 2 - (500 - run) / 10, HEIGHT / 2, 8);	// 元の画像が小さいので8倍で出力

		// ファラオ　プレイヤーのアニメーション一周ごとに切り替わるboolで上下させる
		if (pharaohUp == true)	DrawImage(pharaoh, 100, HEIGHT / 2 - (timer % drawSpeed) / 2, 8);
		else					DrawImage(pharaoh, 100, (HEIGHT / 2-drawSpeed/2)+(timer % drawSpeed)/2, 8);	
		SetDrawBright(255, 255, 255);
	

		// 正しいタイミングでSキーが押された瞬間に一回だけ進める
		if (timer % drawSpeed > (drawSpeed/6) * 5 && pushS == 1&&dush==false)run += drawSpeed*2,dush=true,pharaoh = imgPharaoh;

		// Sキーが押されていなかった場合に少し後退させる
		if (timer % drawSpeed == 0 && dush == false)pharaoh = imgPharaohMiss;

		// 少し経った後に失敗モーションを終わらせる
		if (timer % drawSpeed < (drawSpeed / 6) * 5&&timer % drawSpeed > (drawSpeed / 6) * 2 && dush == true)dush = false;
	
		// 正しくないタイミングでSキーが押された瞬間に少し後退させる
		if (timer%drawSpeed<(drawSpeed/6)*5&&timer % drawSpeed > 0 && pushS == 1)run -= drawSpeed * 2, pharaoh = imgPharaohMiss;

		if(pharaoh==imgPharaohMiss)run -= 5;	// プレイヤーがミスを行った場合成功するまで後ろに引っ張る
		else if (run > 1000)run -= 2;	// プレイヤーが一定より前にいる場合後ろに引っ張り調整する
		else if(run>-1000) run -= 1;

		if(distance>0)distance --;	// 残り距離を縮める
		if(distance>500)DrawText(WIDTH/2-150, HEIGHT-200, "Finish:%dm", distance, 0xff1010, 50);	// ゴールまでを表すテキスト　ゴールに近づけば非表示になる
		if (distance < 0)distance = 0;

		// Sキーが押された瞬間かどうかを判定する
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

	// ファラオ（追いかけてくる敵）の画像読み込み
	imgPharaoh = LoadGraphWithCheck("image/runPharaohNormal.png");
	imgPharaohMiss = LoadGraphWithCheck("image/runPharaohMiss.png");

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