#include <netdb.h>

//#include "../constants.h"
#include "../common.h"
#include "client_func.h"

static GAME_STATE gstate = GAME_TITLE;

static int num_clients;
static int myid;
static int sock;
static int num_sock;
static fd_set mask;
static CLIENT clients[MAX_NUM_CLIENTS];

static int DrawGameTitle();
static int DrawGameSelect();
static int DrawGameLoad();
static int DrawGameMain();
static int DrawGameOver();
static int DrawGameClear();
static void PlayerAllInit(int knd); //初期データの送信・受信
//static void input_main_command(void); //送信（コマンド入力）
static int execute_command(void); //受信（行動実行）
//static void send_data(void *, int); //データを送る
static int receive_data(void *, int); //データを受け取る


static SDL_Surface *haikei1, *haikei2, *haikei3;
static SDL_Rect src_rect, dst_rect, src_rect2, dst_rect2;


/******************************
void setup_client(char *server_name, u_short port)
引数：*server_name サーバー名(ホスト名)
      port クライアントが使用するポート番号
機能：クライアントの起動
********************************/
void setup_client(char *server_name, u_short port) {
  struct hostent *server;
  struct sockaddr_in sv_addr;

  fprintf(stderr, "Trying to connect server %s (port = %d).\n", server_name, port);
  if ((server = gethostbyname(server_name)) == NULL) { //ホスト名からIPアドレスを取得出来なかったら
    handle_error("gethostbyname()");
  }

  sock = socket(AF_INET, SOCK_STREAM, 0); //ソケットの識別番号を返す
  if (sock < 0) {
    handle_error("socket()");
  }

//サーバーのアドレス
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(port); //IPネットワークバイトオーダーの（16bit）形式で変換
  sv_addr.sin_addr.s_addr = *(u_int *)server->h_addr_list[0];

//connect:クライアントからサーバー側へコネクションの接続確立要求を行う
//引数１：ソケットディスクリプタ　引数２：sockaddr構造体へのポインタ　引数３：サイズ
  if(connect(sock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) { 
    handle_error("connect()");
  }

  fprintf(stderr, "Input your name: ");
  char user_name[MAX_LEN_NAME];
  if(fgets(user_name, sizeof(user_name), stdin) == NULL) { //fgets:引数３ファイルから１行、または引数２バイトを引数１に書きこむ
    handle_error("fgets()");
  }
  user_name[strlen(user_name) - 1] = '\0';
  send_data(user_name, MAX_LEN_NAME, sock); //引数１をソケットに入れる

  fprintf(stderr, "Waiting for other clients...\n");
  receive_data(&num_clients, sizeof(int)); //クライアント数を読み込む
  fprintf(stderr, "Number of clients = %d.\n", num_clients);
  receive_data(&myid, sizeof(int)); //自分のIDを読み込む
  fprintf(stderr, "Your ID = %d.\n", myid);
  int i;
  for(i = 0; i < num_clients; i++) {
      receive_data(&clients[i], sizeof(CLIENT)); //クライアントのデータを読み込む
      fprintf(stderr, "Client %d is accepted (name=%s).\n", i, clients[i].name);
  }

  num_sock = sock + 1;
  FD_ZERO(&mask); //引数をゼロクリア
  FD_SET(0, &mask); //0番目のFDに対応する値を1にセット
  FD_SET(sock, &mask); //
  //fprintf(stderr, "Input command (M=message, S=select, Q=quit): \n");

/*SDLの初期画面設定*/
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER) < 0) {
        printf("failed to initialize SDL.\n");
        return;
    }
    SDL_EnableKeyRepeat(0, 0);

    TTF_Init();
    font = TTF_OpenFont("sozai/kochi-gothic-subst.ttf", 36);

    if((window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, /*SDL_HWSURFACE | SDL_FULLSCREEN*/SDL_SWSURFACE)) == NULL) {
        printf("failed to initialize videomode.\n");
        exit(-1);
    }

    //SDL_WM_SetCaption("SDL Game", "Software Exp");	// ウィンドウ名などを指定

    for(i = 0; i < SDL_NumJoysticks(); i++){
        printf("%s\n", SDL_JoystickName(i));
    }
    joystick = SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);
    if(!joystick){
        fprintf(stderr, "failed to open joystick.\n");
        return;
    }

