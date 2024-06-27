#include <BleKeyboard.h>
#include "driver/ledc.h"


BleKeyboard bleKeyboard;

const int KEYIN[] = {18, 19};
const int KEYOUT[] = {13, 12};
const int CONNECTION_CHECK_PIN = 14;
const int BUZZER_TEST = 27;

int touchToneDuration = 100;
int sendToneDuration = 400;

unsigned long previousMillis = 0;
const long interval = 10; // スキャン間隔を30msから10msに短縮
int columnNum = 0;
int sw[2][2] = {0};
char keyMap[2][2] = {
  {'1','2'},
  {'3','4'}
};

// 初期化関数
void initBuzz() {
  ledcSetup(0, 988, 8);  // チャンネル0、988Hz、8ビットの分解能
  ledcAttachPin(BUZZER_TEST, 0);  // BUZZER_TESTピンにチャンネル0を接続
}




void setup() {
  pinMode(CONNECTION_CHECK_PIN, OUTPUT);

  for(int i = 0; i < 2; i++){
    pinMode(KEYOUT[i], OUTPUT);
    digitalWrite(KEYOUT[i], LOW);
  }
  for(int i = 0; i < 2; i++){
    pinMode(KEYIN[i], INPUT);
  }
  
  bleKeyboard.begin();
  Serial.begin(9600);

  pinMode(BUZZER_TEST, OUTPUT);
  initBuzz();  // ブザーの初期化
}

void startBuzz() {
  ledcWrite(0, 128);  // チャンネル0、デューティーサイクル50%
}

void stopBuzz() {
  ledcWrite(0, 0);  // チャンネル0、デューティーサイクル0%
}

void buzz(){
  startBuzz();
  delay(touchToneDuration);
  stopBuzz();
}

void keyPress(char key) {
  buzz();
  switch (key) {
    case '1':
      bleKeyboard.write('1');
      break;
    case '2':
      bleKeyboard.write('2');
      break;
    case '3':
      bleKeyboard.write('3');
      break;
    case '4':
      break;
    default:
      break;
  }
}

void loop() {
  if (bleKeyboard.isConnected()) {
    digitalWrite(CONNECTION_CHECK_PIN, HIGH);
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(KEYOUT[columnNum], HIGH);

      for (int rowNum = 0; rowNum < 2; rowNum++) {
        int readValue = digitalRead(KEYIN[rowNum]);
        if (readValue != sw[columnNum][rowNum] && readValue == HIGH) {
          delay(5); // デバウンス処理のディレイを10msから5msに短縮
          readValue = digitalRead(KEYIN[rowNum]); // もう一度読み取る 
          if (readValue == HIGH) {
            keyPress(keyMap[columnNum][rowNum]);
          }
        }

        sw[columnNum][rowNum] = readValue;
      }

      digitalWrite(KEYOUT[columnNum], LOW);
      columnNum++;
      columnNum %= 2;
    }
  } else {
    digitalWrite(CONNECTION_CHECK_PIN, LOW);
  }
}
