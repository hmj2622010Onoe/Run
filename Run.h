#pragma once

// 関数プロトタイプ宣言
void InitGame(void);
void DrawImage(int img, int x, int y,int enlarge);
//void ScrollBG(int spd);
void DrawText(int x, int y, const char* txt, int vai, int col, int siz);

int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);