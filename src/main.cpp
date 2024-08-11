#include "Arduino.h"

#include <LovyanGFX_DentaroUI.hpp>

LGFX lcd;
LGFX_Sprite screen(&lcd);

#include <Arduboy2.h>
#include <cglab.h>

uint32_t preTime = 0;
LovyanGFX_DentaroUI ui(&lcd);

#define MAX_CHANNEL 2

EXTERNC void setupGameSurvivor(void);

// タイマー設定用変数
hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;

int pressedBtnID = -1;//この値をタッチボタン、物理ボタンの両方から操作してbtnStateを間接的に操作している

volatile uint32_t pinToggleCount[MAX_CHANNEL];

// 変数
volatile uint8_t playingFlag = 0;
// volatile uint16_t pinToggleCount[2] = {0, 0};

// タイマー1の割り込みハンドラ
void IRAM_ATTR onTimer1() {
  if (playingFlag & (1 << 1)) {
    // toggleTimerPin(1) 相当の処理
  }
  if (pinToggleCount[1] > 0 && --pinToggleCount[1] == 0) {
    // forwardSound(1) 相当の処理
  }
}

// タイマー2の割り込みハンドラ
void IRAM_ATTR onTimer2() {
  if (playingFlag & (1 << 0)) {
    // toggleTimerPin(3) 相当の処理
  }
  if (pinToggleCount[0] > 0 && --pinToggleCount[0] == 0) {
    // forwardSound(0) 相当の処理
  }
}


uint8_t getPhBtnInput(){
  uint8_t buttons = 0;




if(ui.getPhVolVec(0, 1)==7){buttons |= INPUT_LEFT;}
if(ui.getPhVolVec(0, 1)==1){buttons |= INPUT_DOWN;}
if(ui.getPhVolVec(0, 1)==3){buttons |= INPUT_RIGHT;}
if(ui.getPhVolVec(0, 1)==5){buttons |= INPUT_UP;}

if(ui.getPhVolDir(2)==7){buttons |= INPUT_A;}
if(ui.getPhVolDir(2)==1){buttons |= INPUT_B;}
if(ui.getPhVolDir(2)==3){buttons |= INPUT_A;}
if(ui.getPhVolDir(2)==5){buttons |= INPUT_B;}


ui.updatePhBtns();//物理ボタン3つ

if(ui.getHitValue() == 101||ui.getHitValue() == 111){buttons |= INPUT_A;}
if(ui.getHitValue() == 102||ui.getHitValue() == 112){buttons |= INPUT_B;}
if(ui.getHitValue() == 103||ui.getHitValue() == 113){buttons |= INPUT_DOWN;}



  return buttons;
}


void setup() {
  

  
   // arduboy.begin();
   // タイマー1の設定
  timer1 = timerBegin(0, 80, true); // タイマー0, プレスケーラ80で1µs単位
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 1000000, true); // 1秒毎の割り込み
  timerAlarmEnable(timer1);

  // タイマー2の設定
  timer2 = timerBegin(1, 80, true); // タイマー1, プレスケーラ80で1µs単位
  timerAttachInterrupt(timer2, &onTimer2, true);
  timerAlarmWrite(timer2, 1000000, true); // 1秒毎の割り込み
  timerAlarmEnable(timer2);

  // pinToggleCount の初期化
  for (int i = 0; i < MAX_CHANNEL; ++i) {
    pinToggleCount[i] = 0;
  }

  Serial.begin(115200);
  // Serial.begin(9600);
  delay(50);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  //  SD.end();
  // // SDカードがマウントされているかの確認

  if(SD.begin(SDCARD_SS_PIN)){//custom設定のCPUのクロック周波数の定数分の1でないとタイミングが合わず、動かなくなるもよう
  //   Serial.println("Card Mount Successful");
  //   isCardMounted = true;
  //   delay(500);
  } else {
  //   Serial.println("Card Mount Failed");
  //   // SDカードのマウントに失敗した場合は、フラグをfalseに設定し、後で再試行する
  //   isCardMounted = false;
  //   // while (1) {}
  }

  ui.begin( lcd, 16, 1, false);

  screen.setPsram( false );//DMA利用のためPSRAMは切る
  screen.createSprite( 128, 64 );
  screen.startWrite();//CSアサート開始

  // arduboy.begin();

  //ゲーム内のprint時の文字設定をしておく
  //  screen.setTextSize(1);//サイズ
  //  screen.setFont(&lgfxJapanGothicP_8);//日本語可
  //  // screen.setCursor(0, 0);//位置
  //  screen.setTextColor(WHITE, BLACK);
  //  screen.setTextWrap(true);

  setupGameSurvivor();
  initGame();
}

