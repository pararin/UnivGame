/*****************************************************************
ファイル名	: client_command.c
機能		: クライアントのコマンド処理
*****************************************************************/

#include"common.h"
#include"client_func.h"

static void SetIntData2DataBlock(void *data,int intData,int *dataSize);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void RecvUpData(void);
static void RecvDownData(void);
static void RecvLeftData(void);
static void RecvRightData(void);
static void RecvSepaUdData(void);
static void RecvSepaLrData(void);
static void RecvShotData(void);
static void RecvShotFinishData(void);
static void RecvDiamondData(void);

/*****************************************************************
関数名	: ExecuteCommand
機能	: サーバーから送られてきたコマンドを元に，
		  引き数を受信し，実行する
引数	: char	command		: コマンド
出力	: プログラム終了コマンドがおくられてきた時には0を返す．
		  それ以外は1を返す
*****************************************************************/
int ExecuteCommand(char command)
{
    int	endFlag = 1;
#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("command = %c\n",command);
#endif
    switch(command){
    case END_COMMAND:
        endFlag = 0;
        break;
    case UP_COMMAND:
        RecvUpData();
        break;
    case DOWN_COMMAND:
        RecvDownData();
        break;
    case LEFT_COMMAND:
        RecvLeftData();
        break;
    case RIGHT_COMMAND:
        RecvRightData();
        break;
    case SEPARATE_UPDO_COMMAND:
        RecvSepaUdData();
        break;
    case SEPARATE_LERI_COMMAND:
        RecvSepaLrData();
        break;
    case SHOT_COMMAND:
        RecvShotData();
        break;
    case SHOT_FINISH_COMMAND:
        RecvShotFinishData();
        break;
    }
    return endFlag;
}


/*****************************************************************
関数名	: SendUpCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendUpCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,UP_COMMAND,&dataSize);
    /* クライアント番号のセット */
    //SetIntData2DataBlock(data,pos,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendDownCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendDownCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,DOWN_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendLeftCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendLeftCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,LEFT_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendRightCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendRightCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,RIGHT_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendSeparateCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendSeparateCommand(int pos, int muki)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    if(muki == 0) //左右
        SetCharData2DataBlock(data,SEPARATE_LERI_COMMAND,&dataSize);
    else if(muki == 1)
        SetCharData2DataBlock(data,SEPARATE_UPDO_COMMAND,&dataSize);
    SetIntData2DataBlock(data, muki, &dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendShotCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendShotCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,SHOT_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendShotFinishCommand
機能	: クライアントに円を表示させるために，
		  サーバーにデータを送る
引数	: int		pos	    : 円を表示させるクライアント番号
出力	: なし
*****************************************************************/
void SendShotFinishCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* 引き数チェック */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,SHOT_FINISH_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}


/*****************************************************************
関数名	: SendEndCommand
機能	: プログラムの終了を知らせるために，
		  サーバーにデータを送る
引数	: なし
出力	: なし
*****************************************************************/
void SendEndCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendEndCommand()\n");
#endif
    dataSize = 0;
    /* コマンドのセット */
    SetCharData2DataBlock(data,END_COMMAND,&dataSize);

    /* データの送信 */
    SendData(data,dataSize);
}

/*****
static
*****/
/*****************************************************************
関数名	: SetIntData2DataBlock
機能	: int 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetIntData2DataBlock(void *data,int intData,int *dataSize)
{
    int tmp;

    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    tmp = htonl(intData);

    /* int 型のデータを送信用データの最後にコピーする */
    memcpy(data + (*dataSize),&tmp,sizeof(int));
    /* データサイズを増やす */
    (*dataSize) += sizeof(int);
}

/*****************************************************************
関数名	: SetCharData2DataBlock
機能	: char 型のデータを送信用データの最後にセットする
引数	: void		*data		: 送信用データ
		  int		intData		: セットするデータ
		  int		*dataSize	: 送信用データの現在のサイズ
出力	: なし
*****************************************************************/
static void SetCharData2DataBlock(void *data,char charData,int *dataSize)
{
    /* 引き数チェック */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* char 型のデータを送信用データの最後にコピーする */
    *(char *)(data + (*dataSize)) = charData;
    /* データサイズを増やす */
    (*dataSize) += sizeof(char);
}

/*****************************************************************
関数名	: RecvUpData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvUpData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    //RecvIntData(&x);
    RecvIntData(&pos);

    //fprintf(stderr, "x = %d\n", x);
    fprintf(stderr, "UP:pos = %d\n", pos);

    //RecvIntData(&y);
    //RecvIntData(&r);

    /* 円を表示する */
    DrawUpCircle(pos);
    player[pos].command.up = 1;
}

/*****************************************************************
関数名	: RecvDownData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvDownData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "DOWN:pos = %d\n", pos);

    /* 円を表示する */
    DrawDownCircle(pos);
    player[pos].command.down = 1;
}

/*****************************************************************
関数名	: RecvLeftData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvLeftData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "LEFT:pos = %d\n", pos);

    /* 円を表示する */
    DrawLeftCircle(pos);
    player[pos].command.left = 1;
}

/*****************************************************************
関数名	: RecvRightData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvRightData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "RIGHT:pos = %d\n", pos);

    /* 円を表示する */
    DrawRightCircle(pos);
    player[pos].command.right = 1;
}

/*****************************************************************
関数名	: RecvSepaUdData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvSepaUdData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "SEPA1:pos = %d\n", pos);

    /* 円を表示する */
    player[pos].command.up = 0;
    player[pos].command.down = 0;
}

/*****************************************************************
関数名	: RecvSepaLrData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvSepaLrData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "SEPA2:pos = %d\n", pos);

    /* 円を表示する */
    player[pos].command.left = 0;
    player[pos].command.right = 0;
}

/*****************************************************************
関数名	: RecvShotData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvShotData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "SHOT:pos = %d\n", pos);

    /* 円を表示する */
    PlayerBulletEnter(pos); //client_PlayerData.c にあり
    player[pos].command.b5 = 1;
}


/*****************************************************************
関数名	: RecvShotFinishData
機能	: 円を表示するためのデータを受信し，表示する
引数	: なし
出力	: なし
*****************************************************************/
static void RecvShotFinishData(void)
{
    int	pos;

    /* 円コマンドに対する引き数を受信する */
    RecvIntData(&pos);

    fprintf(stderr, "SHOT_FINISH:pos = %d\n", pos);

    /* 円を表示する */
    player[pos].command.b5 = 0;
}
