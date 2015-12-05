/*****************************************************************
ファイル名	: common.h
機能		: サーバーとクライアントで使用する定数の宣言を行う
*****************************************************************/

/*
バグ
・client_win.c : DrawLeftCircle関数 : 円が左に行かない
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<assert.h>
#include<math.h>

//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
//#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_mixer.h>

#define PORT			(u_short)8888	/* ポート番号 */

#define MAX_CLIENTS		4				/* クライアント数の最大値 */
#define MAX_NAME_SIZE	10 				/* ユーザー名の最大値*/

#define MAX_DATA		200				/* 送受信するデータの最大値 */

#define END_COMMAND		'E'		  		/* プログラム終了コマンド */
#define LEFT_COMMAND	'L'				/* 左コマンド */
#define RIGHT_COMMAND	'R'				/* 右コマンド */
#define UP_COMMAND	'U'				/* 上コマンド */
#define DOWN_COMMAND	'D'				/* 下コマンド */
#define SEPARATE_UPDO_COMMAND	'S'				/* 左スティックを離すコマンド */
#define SEPARATE_LERI_COMMAND	'T'				/* 左スティックを離すコマンド */

typedef struct{
    CvPoint center;
    int x, y, r;
    int command;
}PlayerData;

PlayerData player[2];

#endif
