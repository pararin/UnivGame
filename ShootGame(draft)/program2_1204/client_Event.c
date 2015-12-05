#include "common.h"
#include "client_func.h"

/*****************************************************************
´Ø¿ôÌ¾	: EventMainFighter
µ¡Ç½	: ¥á¥¤¥ó¥¦¥¤¥ó¥É¥¦¤ËÂÐ¤¹¤ë¥¤¥Ù¥ó¥È½èÍý¤ò¹Ô¤¦
°ú¿ô	: int		num		: Á´¥¯¥é¥¤¥¢¥ó¥È¿ô
½ÐÎÏ	: ¤Ê¤·
*****************************************************************/
void EventMainFighter(int num)
{
    SDL_Event event;
    int buttonNO;

    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(0<num && num<=MAX_CLIENTS);

//コントローラーから
    if(SDL_PollEvent(&event)){
        switch(event.type){

        case SDL_QUIT:
            SendEndCommand();
            break;


        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
            case SDLK_ESCAPE:
                SendEndCommand();
                break;
            }
            break;


	case SDL_JOYAXISMOTION:
            //printf("The axis ID of the operated key is %d.\n",event.jaxis.axis);	// 操作された方向キーの方向軸を表示（0：アナログキー，1：アナログキー，2：方向キー左右方向，3：方向キー上下方向）
            if(event.jaxis.axis==0){
                if(event.jaxis.value == -32768){
                    SendLeftCommand(1);
                    //printf("左\n");
                }
                if(event.jaxis.value == 32767){
                    SendRightCommand(1);
                    //printf("右\n");
                }
                //printf("--- Analog-Direction Key: ?? Axis\n");
            }
            else if(event.jaxis.axis==1){
                if(event.jaxis.value == -32768){
                    SendUpCommand(1);
                    //printf("上\n");
                }
                if(event.jaxis.value == 32767){
                    SendDownCommand(1);
                    //printf("下\n");
                }
                //printf("--- Anolag-Direction Key: ?? Axis\n");
            }					
            //printf("--- The axis value of the operated key is %d.\n",event.jaxis.value);	// ジョイスティックの操作された方向キーの値を表示（-32767（真左，真上）～32767（真右，真下））
            if(event.jaxis.value == 0){
                SendSeparateCommand(1, event.jaxis.axis);
                //printf("%d : 左スティックを離した\n", event.jaxis.axis);
            }
            break;


            // ジョイスティックのボタンが押された時
        case SDL_JOYBUTTONDOWN:
            printf("The ID of the pressed button is %d.\n", event.jbutton.button);	// 押されたボタンのIDを表示（0から）
            // ボタンIDに応じた処理
            switch(event.jbutton.button){
            case 5:
                SendShotCommand(1);
                break;
            }

            if(event.jbutton.button==0){
                //printf("--- You pressed a button on the joystick.\n");
            }
            break;

            // ボタンが離された時
        case SDL_JOYBUTTONUP:
            printf("The ID of the released button is %d.\n", event.jbutton.button);	// 離されたボタンのIDを表示（0から）
            // ボタンIDに応じた処理
            switch(event.jbutton.button){
            case 5:
                SendShotFinishCommand(1);
                break;
            }

            if(event.jbutton.button==0){
                //printf("--- You released a button on the joystick.\n");
            }
            break;
        }
    }
}



/*****************************************************************
´Ø¿ôÌ¾	: EventMainTank
µ¡Ç½	: ¥á¥¤¥ó¥¦¥¤¥ó¥É¥¦¤ËÂÐ¤¹¤ë¥¤¥Ù¥ó¥È½èÍý¤ò¹Ô¤¦
°ú¿ô	: int		num		: Á´¥¯¥é¥¤¥¢¥ó¥È¿ô
½ÐÎÏ	: ¤Ê¤·
*****************************************************************/
void EventMainTank(int num)
{
    SDL_Event event;
    int buttonNO;

    /* °ú¤­¿ô¥Á¥§¥Ã¥¯ */
    assert(0<num && num<=MAX_CLIENTS);

//コントローラーから
    if(SDL_PollEvent(&event)){
        switch(event.type){

        case SDL_QUIT:
            SendEndCommand();
            break;


        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
            case SDLK_ESCAPE:
                SendEndCommand();
                break;
            }
            break;


	case SDL_JOYAXISMOTION:
            //printf("The axis ID of the operated key is %d.\n",event.jaxis.axis);	// 操作された方向キーの方向軸を表示（0：アナログキー，1：アナログキー，2：方向キー左右方向，3：方向キー上下方向）
            if(event.jaxis.axis==0){
                if(event.jaxis.value == -32768){
                    SendLeftCommand(1);
                    //printf("左\n");
                }
                if(event.jaxis.value == 32767){
                    SendRightCommand(1);
                    //printf("右\n");
                }
                //printf("--- Analog-Direction Key: ?? Axis\n");
            }					
            //printf("--- The axis value of the operated key is %d.\n",event.jaxis.value);	// ジョイスティックの操作された方向キーの値を表示（-32767（真左，真上）～32767（真右，真下））
            if(event.jaxis.value == 0){
                SendSeparateCommand(1, event.jaxis.axis);
                //printf("%d : 左スティックを離した\n", event.jaxis.axis);
            }
            break;


            // ジョイスティックのボタンが押された時
        case SDL_JOYBUTTONDOWN:
            printf("The ID of the pressed button is %d.\n", event.jbutton.button);	// 押されたボタンのIDを表示（0から）
            // ボタンIDに応じた処理
            switch(event.jbutton.button){
            case 5:
                SendShotCommand(1);
                break;
            }

            if(event.jbutton.button==0){
                //printf("--- You pressed a button on the joystick.\n");
            }
            break;

            // ボタンが離された時
        case SDL_JOYBUTTONUP:
            printf("The ID of the released button is %d.\n", event.jbutton.button);	// 離されたボタンのIDを表示（0から）
            // ボタンIDに応じた処理
            switch(event.jbutton.button){
            case 5:
                SendShotFinishCommand(1);
                break;
            }

            if(event.jbutton.button==0){
                //printf("--- You released a button on the joystick.\n");
            }
            break;
        }
    }
}



