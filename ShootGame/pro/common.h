/*****************************************************************
ファイル名	: common.h
機能		: サーバーとクライアントで使用する定数の宣言を行う
*****************************************************************/

/*
バグ
・client_EnemyShotPattern.c  自機狙いができない 解決
・client_EnemyData.c  たまに敵が描画されない 解決
・server.c　スコアの計算をする
・クライアントで管理しているもの（敵の状態など）を共通させる[描画の違いをなくすため]
・各クライアントでスコア等の動作が違う（ラグかな？）
・PlayerData.csv　本番用に変える　解決
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<assert.h>
#include<math.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

//#include <opencv/cv.h>
//#include <opencv/highgui.h>
/*#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
*/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_mixer.h>

#define PORT			(u_short)8888	/* ポート番号 */

#define MAX_CLIENTS		4				/* クライアント数の最大値 */
#define MAX_NAME_SIZE	10 				/* ユーザー名の最大値*/

#define MAX_DATA		200				/* 送受信するデータの最大値 */

#define END_COMMAND		'Q'		  		/* プログラム終了コマンド */

#define FOUR_COMMAND     'H'
#define THREE_COMMAND    'I'

#define LEFT_COMMAND	'L'				/* 左コマンド */
#define RIGHT_COMMAND	'R'				/* 右コマンド */
#define UP_COMMAND	'U'				/* 上コマンド */
#define DOWN_COMMAND	'D'				/* 下コマンド */
#define SEPARATE_UPDO_COMMAND	'Y'				/* 左スティックを離すコマンド */
#define SEPARATE_LERI_COMMAND	'X'				/* 左スティックを離すコマンド */
#define SHOT_COMMAND 'S'                    /* 5ボタンを押した時 */
#define SHOT_FINISH_COMMAND 'F'            /* 5ボタンを離した時 */
#define DATA_PULL 'C'

//0:上 1:右 3:左 (右スティック)
#define LEFT_ROTA 'B'
#define LEFT_SEPA_ROTA 'G'
#define RIGHT_ROTA 'T'
#define RIGHT_SEPA_ROTA 'V'
#define UP_ROTA 'O'
#define UP_SEPA_ROTA 'N'

#define PLAYER_HIT 'P'
#define PLAYER_HIT2 'A'
#define ENEMY_HIT 'E'


#define PLAYER_ORDER_MAX 10
#define PLAYER_SHOT_MAX 4
#define PLAYER_SHOT_PATTERN_MAX 2
#define ENEMY_MAX 5
#define ENEMY_ORDER_MAX 21
#define ENEMY_PATTERN_MAX 2
#define ENEMY_SHOT_MAX 5
#define ENEMY_SHOT_PATTERN_MAX 2
#define SHOT_BULLET_MAX 100
#define PI 3.1415926535


SDL_Surface *window;
SDL_Joystick *joystick;
TTF_Font* font;

typedef enum{
    GAME_TITLE,
    GAME_SELECT,
    GAME_LOAD,
    GAME_MAIN,
    GAME_OVER,
    GAME_CLEAR
}GAME_STATE;

typedef struct{
    Uint32 now; //現在時間
    Uint32 wit; //待ち時間
    Uint32 lev; //経過時間
}Timer;

typedef struct{
    int up, down, left, right;
    int b5;
    int rotaU, rotaL, rotaR;

    int kndP; //機種選択
    int kPflag; //決定したか
}Command;

typedef struct{
    SDL_Rect src, dst; //本体用
    SDL_Rect src2, dst2; //砲台用
    int tx, ty;
    Command command;

    int flag, flag2; //生成されている状態か //ダメージを受けているか
    int knd, knd2, pattern2; //どの機種か //戦闘機か戦車か //初期の攻撃方法
    int power; //攻撃力
    int sp; //速度
    double ang, rad; //砲台の角度
    int num; //○Pか
}PlayerData;

typedef struct {
    SDL_Rect src, dst; //画像
    int tx, ty;
    int flag; //生成されているか
    int flag2; //ダメージを受けているか
    int hp, hp_max; //体力
    int wait;
    int knd, pattern, sp, cnt; //種類、移動
    int pattern2, blknd, blW, blH, blCnt, bltime; //攻撃
    int item, item2;
    int score;
}EnemyData;

typedef struct {
    int tx, ty, w, h; //画像の位置、大きさ
    int hp_max, flag;
    int mission, stage, wait; //ステージ
    int knd, pattern, sp; //種類、移動
    int pattern2, blknd, blW, blH, blCnt, bltime; //攻撃
    int item, item2;
    int score;
}EnemyOrder;

typedef struct {
    SDL_Rect src, dst;
    double tx, ty;
    int ghandle;
    int flag, knd, cnt, state;
    double ang, rad, spd;
}Bullet;

typedef struct {
	int flag, cnt, knd, num; //フラグ、カウント、種類、敵の番号
	Bullet bullet[SHOT_BULLET_MAX];
}Shot;

typedef struct{
    Sint16 x, y, r;
    Command command;
}En;

En en[MAX_CLIENTS];

Timer timer;
Command pla_sele[MAX_CLIENTS]; //選択画面で使用(サーバー側・クライアント側両方で使用)
PlayerData player[MAX_CLIENTS];
Shot pla_shot[PLAYER_SHOT_MAX];
EnemyData enemy[ENEMY_MAX];
EnemyOrder enemyOrder[ENEMY_ORDER_MAX];
Shot ene_shot[ENEMY_SHOT_MAX];

int stage;
int stageFlag;

typedef struct {
    char command;
    int cid; //クライアントのID
    GAME_STATE state;
    int tx, ty; //プレイヤーの中心座標を送るときに使用
    int m, n; //敵の打った弾の番号を送るときに使用(ene_shot[m].bullet[n]),(pla_shot[m].bullet[n])
    int ene_num; //敵に弾が当たった時の敵の番号
    PlayerData player;
    EnemyData enemy;
    int hp;
    int flag; //1:最大HPの送信  2:  3:機種決定した時  4:全員が機種決定した時

    int kndP; //選択時の各クライアントの位置
    int kPflag;
} CONTAINER;

#endif
