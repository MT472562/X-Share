#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

const char* ssid = "ssid";         // Wi-FiのSSID
const char* password = "pass";         // Wi-Fiのパスワード
const char* webhookUrl = "WebHookのURL"; // Discord WebHookのURL

const char* ntpServer = "pool.ntp.org"; // NTPサーバー
const long gmtOffset_sec = 9 * 3600;    // 日本の標準時（GMT+9）
const int daylightOffset_sec = 0;       // サマータイムなし

#define MAGNETIC_SWITCH_PIN 5           // 磁気リードスイッチが接続されているピン
#define ALERT_DELAY 120000              // 2分（120000ミリ秒）
#define CONFIRMATION_COUNT 5            // 確定するための連続一致回数

unsigned long doorOpenedTime = 0;  // ドアが開いた時間
bool doorIsOpen = false;           // ドアの状態フラグ
int confirmationCounter = 0;       // 確定カウンター
int lastCheckedDay = -1;           // 前回の確認日

void setup() {
  Serial.begin(115200);
  pinMode(MAGNETIC_SWITCH_PIN, INPUT_PULLUP); 
  // Wi-Fi接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi接続中...");
  }
  Serial.println("WiFi接続成功");

  // NTPからの時間取得の設定
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("NTPサーバーから時刻を取得中...");
}

void loop() {
  // 現在の時刻を取得
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo)) {
    Serial.println("時刻を取得できませんでした。");
    delay(1000);
    return;
  }

  int currentHour = timeInfo.tm_hour;
  int currentMinute = timeInfo.tm_min;
  int currentSecond = timeInfo.tm_sec;
  int currentDay = timeInfo.tm_mday;

  // 時刻をシリアルモニタに出力
  Serial.printf("現在時刻: %02d:%02d:%02d\n", currentHour, currentMinute, currentSecond);

  // 毎日6時に通知を送信（1日1回のみ）
  if (currentHour == 18 && currentMinute == 0 && currentSecond == 0) {
    if (currentDay != lastCheckedDay) {
      sendDiscordNotification("冷凍庫のドア開閉確認システムは正常に稼働しています。この通知は毎日午後6時に送信されます");
      lastCheckedDay = currentDay; // チェックした日を更新
      Serial.println("6時の通知を送信しました。");
    }
  }

  // ドアの状態確認
  int switchState = digitalRead(MAGNETIC_SWITCH_PIN);
  switchState = !switchState;
  Serial.printf("ドア状態: %s\n", switchState == LOW ? "開いている" : "閉じている");
  if (switchState == LOW) { // ドアが開いていると読み取られたとき
    confirmationCounter++;
  } else {
    confirmationCounter = 0; // スイッチが閉じていると読み取られたらカウンターをリセット
  }

  // 一定回数連続でドアが開いていると確認された場合
  if (confirmationCounter >= CONFIRMATION_COUNT) {
    if (!doorIsOpen) {
      doorOpenedTime = millis(); // ドアが開いた時間を記録
      doorIsOpen = true;
      Serial.println("ドアが開きました。");
    } else {
      unsigned long elapsedMillis = millis() - doorOpenedTime;
      unsigned long elapsedSeconds = elapsedMillis / 1000;
      Serial.printf("ドアが開いてから %lu 秒が経過しました。\n", elapsedSeconds);

      // 2分経過ごとに通知を送信
      if (elapsedMillis > ALERT_DELAY) {
        sendDiscordNotification("警告: 裏の冷凍庫のドアが2分以上開いたままです！ドアを閉じてください");
        doorOpenedTime = millis(); // カウンターをリセットして再度計測
        Serial.println("2分経過の警告を送信しました。");
      }
    }
  }

  // ドアが閉じたとき
  if (switchState == HIGH && doorIsOpen) {
    doorIsOpen = false;
    unsigned long totalOpenTimeMillis = millis() - doorOpenedTime;
    unsigned long totalOpenTimeSeconds = totalOpenTimeMillis / 1000;
    Serial.printf("ドアが閉まりました。開いていた時間: %lu 秒\n", totalOpenTimeSeconds);
    confirmationCounter = 0; // カウンターをリセット
  }

  delay(1000); // 1秒待機
}

void sendDiscordNotification(const char* message) {
  if (WiFi.status() == WL_CONNECTED) { // Wi-Fi接続確認
    HTTPClient http;
    http.begin(webhookUrl); // Discord WebHookのURLを設定
    http.addHeader("Content-Type", "application/json");

    String payload = String("{\"content\":\"") + message + String("\"}");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.printf("Discord通知送信成功: %d\n", httpResponseCode);
    } else {
      Serial.printf("Discord通知送信失敗: %d\n", httpResponseCode);
    }

    http.end(); // 接続を終了
  } else {
    Serial.println("Wi-Fi未接続のため、通知送信できませんでした。");
  }
}