void loop()
{
  ui.updatePhVols();//volボタン関連
  ui.setConstantGetF(true);//trueだとタッチポイントのボタンIDを連続取得するモード
  ui.update(lcd);//タッチイベントを取るので、LGFXが基底クラスでないといけない
  

  if( ui.getEvent() != NO_EVENT ){//何かイベントがあれば
    if( ui.getEvent() == TOUCH ){//TOUCHの時だけ
      
    }
    if(ui.getEvent() == MOVE){
      pressedBtnID = ui.getTouchBtnID();
    }
    if( ui.getEvent() == RELEASE ){//RELEASEの時だけ
      // ui.setBtnID(-1);//タッチボタンIDをリセット
      // pressedBtnID = ui.getTouchBtnID()+12;//12個分の物理ボタンをタッチボタンIDに足す
      pressedBtnID = -1;//リセット
    }
  }
 
  updateGame(getPhBtnInput());

  // screen.fillScreen(TFT_RED);

//  arduboy.setCursor(0,0);
//  arduboy.print("Hello World");

  screen.setPivot(0, 0);
  screen.pushRotateZoom(&lcd, 0, 0, 0, 1,1);

  delay(1);
}
//
//Arduboy2 arduboy;
//
//// void setup() {  
////   arduboy.begin();
////   arduboy.clear();
//
//// }
//
//// void loop() {
////   arduboy.clear();
//
////   arduboy.setCursor(0,0);
////   arduboy.print("Hello World");
//
////   arduboy.display();
//// }
//
//
//int pressedBtnID = -1;//この値をタッチボタン、物理ボタンの両方から操作してbtnStateを間接的に操作している
//
//// LGFX screen;//LGFXを継承
//LGFX lcd;
//LGFX_Sprite screen(&lcd);
//uint32_t preTime = 0;
//
//LovyanGFX_DentaroUI ui(&lcd);
//
//
//void setup()
//{
//  // arduboy.begin();
//
//  Serial.begin(115200);
//  // Serial.begin(9600);
//  delay(50);
//
//  if (!SPIFFS.begin(true))
//  {
//    Serial.println("An Error has occurred while mounting SPIFFS");
//    return;
//  }
//
//  //  SD.end();
//  // // SDカードがマウントされているかの確認
//
//  if(SD.begin(SDCARD_SS_PIN)){//custom設定のCPUのクロック周波数の定数分の1でないとタイミングが合わず、動かなくなるもよう
//  //   Serial.println("Card Mount Successful");
//  //   isCardMounted = true;
//  //   delay(500);
//  } else {
//  //   Serial.println("Card Mount Failed");
//  //   // SDカードのマウントに失敗した場合は、フラグをfalseに設定し、後で再試行する
//  //   isCardMounted = false;
//  //   // while (1) {}
//  }
//
//  ui.begin( lcd, 16, 1, false);
//
//  screen.setPsram( false );//DMA利用のためPSRAMは切る
//  screen.createSprite( 128, 64 );
//  screen.startWrite();//CSアサート開始
//
//  // arduboy.begin();
//
//  //ゲーム内のprint時の文字設定をしておく
//  screen.setTextSize(1);//サイズ
//  screen.setFont(&lgfxJapanGothicP_8);//日本語可
//  // screen.setCursor(0, 0);//位置
//  screen.setTextColor(WHITE, BLACK);
//  screen.setTextWrap(true);
//}
//
//
//void loop()
//{
//
//  // ui.setConstantGetF(true);//trueだとタッチポイントのボタンIDを連続取得するモード
//  ui.update(lcd);//タッチイベントを取るので、LGFXが基底クラスでないといけない
//
//  if( ui.getEvent() != NO_EVENT ){//何かイベントがあれば
//
//    if( ui.getEvent() == TOUCH ){//TOUCHの時だけ
//      
//    }
//    if(ui.getEvent() == MOVE){
//      pressedBtnID = ui.getTouchBtnID();
//    }
//    if( ui.getEvent() == RELEASE ){//RELEASEの時だけ
//      // ui.setBtnID(-1);//タッチボタンIDをリセット
//      // pressedBtnID = ui.getTouchBtnID()+12;//12個分の物理ボタンをタッチボタンIDに足す
//      pressedBtnID = -1;//リセット
//    }
//  }
//
//  
//  uint32_t now = millis();
//  uint32_t remainTime= (now - preTime);
//  preTime = now;
//
//  screen.fillScreen(TFT_RED);
//
//  arduboy.setCursor(0,0);
//  arduboy.print("Hello World");
//
//  screen.setPivot(0, 0);
//  screen.pushRotateZoom(&lcd, 0, 0, 0, 1,1);
//
//  delay(1);
//}
