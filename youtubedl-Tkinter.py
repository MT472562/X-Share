import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import yt_dlp as youtube_dl
import os

class YouTubeDownloader(tk.Tk):
    def __init__(self):
        super().__init__()
        
        self.title("YouTube Downloader")
        self.geometry("500x350")
        self.resizable(False, False)
        
        self.create_widgets()
    
    def create_widgets(self):
        # URL Entry
        self.url_label = ttk.Label(self, text="YouTube URL:")
        self.url_label.pack(pady=5)
        self.url_entry = ttk.Entry(self, width=50)
        self.url_entry.pack(pady=5)
        
        # Download Directory Button
        self.dir_button = ttk.Button(self, text="Choose Download Directory", command=self.choose_directory)
        self.dir_button.pack(pady=5)
        self.dir_label = ttk.Label(self, text="")
        self.dir_label.pack(pady=5)
        
        # Playlist Option
        self.playlist_var = tk.BooleanVar()
        self.playlist_check = ttk.Checkbutton(self, text="Download Playlist", variable=self.playlist_var)
        self.playlist_check.pack(pady=5)
        
        # Audio Only Option
        self.audio_var = tk.BooleanVar()
        self.audio_check = ttk.Checkbutton(self, text="Download Audio Only", variable=self.audio_var)
        self.audio_check.pack(pady=5)
        
        # Max Filesize Option
        self.maxsize_label = ttk.Label(self, text="Max Filesize (bytes):")
        self.maxsize_label.pack(pady=5)
        self.maxsize_entry = ttk.Entry(self, width=20)
        self.maxsize_entry.pack(pady=5)
        
        # Download Button
        self.download_button = ttk.Button(self, text="Download", command=self.download)
        self.download_button.pack(pady=20)
    
    def choose_directory(self):
        path = filedialog.askdirectory(title="Choose Download Directory")
        if path:
            self.dir_label.config(text=path)
    
    def download(self):
        url = self.url_entry.get()
        save_path = self.dir_label.cget("text")
        
        if not url or not save_path:
            messagebox.showerror("Error", "Please provide both URL and Download Directory.")
            return
        
        options = {
            'verbose': True,
            'outtmpl': os.path.join(save_path, '%(title)s.%(ext)s'),
        }
        
        if not self.playlist_var.get():
            options['noplaylist'] = True
        
        if self.audio_var.get():
            options['format'] = 'bestaudio/best'
            options['postprocessors'] = [{
                'key': 'FFmpegExtractAudio',
                'preferredcodec': 'mp3',
                'preferredquality': '192',
            }]
        else:
            options['format'] = 'bestvideo+bestaudio/best'
        
        max_size = self.maxsize_entry.get()
        if max_size:
            try:
                options['max_filesize'] = int(max_size)
            except ValueError:
                messagebox.showwarning("Warning", "Invalid filesize. Ignoring filesize limit.")
        else:
            messagebox.showinfo("No Filesize Limit", "No filesize limit was set.")
        
        try:
            with youtube_dl.YoutubeDL(options) as ydl:
                ydl.download([url])
                messagebox.showinfo("Success", "Download completed successfully.")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {str(e)}")

if __name__ == "__main__":
    app = YouTubeDownloader()
    app.mainloop()
