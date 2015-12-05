#include "common.h"
#include "client_func.h"

static SDL_Surface *gPlayer, *gPTama0;

static void PlayerShotEnter(int n, int num);

void(*PlayerShotPattern[PLAYER_SHOT_PATTERN_MAX])(int) = {
    PlayerShotPattern0, PlayerShotPattern1
};


void PlayerLoad(){
    gPlayer = IMG_Load("sozai/player1.png");
    gPTama0 = IMG_Load("sozai/Ptama0.png");
}


void PlayerInit(int num){
    int i, j;
    for(i = 0; i < num; i++){
        player[i].flag = 0;
        player[i].knd = 0;
    }
    //HP_Max = 0;
    //HP_Num = HP_Max;

    for(i = 0; i < PLAYER_SHOT_MAX; i++){
        pla_shot[i].flag = 0;
        for(j = 0 ; j < SHOT_BULLET_MAX; j++){
            pla_shot[i].bullet[j].flag = 0;
        }
    }
}


void PlayerDraw(int pos){
    if(player[pos].flag > 0){
        switch(player[pos].knd){
        case 1:
            SDL_BlitSurface(gPlayer, &player[pos].src, window, &player[pos].dst);
            break;
        }
    }

    int i, j;
    SDL_Surface *Pl;
    Sint16 wid, hig;
    SDL_Rect pl_dst;

    for(i = 0 ; i < PLAYER_SHOT_MAX; i++){
        if(pla_shot[i].flag == 1){
            for(j = 0; j < SHOT_BULLET_MAX; j++){
                if(pla_shot[i].bullet[j].flag > 0){
                    switch(pla_shot[i].bullet[j].knd){
                    case 0:
                        Pl = rotozoomSurface(gPTama0, -pla_shot[i].bullet[j].rad, 1, 0);
                        wid = (Sint16)((Pl->w-10)/2+0.5);
                        hig = (Sint16)((Pl->h-10)/2+0.5);
                        pl_dst = DstRectInit(pla_shot[i].bullet[j].dst.x-wid, pla_shot[i].bullet[j].dst.y-hig);

                        SDL_BlitSurface(Pl, &pla_shot[i].bullet[j].src, window, &pl_dst);

                        SDL_Flip(window);
                        SDL_FreeSurface(Pl);
                    }
                }
            }
        }
    }

    SDL_Flip(window);
}


void PlayerDataLoad(){
    FILE *fp;//ファイルを読み込む型
    int input[64];
    char inputc[64];
    int i, j;

    if((fp = fopen("PlayerData.csv", "r")) == NULL){//ファイル読み込み
        SendEndCommand();
        return;
    }
    for (i = 0; i < 2; i++)//   2      
        while (getc(fp) != '\n');

    int n = 0;//行
    int num = 0;//列

    while (1) {
        for (i = 0; i < 64; i++) {
            input[i] = getc(fp);//1文字取得
            inputc[i] = input[i];
            if (inputc[i] == '/') {//スラッシュがあれば
                while (getc(fp) != '\n');
                i = -1;//カウンタを最初に戻す
                continue;
            }
            if (input[i] == ',' || input[i] == '\n') {//カンマか改行なら
                inputc[i] = '\0';

                break;
            }
            if (input[i] == EOF) {//ファイルの終わりなら
                goto EXFILE;
            }
        }

        switch (num) {
        case 0: playerOrder[n].knd = atoi(inputc); break; //プレイヤーの種類
        case 1: playerOrder[n].knd2 = atoi(inputc); break; //戦闘機か戦車か
        case 2: playerOrder[n].sp = atoi(inputc); break; //動く速度
        case 3: playerOrder[n].pattern2 = atoi(inputc); break; //攻撃パターン
        case 4: playerOrder[n].w = atoi(inputc); break; //（画像の）幅
        case 5: playerOrder[n].h = atoi(inputc); break; //（画像の）高さ
        case 6: playerOrder[n].hp_max = atoi(inputc); break; //体力
        case 7: playerOrder[n].power = atoi(inputc); break; //初期攻撃力
        case 8: playerOrder[n].w2 = atoi(inputc); break; //（砲台画像の）幅
        case 9: playerOrder[n].h2 = atoi(inputc); break; //（砲台画像の）高さ
        }
        num++;
        if (num == 10) {
            num = 0;
            n++;
        }
    }
EXFILE:
    fclose(fp);
}