/*ロード*/
    //haikei1 = IMG_Load("sozai/haikei1.png");
    haikei1 = IMG_Load("sozai/main_resource/05_main_game/background/bg_1.png");
    haikei2 = IMG_Load("sozai/main_resource/05_main_game/background/bg_2.png");
    haikei3 = IMG_Load("sozai/main_resource/05_main_game/background/bg_3.png");
    PlayerLoad();
    EnemyLoad();
    PlSeLoad();

/*初期化*/
    stageFlag = 1;
    //PlayerAllInit(3); //一斉スタートするとき
}

/*************************************
int control_requests ()
引数：なし
機能：クライアント中の動作(main画面)
返値：1…メッセージの送信　0…チャット終了
***************************************/
int control_requests () {
  int result = 1;
  switch(gstate){
  case GAME_TITLE:
      result = DrawGameTitle();
      break;
  case GAME_SELECT:
      result = DrawGameSelect();
      break;
  case GAME_LOAD:
      result = DrawGameLoad();
      break;
  case GAME_MAIN:
      result = DrawGameMain();
      break;
  case GAME_OVER:
      result = DrawGameOver();
      break;
  case GAME_CLEAR:
      result = DrawGameClear();
      break;
  }
  return result;
}


/************************************
static int DrawGameTitle()
引数：
機能：
返値：result
*************************************/
static int DrawGameTitle(){

    fd_set read_flag = mask;

    struct timeval timeout;
    timeout.tv_sec = 0; //クライアント側
    timeout.tv_usec = 30; //ユーザー側

//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1) {
        handle_error("select()");
    }

    int result = 1;
    if(stageFlag == 1){
        EventTitle(myid, sock);
    }
    if (FD_ISSET(sock, &read_flag)) {
        result = execute_command(); //受信
    }

    SDL_FillRect(window, NULL, SDL_MapRGBA(window->format, 255, 255, 255, 255));

    return result;
}

/************************************
static int DrawGameSelect()
引数：
機能：
返値：result
*************************************/
static int DrawGameSelect(){

    fd_set read_flag = mask;

    struct timeval timeout;
    timeout.tv_sec = 0; //クライアント側
    timeout.tv_usec = 30; //ユーザー側

//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1) {
        handle_error("select()");
    }

    int result = 1;
    if(stageFlag > 0){
        EventSelect(myid, sock);
    }
    if (FD_ISSET(sock, &read_flag)) {
        result = execute_command(); //受信
    }

    SDL_FillRect(window, NULL, SDL_MapRGBA(window->format, 255, 255, 255, 255));

    PlayerSelect(myid, num_clients);

    return result;
}

/************************************
static int DrawGameLoad()
引数：
機能：
返値：result
*************************************/
static int DrawGameLoad(){

    fd_set read_flag = mask;

    struct timeval timeout;
    timeout.tv_sec = 0; //クライアント側
    timeout.tv_usec = 30; //ユーザー側

//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1) {
        handle_error("select()");
    }

    int result = 1;
    if(stageFlag == 1){
        EventLoad(myid, sock);
    }
    if (FD_ISSET(sock, &read_flag)) {
        result = execute_command(); //受信
    }

    SDL_FillRect(window, NULL, SDL_MapRGBA(window->format, 255, 255, 255, 255));

    return result;
}

