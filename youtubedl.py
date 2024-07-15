import yt_dlp as youtube_dl
print("URLを入力してください")
uri=input()
ydl_opts = {
    'verbose': True,
    'noplaylist': True
}

with youtube_dl.YoutubeDL(ydl_opts) as ydl:
    ydl.download([uri])
