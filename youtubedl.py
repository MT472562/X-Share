# import yt_dlp as youtube_dl
# print("URLを入力してください　LISTが入っているとリストごとダウンロードされます")
# uri=input()
# ydl_opts = {
#     'verbose': True,
#     'noplaylist': True  # リストをダウンロードしないようにするオプション
# }

# with youtube_dl.YoutubeDL(ydl_opts) as ydl:
#     ydl.download([uri])

import yt_dlp as youtube_dl
import os
print("保存するpathを入力してください")
path=input()
# 保存先のパスを指定
save_path = fr'{path}\%(title)s.%(ext)s'
print("以下のパスにダウンロードします。")
print(path)

print("Youtubeのダウンロードを行います。")

# ユーザーにURLを入力させる
print("URLを入力してください:")
uri = input()


# オプション設定
ydl_opts = {
    'verbose': True,
    'outtmpl': save_path,  # 保存場所を指定
}

# プレイリストをダウンロードしない
noplaylist = input("プレイリストをダウンロードしますか？ (y/n): ").strip().lower()
if noplaylist == 'n':
    ydl_opts['noplaylist'] = True
    
# ダウンロードの種類指定
audio_only = input("音声だけダウンロードしますか？ (y/n): ").strip().lower()
if audio_only == 'y':
    ydl_opts['format'] = 'bestaudio/best'


# ファイルサイズ制限を設定する
max_filesize = input("ファイルサイズ制限を設定しますか？ (y/n): ").strip().lower()
if max_filesize == 'y':
    size = input("最大ファイルサイズ (バイト単位) を入力してください: ")
    try:
        ydl_opts['max_filesize'] = int(size)
    except ValueError:
        print("無効なサイズです。ファイルサイズ制限は設定されません。")

# ダウンロードの実行
with youtube_dl.YoutubeDL(ydl_opts) as ydl:
    ydl.download([uri])