/************************************
static void DrawGameMain()
引数：
機能：
返値：result
*************************************/
static int DrawGameMain(){
    fd_set read_flag = mask;

    struct timeval timeout;
    timeout.tv_sec = 0; //クライアント側
    timeout.tv_usec = 30; //ユーザー側

//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1) {
        handle_error("select()");
    }

    int result = 1;
    if(stageFlag == 1){
        //input_main_command(); //送信
        switch(player[myid].knd2){
        case 1:
            EventMainFighter(myid, sock);
            break;
        case 2:
            EventMainTank(myid, sock);
            break;
        }
      
        if (FD_ISSET(sock, &read_flag)) {
            result = execute_command(); //受信
        }
    }

    int i;
    if(HP == 0){
        for(i = 0; i < num_clients; i++){
            if(player[i].flag == 0){ //読み込み中
                result = 1;
                return result;
            }
        }
    }
  

    SDL_FillRect(window, NULL, SDL_MapRGBA(window->format, 0, 0, 0, 255));
    if(stageFlag != 1){
        switch(stageFlag){
        case 2 : //ステージ２に移動
            src_rect.x += 10;
            if(src_rect.x + src_rect.w > WINDOW_WIDTH+200)
                src_rect.w = WINDOW_WIDTH + 200 - src_rect.x;
            if(src_rect.w < 0){
                src_rect.w = 0;
                src_rect2.x+=10;
            }

            src_rect2.w += 10;
            /*if(src_rect2.w > WINDOW_WIDTH){
              src_rect2.w = WINDOW_WIDTH;
              }*/
            dst_rect2 = DstRectInit(src_rect.w, 0);

            SDL_BlitSurface(haikei1, &src_rect, window, &dst_rect);
            SDL_BlitSurface(haikei2, &src_rect2, window, &dst_rect2);

            if(src_rect.w <= 0/*src_rect2.x >= 100*/){
                stageFlag = 1;
                src_rect = SrcRectInit(/*10*/0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                dst_rect = DstRectInit(0, 0);
                src_rect2 = SrcRectInit(0, 0, 0, WINDOW_HEIGHT);
                dst_rect2 = DstRectInit(0, 0);
            }
            break;
        case 3:
            src_rect.x += 10;
            if(src_rect.x + src_rect.w > WINDOW_WIDTH+200)
                src_rect.w = WINDOW_WIDTH + 200 - src_rect.x;
            if(src_rect.w < 0){
                src_rect.w = 0;
                src_rect2.x+=10;
            }

            src_rect2.w += 10;
            /*if(src_rect2.w > WINDOW_WIDTH){
              src_rect2.w = WINDOW_WIDTH;
              }*/
            dst_rect2 = DstRectInit(src_rect.w, 0);

            SDL_BlitSurface(haikei2, &src_rect, window, &dst_rect);
            SDL_BlitSurface(haikei3, &src_rect2, window, &dst_rect2);

            if(src_rect.w <= 0/*src_rect2.x >= 100*/){
                stageFlag = 1;
                src_rect = SrcRectInit(/*10*/0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
                dst_rect = DstRectInit(0, 0);
                src_rect2 = SrcRectInit(0, 0, 0, WINDOW_HEIGHT);
                dst_rect2 = DstRectInit(0, 0);
            }
            break;
        }
        result = 1;
    }
    else{ //遊んでいるとき
        switch(stage){
        case 1:
            SDL_BlitSurface(haikei1, &src_rect, window, &dst_rect);
            break;
        case 2:
            SDL_BlitSurface(haikei2, &src_rect, window, &dst_rect);
            break;
        case 3:
            SDL_BlitSurface(haikei3, &src_rect, window, &dst_rect);
            break;
        }
        result = 1;
    }

    boxColor(window, 30/*1280*/, 30, WINDOW_WIDTH-30, 120, 0xffffffaa);
//体力ゲージの描画
    if(HP != 0){
        boxColor(window, 50, 50, (WINDOW_WIDTH - 400)*HP / HP_M, 100, 0x00ff00ff);

        StringDraw(HP, 0);
    }

//スコアの設定
    if(Score_Plus > 0){
        if(Score_Plus >= 10){
            Total_Score += 10;
            Score_Plus -= 10;
        }
        else{
            Total_Score += Score_Plus;
            Score_Plus = 0;
        }
    }
    StringDraw(Total_Score, 1);

//攻撃レベル
    int power = 1;
    StringDraw(power,2);

//速度レベル
    int speed = 1;
    StringDraw(speed, 3);

    if(stageFlag == 1){
//敵の描画
        EnemyDraw();
    }

//プレイヤーの動作
    for(i = 0; i < num_clients; i++){
        if(stageFlag == 1)
            PlayerAction(i);
        PlayerDraw(i);
    }

    if(stageFlag == 1){
//その他の動作
        EnemyEnter();

        EnemyMove(num_clients, myid, sock);
        PlayerShotCalc(myid, sock);
        EnemyBulletMove(num_clients, myid, sock);
    }
    return result;
}

/************************************
static int DrawGameOver()
引数：
機能：
返値：result
*************************************/
static int DrawGameOver(){

    fd_set read_flag = mask;

    struct timeval timeout;
    timeout.tv_sec = 0; //クライアント側
    timeout.tv_usec = 30; //ユーザー側

//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1) {
        handle_error("select()");
    }

    int result = 1;
    if(stageFlag == 1){
        EventOver(myid, sock);
    }
    if (FD_ISSET(sock, &read_flag)) {
        result = execute_command(); //受信
    }

    SDL_FillRect(window, NULL, SDL_MapRGBA(window->format, 255, 255, 255, 255));

    return result;
}

