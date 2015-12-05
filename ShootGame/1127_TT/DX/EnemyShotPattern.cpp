#include "common.h"
#include <math.h>
#include <time.h>


//自分に発射
static double shotatan2(int n) {
	return atan2(player[0].ty - enemy[ene_shot[n].num].ty, player[0].tx - enemy[ene_shot[n].num].tx);
}

//敵に一番近いプレイヤーに発射
static double shotatan2_2(int n) {
	int x1 = player[0].tx - enemy[ene_shot[n].num].tx;
	int y1 = player[0].ty - enemy[ene_shot[n].num].ty;
	int r1 = sqrt(x1*x1 + y1*y1);

	for (int i = 1; i < PLAYER_MAX; i++) {
		int x2 = player[i].tx - enemy[ene_shot[n].num].tx;
		int y2 = player[i].ty - enemy[ene_shot[n].num].ty;
		int r2 = sqrt(x2*x2 + y2*y2);

		if (r2 < r1) {
			x1 = x2;
			y1 = y2;
			r1 = r2;
		}
	}

	return atan2(y1, x1);
}

//ランダムにプレイヤー狙い発射
static double shotatan2_3(int n) {
	int i = rand() % PLAYER_MAX;

	return atan2(player[i].ty - enemy[ene_shot[n].num].ty, player[i].tx - enemy[ene_shot[n].num].tx);
}

static int shot_search(int n) {
	for (int j = 0; j < SHOT_BULLET_MAX; j++) {
		if (ene_shot[n].bullet[j].flag == 0) {
			return j;
		}
	}
	return -1;
}

void EnemyShotPattern0(int n) {
	int k;
	if (ene_shot[n].cnt % enemy[ene_shot[n].num].blCnt == 1) { //弾幕が始まってから0カウント目
		//敵が倒されていなくて、探した登録可能な弾番号が有効なら
		if (ene_shot[n].flag != 2 && (k = shot_search(n)) != -1) {
			ene_shot[n].bullet[k].knd = enemy[ene_shot[n].num].blknd;
			ene_shot[n].bullet[k].ang = shotatan2_2(n);
			ene_shot[n].bullet[k].flag = 1;
			ene_shot[n].bullet[k].tx = enemy[ene_shot[n].num].tx;
			ene_shot[n].bullet[k].ty = enemy[ene_shot[n].num].ty;
			ene_shot[n].bullet[k].w = enemy[ene_shot[n].num].blW;
			ene_shot[n].bullet[k].h = enemy[ene_shot[n].num].blH;
			ene_shot[n].bullet[k].cnt = 0;
			ene_shot[n].bullet[k].spd = 3;
		}
	}
}

void EnemyShotPattern1(int n) {
	int k;
	if (ene_shot[n].cnt % enemy[ene_shot[n].num].blCnt == 1) { //弾幕が始まってから0カウント目
								  //敵が倒されていなくて、探した登録可能な弾番号が有効なら
		if (ene_shot[n].flag != 2 && (k = shot_search(n)) != -1) {
			ene_shot[n].bullet[k].knd = enemy[ene_shot[n].num].blknd;
			ene_shot[n].bullet[k].ang = -PI;
			ene_shot[n].bullet[k].flag = 1;
			ene_shot[n].bullet[k].tx = enemy[ene_shot[n].num].tx;
			ene_shot[n].bullet[k].ty = enemy[ene_shot[n].num].ty;
			ene_shot[n].bullet[k].w = enemy[ene_shot[n].num].blW;
			ene_shot[n].bullet[k].h = enemy[ene_shot[n].num].blH;
			ene_shot[n].bullet[k].cnt = 0;
			ene_shot[n].bullet[k].spd = 3;
		}
	}
}
