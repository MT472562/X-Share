import yt_dlp as youtube_dl
print("URLを入力してください　LISTが入っているとリストごとダウンロードされます")
uri=input()
ydl_opts = {
    'verbose': True,
    'noplaylist': True  # リストをダウンロードしないようにするオプション
}

with youtube_dl.YoutubeDL(ydl_opts) as ydl:
    ydl.download([uri])