/************************************
static int DrawGameClear()
引数：
機能：
返値：result
*************************************/
static int DrawGameClear(){

    fd_set read_flag = mask;

    struct timeval timeout;
    timeout.tv_sec = 0; //クライアント側
    timeout.tv_usec = 30; //ユーザー側

//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_sock, (fd_set *)&read_flag, NULL, NULL, &timeout) == -1) {
        handle_error("select()");
    }

    int result = 1;
    if(stageFlag == 1){
        EventClear(myid, sock);
    }
    if (FD_ISSET(sock, &read_flag)) {
        result = execute_command(); //受信
    }

    SDL_FillRect(window, NULL, SDL_MapRGBA(window->format, 255, 255, 255, 255));

    return result;
}

/********************************************
static void PlayerAllInit()
引数：
機能：自分の選んだ機種をサーバーからもらう
返値：
*********************************************/
static void PlayerAllInit(int knd) {
    stage = 1;
    stageFlag = 1;

    PlayerInit(num_clients);
    //PlayerDataLoad();
    EnemyInit();
    EnemyDataLoad();
    
    src_rect = SrcRectInit(100, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    dst_rect = DstRectInit(0, 0);
    src_rect2 = SrcRectInit(0, 0, 0, WINDOW_HEIGHT);
    dst_rect2 = DstRectInit(0, 0);

    player[myid].knd = knd; //ここは機体セレクトで決める
    //fprintf(stderr, "player[%d].knd = %d\n", myid, player[myid].knd);
    CONTAINER data;
    memset(&data, 0, sizeof(CONTAINER));

    data.command = DATA_PULL;
    data.cid = myid;
    data.state = gstate;
    data.player = player[myid];
    data.flag = 2;
    send_data(&data, sizeof(CONTAINER), sock);

    //PlayerEnter(num_clients, sock); //ロードするとき（ゲーム開始前）に取り入れる
}


/**********************************
static int execute_command()
引数：なし
機能：サーバーからクライアントへの受信
返値：
***********************************/
static int execute_command() {
    CONTAINER data;
    int result = 1;
    int k;
    memset(&data, 0, sizeof(CONTAINER));
    receive_data(&data, sizeof(data));
    switch(gstate) {
//GAME_TITLE
    case GAME_TITLE:
        switch(data.command) {
        case FOUR_COMMAND:
            fprintf(stderr, "Go to Game!\n");
            gstate = data.state;
            stageFlag = 1;
            //PlayerAllInit(3);
            result = 1;
            break;
        case END_COMMAND:
            fprintf(stderr, "client[%d] %s sent quit command.\n", data.cid, clients[data.cid].name);
            result = 0; //チャットを終了
            break;
        default:
            fprintf(stderr, "execute_command(): %c is not a valid command.\n", data.command);    
            exit(1);
            break;
        }
        break;

//GAME_SELECT
    case GAME_SELECT:
        switch(data.command) {
        case LEFT_COMMAND:
        case RIGHT_COMMAND:
        case UP_COMMAND:
        case DOWN_COMMAND:
            pla_sele[data.cid].kndP = data.kndP;
            fprintf(stderr, "pla_sele[%d] = %d\n", data.cid, data.kndP);
            result = 1;
            break;
        case FOUR_COMMAND:
            switch(data.flag) {
            case 3:
                pla_sele[data.cid].kPflag = data.kPflag;
                result = 1;
                break;
            case 4:
                gstate = data.state;
                stageFlag = 1;
                PlayerAllInit(pla_sele[myid].kndP);
                int i;
                for(i = 0; i < num_clients; i++){
                    pla_sele[i].kndP = i;
                    pla_sele[i].kPflag = 0;
                }
                result = 1;
                break;
            }
            break;
        case THREE_COMMAND:

            break;
        case END_COMMAND:
            fprintf(stderr, "client[%d] %s sent quit command.\n", data.cid, clients[data.cid].name);
            result = 0; //チャットを終了
            break;
        default:
            fprintf(stderr, "execute_command(): %c is not a valid command.\n", data.command);    
            exit(1);
            break;
        }
        break;

//GAME_LOAD
    case GAME_LOAD:
        switch(data.command) {
        case END_COMMAND:
            fprintf(stderr, "client[%d] %s sent quit command.\n", data.cid, clients[data.cid].name);
            result = 0; //チャットを終了
            break;
        default:
            fprintf(stderr, "execute_command(): %c is not a valid command.\n", data.command);    
            exit(1);
            break;
        }
        break;

//GAME_MAIN
    case GAME_MAIN:
        switch (data.command) { //クライアントのチャットの行動
        case UP_COMMAND: //メッセージが送信されたら
            player[data.cid].command.up = 1;
            result = 1;
            break;
        case DOWN_COMMAND: //メッセージが送信されたら
            player[data.cid].command.down = 1;
            result = 1;
            break;
        case LEFT_COMMAND: //メッセージが送信されたら
            player[data.cid].command.left = 1;
            result = 1;
            break;
        case RIGHT_COMMAND: //メッセージが送信されたら
            player[data.cid].command.right = 1;
            result = 1;
            break;

        case SEPARATE_UPDO_COMMAND:
            player[data.cid].command.up = 0;
            player[data.cid].command.down = 0;
            if(myid != data.cid){
                player[data.cid].tx = data.tx;
                player[data.cid].ty = data.ty;
            }
            result = 1;
            break;
        case SEPARATE_LERI_COMMAND:
            player[data.cid].command.left = 0;
            player[data.cid].command.right = 0;
            if(myid != data.cid){
                player[data.cid].tx = data.tx;
                player[data.cid].ty = data.ty;
            }
            result = 1;
            break;

        case UP_ROTA:
            player[data.cid].command.rotaU = 1;
            result = 1;
            break;
        case RIGHT_ROTA:
            player[data.cid].command.rotaR = 1;
            result = 1;
            break;
        case LEFT_ROTA:
            player[data.cid].command.rotaL = 1;
            result = 1;
            break;

        case UP_SEPA_ROTA:
            player[data.cid].command.rotaU = 0;
            result = 1;
            break;
        case RIGHT_SEPA_ROTA:
            player[data.cid].command.rotaR = 0;
            result = 1;
            break;
        case LEFT_SEPA_ROTA:
            player[data.cid].command.rotaL = 0;
            result = 1;
            break;

        case SHOT_COMMAND:
            player[data.cid].command.b5 = 1;
            //player[data.cid].tx = data.tx;
            //player[data.cid].ty = data.ty;
            result = 1;
            break;
        case SHOT_FINISH_COMMAND:
            player[data.cid].command.b5 = 0;
            //player[data.cid].tx = data.tx;
            //player[data.cid].ty = data.ty;
            result = 1;
            break;

        case PLAYER_HIT:
            PlayerDamage(data, myid, sock);
            result = 1;
            break;
        case PLAYER_HIT2:
            PlayerDamage2(data, myid, sock);
            result = 1;
            break;
        case ENEMY_HIT:
            k = EnemyDamage(data);
            if(stage % 3 != 0){ //ザコ面
                if(k % 10 == 0 + stage/3){
                    stage++;
                    stageFlag = stage;
                    PlayerBulletClean();
                    EnemyBulletClean();
                }
            }
            else { //ボス面
                if(k == 21) {//中ボス１を倒したら
                    memset(&data, 0, sizeof(CONTAINER));
                    data.command = END_COMMAND;
                    fprintf(stderr, "%d\n", data.command);
                    data.cid = myid;
                    data.state = gstate;
                    send_data(&data, sizeof(CONTAINER), sock);

                    fprintf(stderr, "////////////////////\n");
                    fprintf(stderr, "//     Clear!     //\n");
                    fprintf(stderr, "////////////////////\n");
                }
            }
            result = 1;
            break;

        case DATA_PULL:
            switch(data.flag){
            case 2:
                player[data.cid] = data.player;
                fprintf(stderr, "%d:in! %d\n", data.cid, player[data.cid].knd);
                PlayerShotEnter(data.cid, num_clients);
                if(data.hp > 0){
                    HP_M = data.hp;
                    HP = HP_M;
                }
                break;
            }
            break;

        case END_COMMAND: //誰かが退室したら
            fprintf(stderr, "client[%d] %s sent quit command.\n", data.cid, clients[data.cid].name);
            result = 0; //チャットを終了
            break;
        default:
            fprintf(stderr, "execute_command(): %c is not a valid command.\n", data.command);    
            exit(1);
            break;
        }
        break;

//GAME_OVER
    case GAME_OVER:
        switch(data.command) {
        case FOUR_COMMAND:

            break;
        case END_COMMAND: //誰かが退室したら
            fprintf(stderr, "client[%d] %s sent quit command.\n", data.cid, clients[data.cid].name);
            result = 0; //チャットを終了
            break;
        default:
            fprintf(stderr, "execute_command(): %c is not a valid command.\n", data.command);    
            exit(1);
            break;
        }
        break;

//GAME_CLEAR
    case GAME_CLEAR:
        switch(data.command) {
        case FOUR_COMMAND:

            break;
        case END_COMMAND: //誰かが退室したら
            fprintf(stderr, "client[%d] %s sent quit command.\n", data.cid, clients[data.cid].name);
            result = 0; //チャットを終了
            break;
        default:
            fprintf(stderr, "execute_command(): %c is not a valid command.\n", data.command);    
            exit(1);
            break;
        }
        break;
    }
    return result;
}

/*****************************************
static void send_data(void *data, int size)
引数：*data 書きこむデータ
       size データのサイズ
機能：データをソケット(sock)に入れる
 ****************************************/
/*static void send_data(void *data, int size) {
  if ((data == NULL) || (size <= 0)) {
        fprintf(stderr, "send_data(): data is illeagal.\n");
	exit(1);
  }

  if(write(sock, data, size) == -1) { //sockにdataのsizeバイト分書きこむ
      handle_error("write()"); //エラー
  }    
}*/

/*******************************************
static int receive_data(void *data, int size)
引数：*data 読み込みデータ
       size 読み込みデータのサイズ
機能：コネクションからバッファへのデータの読み込み
返値：読み込み結果
********************************************/
static int receive_data(void *data, int size) {
  if ((data == NULL) || (size <= 0)) {
        fprintf(stderr, "receive_data(): data is illeagal.\n");
	exit(1);
  }

  return(read(sock, data, size)); //引数１：接続ソケット　引数２：読み込み先のバッファ　引数３：読み込みバイト数
}

/**********************************************
static void handle_error(char *message)
引数：*message エラーメッセージ
機能：エラーメッセージの送信
***********************************************/
void handle_error(char *message) {
  perror(message);
  fprintf(stderr, "%d\n", errno);
  exit(1);
}
/**********************************************
void terminate_client()
引数：なし
機能：クライアントの終了
***********************************************/
void terminate_client() {
  fprintf(stderr, "Connection is closed.\n");
  close(sock);
  exit(0);
}


void HaikeiFree(){
    SDL_FreeSurface(haikei1);
    SDL_FreeSurface(haikei2);
    SDL_FreeSurface(haikei3);
}


SDL_Rect SrcRectInit(int x, int y, int w, int h){
    SDL_Rect tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.w = w;
    tmp.h = h;

    return tmp;
}


SDL_Rect DstRectInit(int x, int y){
    SDL_Rect tmp;

    tmp.x = x;
    tmp.y = y;
    
    return tmp;
}




