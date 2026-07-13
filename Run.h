#pragma once

// 関数プロトタイプ宣言
void InitGame(void);
void InitVariable(void);
void DrawImageEnlarge(int img, int x, int y,int enlargeX,int enlargeY);
void BackGround(void);
void DrawText(int x, int y, const char* txt, int vai, int col, int siz);

int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);