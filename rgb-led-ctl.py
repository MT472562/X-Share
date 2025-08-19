import subprocess
import asyncio
import os
from meross_iot.http_api import MerossHttpClient
from meross_iot.manager import MerossManager
import json

# コマンド（PowerShell 用に & をエスケープ）
on_command = '& "OpenRGB.exeのフルパス" --profile "点灯するプロファイル（フルパス）" --noautoconnect'
off_command = '& "OpenRGB.exeのフルパス" --profile "消灯するプロファイル（フルパス）" --noautoconnect'


# 前回の状態を読み込む
last_status_file = "前回の状態を保存するためのJSONファイルパス（フルパス）"
try:
    last_status = json.load(open(last_status_file)).get("status", "OFF")
except FileNotFoundError:
    last_status = "OFF"

EMAIL = os.environ.get('MEROSS_EMAIL') or "Mail"
PASSWORD = os.environ.get('MEROSS_PASSWORD') or "Password"

async def main():
    http_api_client = await MerossHttpClient.async_from_user_password(
        email=EMAIL, password=PASSWORD, api_base_url="https://iot.meross.com"
    )

    manager = MerossManager(http_client=http_api_client)
    await manager.async_init()
    await manager.async_device_discovery()

    status_dict = {}
    try:
        devices = manager.find_devices()
        for dev in devices:
            await dev.async_update()
            status_dict[dev.name] = "ON" if dev.is_on() else "OFF"

        # デバイスの状態から現在の合計状態を判断
        current_status = "ON" if "ON" in status_dict.values() else "OFF"

        # 前回と変化があればコマンドを実行
        if current_status != last_status:
            command_to_run = on_command if current_status == "ON" else off_command
            subprocess.run(["powershell", "-Command", command_to_run], check=True)
            print(f"Executed {command_to_run}")

            # 状態を保存
            with open(last_status_file, "w") as f:
                json.dump({"status": current_status}, f)
    finally:
        manager.close()
        await http_api_client.async_logout()

asyncio.run(main())
