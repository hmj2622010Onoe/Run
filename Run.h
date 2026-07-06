#pragma once

// 関数プロトタイプ宣言
void InitGame(void);
void DrawImage(int img, int x, int y,int enlarge);
void ScrollBG(int spd);

int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);