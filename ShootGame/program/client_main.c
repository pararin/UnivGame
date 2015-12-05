/*****************************************************************
ファイル名	: client_main.c
機能		: クライアントのメインルーチン
*****************************************************************/

#include"common.h"
#include"client_func.h"

int main(int argc,char *argv[])
{
	char	serverName[MAX_NAME_SIZE];
	int		clientID;
    int		num;
    u_short port = PORT;
    int		endFlag = 1;

    /* 引き数チェック */
    switch(argc) {
  	case 1:
    	break;
  	case 2:
    	sprintf(serverName, "%s", argv[1]);
    	break;
  	case 3:
    	sprintf(serverName, "%s", argv[1]);
    	port = (u_short)atoi(argv[2]);
    	break;
  	default:
    	fprintf(stderr, "Usage: %s [server name] [port number]\n", argv[0]);
    	return 1;
  	}

    /* サーバーとの接続 */
    if(SetUpClient(serverName ,port)==-1){
        fprintf(stderr,"setup failed : SetUpClient\n");
        return -1;
    }
    fprintf(stderr, "clientID = %d\n", clientID);

    SetUpClient(serverName ,port); // client側の初期設定
    
    /* ウインドウの初期化 */
    if(InitWindows(clientID,num)==-1){
        fprintf(stderr,"setup failed : InitWindows\n");
        return -1;
    }

	/*画像・データのロード*/
    PlayerLoad();
    PlayerDataLoad();
    EnemyLoad();
    EnemyDataLoad();

	/*各構造体の初期化*/
    PlayerInit(num);
    EnemyInit();

    /* メインイベントループ */
    while(endFlag){
        DrawGameMain(clientID, num);
        endFlag = SendRecvManager();
        //fprintf(stderr, "endFlag = %d\n", endFlag);
    };

    /* 終了処理 */
    DestroyWindow();
    CloseSoc();

    return 0;
}

