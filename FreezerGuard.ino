#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"
#include <WebServer.h>

const char* ssid = "ssid";                                                                                                                // Wi-FiのSSID
const char* password = "pass";                                                                                                                // Wi-Fiのパスワード
const char* webhookUrl = "WebHookのURL";  // Discord WebHookのURL


const char* ntpServer = "pool.ntp.org";  // NTPサーバー
const long gmtOffset_sec = 9 * 3600;     // 日本の標準時（GMT+9）
const int daylightOffset_sec = 0;        // サマータイムなし

#define MAGNETIC_SWITCH_PIN 5  // 磁気リードスイッチが接続されているピン
#define ALERT_DELAY 60000      // 6分（60000ミリ秒）
#define CONFIRMATION_COUNT 5   // 確定するための連続一致回数
int ledPin = 13;
int switchState_html = 0;


unsigned long doorOpenedTime = 0;  // ドアが開いた時間
bool doorIsOpen = false;           // ドアの状態フラグ
int confirmationCounter = 0;       // 確定カウンター
int lastCheckedDay = -1;           // 前回の確認日
WebServer Server(80);
void SendMessage() {
  String msg = R"rawliteral(
<html lang="ja">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>冷蔵庫開閉確認</title>
    <style>
        #open {
            background: linear-gradient(197.37deg, #FF6F61 -0.38%, rgba(255, 158, 143, 0) 101.89%),
                linear-gradient(115.93deg, #FF8C00 4.86%, rgba(255, 165, 0, 0.33) 38.05%, rgba(255, 215, 0, 0) 74.14%),
                radial-gradient(56.47% 76.87% at 6.92% 7.55%, rgba(255, 105, 97, 0.7) 0%, rgba(255, 165, 0, 0.182) 52.16%, rgba(255, 228, 196, 0) 100%),
                linear-gradient(306.53deg, #FFA07A 19.83%, rgba(255, 160, 122, 0) 97.33%);
            background-blend-mode: normal, normal, normal, normal;
            color: white;
        }

        #close {
            background: linear-gradient(197.37deg, #7450DB -0.38%, rgba(138, 234, 240, 0) 101.89%),
                linear-gradient(115.93deg, #3E88F6 4.86%, rgba(62, 180, 246, 0.33) 38.05%, rgba(62, 235, 246, 0) 74.14%),
                radial-gradient(56.47% 76.87% at 6.92% 7.55%, rgba(62, 136, 246, 0.7) 0%, rgba(62, 158, 246, 0.182) 52.16%, rgba(62, 246, 246, 0) 100%),
                linear-gradient(306.53deg, #2EE4E3 19.83%, rgba(46, 228, 227, 0) 97.33%);
            background-blend-mode: normal, normal, normal, normal, normal, normal;
        }

        .container {
            display: grid;
            width: 100%;
            height: 100%;
            place-items: center;
        }

        .ob {
            width: 100%;
            max-width: 600px;
            height: 100%;
            max-height: 600px;
            color: white;
            text-align: center;
            line-height: 200px;
            padding: 20px;
        }

        @media (min-width: 600px) {
            .container {
                justify-content: center;
                align-items: center;
            }
        }
    </style>
</head>

<body>
    <div class="container">
        <div id="open" class="ob" style="display: none;">
            <h2>冷凍庫の扉が開いています</h2>
            <h4>最終更新時間: <span class="time">00:00:00</span></h4>
        </div>
        <div id="close" class="ob" style="display: none;">
            <h2>冷凍庫の扉は閉まっています</h2>
            <h4>最終更新時間: <span class="time">00:00:00</span></h4>
        </div>
    </div>
    <script>
        function fetchData() {
            const url = '/api';
            const timeout = 30000;
            const fetchPromise = fetch(url).then(response => response.json());
            const timeoutPromise = new Promise((_, reject) =>
                setTimeout(() => reject(new Error('Request timed out')), timeout)
            );
            updateTime();

            Promise.race([fetchPromise, timeoutPromise])
                .then(data => {
                    const switchState = data.switchState;
                    const openElement = document.getElementById('open');
                    const closeElement = document.getElementById('close');

                    if (switchState === '1') {
                        openElement.style.display = 'none';
                        closeElement.style.display = 'block';
                    } else if (switchState === '0') {
                        openElement.style.display = 'block';
                        closeElement.style.display = 'none';
                    }
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                });
        }

        setInterval(fetchData, 10000);
        fetchData();
        function updateTime() {
            const now = new Date();
            const hours = String(now.getHours()).padStart(2, '0');
            const minutes = String(now.getMinutes()).padStart(2, '0');
            const seconds = String(now.getSeconds()).padStart(2, '0');
            const timeString = `${hours}:${minutes}:${seconds}`;

            const timeElements = document.querySelectorAll('.time');
            timeElements.forEach(element => {
                element.textContent = timeString;
            });
        }

    </script>
</body>

</html>)rawliteral";
  Server.send(200, "text/html", msg);
}
void SendAPI() {
  String jsonResponse = "{\"switchState\": \"" + String(switchState_html) + "\"}";
  Server.send(200, "application/json", jsonResponse);
}
void SendNotFound() {
  Serial.println("SendNotFound");
  Server.send(404, "text/plain", "404 not found...");
}
void setup() {
  pinMode(ledPin, OUTPUT);
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

  Serial.println(WiFi.localIP());
  Server.on("/", SendMessage);  //  ルートアクセス時の応答
  Server.on("/api", SendAPI);
  Server.onNotFound(SendNotFound);  //  不正アクセス時の応答
  Server.begin();                   //  ウェブサーバ開始
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
      lastCheckedDay = currentDay;  // チェックした日を更新
      Serial.println("6時の通知を送信しました。");
    }
  }

  // ドアの状態確認
  int switchState = digitalRead(MAGNETIC_SWITCH_PIN);
  switchState = !switchState;
  switchState_html = switchState;
  Serial.printf("ドア状態: %s\n", switchState == LOW ? "開いている" : "閉じている");
  if (switchState == LOW) {  // ドアが開いていると読み取られたとき
    confirmationCounter++;
    digitalWrite(ledPin, HIGH);
  } else {
    confirmationCounter = 0;  // スイッチが閉じていると読み取られたらカウンターをリセット
    digitalWrite(ledPin, LOW);
  }

  // 一定回数連続でドアが開いていると確認された場合
  if (confirmationCounter >= CONFIRMATION_COUNT) {
    if (!doorIsOpen) {
      doorOpenedTime = millis();  // ドアが開いた時間を記録
      doorIsOpen = true;
      Serial.println("ドアが開きました。");
    } else {
      unsigned long elapsedMillis = millis() - doorOpenedTime;
      unsigned long elapsedSeconds = elapsedMillis / 1000;
      Serial.printf("ドアが開いてから %lu 秒が経過しました。\n", elapsedSeconds);

      // 2分経過ごとに通知を送信
      if (elapsedMillis > ALERT_DELAY) {
        sendDiscordNotification("警告: 裏の冷凍庫のドアが1分以上開いたままです！ドアを閉じてください");
        doorOpenedTime = millis();  // カウンターをリセットして再度計測
        Serial.println("1分経過の警告を送信しました。");
      }
    }
  }

  // ドアが閉じたとき
  if (switchState == HIGH && doorIsOpen) {
    doorIsOpen = false;
    unsigned long totalOpenTimeMillis = millis() - doorOpenedTime;
    unsigned long totalOpenTimeSeconds = totalOpenTimeMillis / 1000;
    Serial.printf("ドアが閉まりました。開いていた時間: %lu 秒\n", totalOpenTimeSeconds);
    confirmationCounter = 0;  // カウンターをリセット
  }
  Server.handleClient();
  delay(1000);  // 1秒待機
}

void sendDiscordNotification(const char* message) {
  if (WiFi.status() == WL_CONNECTED) {  // Wi-Fi接続確認
    HTTPClient http;
    http.begin(webhookUrl);  // Discord WebHookのURLを設定
    http.addHeader("Content-Type", "application/json");

    String payload = String("{\"content\":\"") + message + String("\"}");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.printf("Discord通知送信成功: %d\n", httpResponseCode);
    } else {
      Serial.printf("Discord通知送信失敗: %d\n", httpResponseCode);
    }

    http.end();  // 接続を終了
  } else {
    Serial.println("Wi-Fi未接続のため、通知送信できませんでした。");
  }
}
