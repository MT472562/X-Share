# pip install line-bot-sdk
# pip install selenium
# pip install chromedriver-binary==バージョン番号
# chromedriverのダウンロードでエラーが起きたら、表示されている一番近いバージョン番号を入力します
import time
from linebot import LineBotApi
from linebot.exceptions import LineBotApiError
from linebot.models import TextSendMessage
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

driver = webdriver.Chrome()
token = "トークン（長期）"
urerid = "ユーザーID"
url = "https://store.evering.jp/item/EVERING_EXPO.html"


def send_message(text):
    line_bot_api = LineBotApi(token)

    try:
        line_bot_api.push_message(urerid, TextSendMessage(text=text))
    except LineBotApiError as e:
        print(e.message)


try:
    driver.get(url)
    time.sleep(5)
    color_silver = WebDriverWait(driver, 30).until(
        EC.element_to_be_clickable(
            (By.XPATH, "/html/body/div[1]/main/div/div[2]/div/div[2]/div/form/div[2]/div[1]/div[1]/select/option[4]"))
    )
    color_silver.click()
    color_silver_text = color_silver.text
    time.sleep(1)
    color_silver_size_list = WebDriverWait(driver, 30).until(
        EC.element_to_be_clickable(
            (By.XPATH, "/html/body/div[1]/main/div/div[2]/div/div[2]/div/form/div[2]/div[1]/div[2]/select"))
    )
    color_silver_size = color_silver_size_list.text
    driver.quit()
except:
    send_message("処理でエラーが発生しました")
if color_silver_text != "SILVER (✕)":
    text = f"エブリングのSILVERカラーの在庫に変動がありました。\n以下が最新の情報です。\n\n{color_silver_size}\n\n早期に購入してください\n{url}"
    send_message(text)
else:
    #在庫に変動がない場合の処理を任意で追加してください
    pass

