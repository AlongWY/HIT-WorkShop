# !/usr/bin/python3
# coding: utf-8

import json
import requests
from threading import Thread
from bs4 import BeautifulSoup
from tkinter import messagebox, Label, Button, Entry, X, LEFT, Tk, Frame

USER_AGENT = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) ' \
             'AppleWebKit/537.36 (KHTML, like Gecko) ' \
             'Chrome/51.0.2704.103 Safari/537.36'
ACCEPT_LANGUAGE = "zh-CN,zh-TW;q=0.9,zh;q=0.8,en-US;q=0.7,en;q=0.6"
ACCEPT_ENCODING = "gzip, deflate"
REFER = 'http://today.hit.edu.cn/'
ACCEPT = 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8'

headers = {
    'Connection': 'keep-alive',
    'Cache-Control': 'max-age=0',
    'Upgrade-Insecure-Requests': '1',
    'DNT': '1',
    'User-Agent': USER_AGENT,
    'Accept': ACCEPT,
    'Referer': REFER,
    'Accept-Language': ACCEPT_LANGUAGE,
    'Accept-Encoding': ACCEPT_ENCODING
}

def support(URL: str, times: int = 10):
    s = requests.session()
    r = s.get(URL, headers=headers, timeout=1)
    r.encoding = 'utf-8'
    soup = BeautifulSoup(r.text, 'lxml')
    support = soup.select('section.block.block-hit-article.block-article-flag.clearfix')

    script = json.loads(soup.find('script', {"data-drupal-selector": 'drupal-settings-json'}).string)
    callback = script["boostPlaceholderIds"]

    for callable in callback.keys():
        if "callback=flag.link_builder" in callable:
            r = s.get(f'http://today.hit.edu.cn/boost/replace?{callable}', headers=headers, timeout=1)

    soup = BeautifulSoup(r.text, 'lxml')
    real_need = json.loads(soup.find(name='textarea').string)

    soup = BeautifulSoup(real_need[0]['data'], 'lxml')
    real_link_suffix = soup.find(name='a', href=True)['href']
    real_link = f"http://today.hit.edu.cn{real_link_suffix}"

    for i in range(times):
        s.cookies.clear()
        r = s.get(real_link, headers=headers, timeout=1)
    messagebox.showinfo('OK', '点赞完毕')


class Application(Tk):
    def __init__(self, master=None):
        Tk.__init__(self, master)
        self.createWidgets()
        import os
        if os.name == "nt":
            from ctypes import windll, pointer, wintypes
            try:
                windll.shcore.SetProcessDpiAwareness(1)
            except Exception:
                pass  # this will fail on Windows Server and maybe early Windows

    def createWidgets(self):
        self.urlFrame = Frame(self)
        self.urlFrame.pack(fill=X)
        self.urlLable = Label(self.urlFrame, text="URL", width=6)
        self.urlLable.pack(side=LEFT, padx=5, pady=5)
        self.urlInput = Entry(self.urlFrame)
        self.urlInput.pack(fill="both",expand=True, padx=5)

        self.timesFrame = Frame(self)
        self.timesFrame.pack(fill=X)
        self.timesLabel = Label(self.timesFrame, text="次数", width=6)
        self.timesLabel.pack(side=LEFT, padx=5, pady=5)
        self.timesInput = Entry(self.timesFrame)
        self.timesInput.pack(fill="both",expand=True, padx=5)

        self.alertButton = Button(self, text='开始点赞', command=self.run)
        self.alertButton.pack()
    
    def create_thread(self, URL: str, times: int):
        self.run_thread = Thread(target=support, args = [URL, times])
        self.run_thread.setDaemon(True)
        self.run_thread.start()

    def run(self):
        URL = self.urlInput.get()
        times = int(self.timesInput.get())
        self.create_thread(URL=URL, times=times)

if __name__ == '__main__':
    app = Application()
    app.mainloop()