void PlayerEnter(int num){
    int i, t;
    for(i = 0; i < num; i++){
        player[i].knd = 1;
    }
    for(i = num; i < MAX_CLIENTS; i++){
        player[i].knd = 0;
    }

    for(i = 0; i < num; i++){
        if(player[i].flag == 0){
            for(t = 0; t < PLAYER_ORDER_MAX; t++){
                if(player[i].knd == playerOrder[t].knd){
                    player[i].flag = 1;
                    player[i].knd2 = playerOrder[t].knd2;
                    player[i].sp = playerOrder[t].sp;
                    player[i].power = playerOrder[t].power;
                    player[i].pattern2 = playerOrder[t].pattern2;

                    //HP_max += playerOrder[t].hp_max;

                    player[i].flag2 = 0;
                    player[i].num = i;

                    switch (player[i].knd2) {
                    case 1:
                        player[i].tx = 100;
                        player[i].ty = 100*(player[i].num+1);
                        player[i].ang = 0;
                        break;
                    case 2:
                        //player[i].w2 = playerOrder[t].w2;
                        //player[i].h2 = playerOrder[t].h2;
                        player[i].tx = 50*(player[i].num+1);
                        player[i].ty = 520+10*(player[i].num+1);
                        player[i].ang = -PI / 6;
                        break;
                    }
                    player[i].src = SrcRectInit(0, 0, playerOrder[t].w, playerOrder[t].h);
                    player[i].dst = DstRectInit(player[i].tx - player[i].src.w / 2, player[i].ty - player[i].src.h / 2);

                    PlayerShotEnter(i, num);//後に"i"に変える

                    break;
                }
            }
        }

    }

}

static void PlayerShotEnter(int n, int num){
    int i;

    for(i = 0; i < num; i++){ //1 <= num <= PLAYER_SHOT_MAX
        if(pla_shot[i].flag == 0){
            pla_shot[i].flag = 1;
            pla_shot[i].knd = player[n].pattern2;
            pla_shot[i].cnt = 0;
            pla_shot[i].num = n;
            return;
        }
    }
}

void PlayerBulletEnter(int pos){
    int i;

    for(i = 0; i < PLAYER_SHOT_MAX; i++){
        if(player[pos].flag > 0 && pla_shot[i].cnt == 0){
            if(pla_shot[i].flag > 0 && 0 <= pla_shot[i].knd && pla_shot[i].knd < PLAYER_SHOT_PATTERN_MAX){
                fprintf(stderr, "in\n");
                PlayerShotPattern[pla_shot[i].knd](i);
            }
        }

        else if(pla_shot[i].cnt > 0)
            pla_shot[pos].cnt--;
    }
}


void PlayerShotCalc(){
    int i, j;

    for(i = 0; i < PLAYER_SHOT_MAX; i++){
        if(pla_shot[i].flag > 0){
            for(j = 0; j < SHOT_BULLET_MAX; j++){
                if(pla_shot[i].bullet[j].flag > 0){
                    pla_shot[i].bullet[j].tx += pla_shot[i].bullet[j].spd * cos(pla_shot[i].bullet[j].ang);
                    pla_shot[i].bullet[j].ty += pla_shot[i].bullet[j].spd * sin(pla_shot[i].bullet[j].ang);
                    DstRectInit(pla_shot[i].bullet[j].tx - pla_shot[i].bullet[j].src.w / 2, pla_shot[i].bullet[j].ty - pla_shot[i].bullet[j].src.h / 2);
                    pla_shot[i].bullet[j].cnt++;

/*敵と弾の当たり判定*/


/*画面外に出たら*/
                    if (pla_shot[i].bullet[j].tx < 0 || WINDOW_WIDTH < pla_shot[i].bullet[j].tx ||
                        pla_shot[i].bullet[j].ty < 0 || WINDOW_HEIGHT < pla_shot[i].bullet[j].ty)
                        pla_shot[i].bullet[j].flag = 0;
                }
            }
        }
    }
}


void PlayerFree(){
    SDL_FreeSurface(gPlayer);
}
