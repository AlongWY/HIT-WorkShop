#!usr/bin/env python
# coding: utf-8


import cv2
import curses
import numpy as np

ASCII_CHAR = list("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\"^`'. ")
FILEPATH = 'data/2.flv'

unit = (256.0 + 1) / len(ASCII_CHAR)

stdscr = curses.initscr()


def set_win():
    global stdscr
    curses.start_color()
    curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_WHITE)
    curses.noecho()
    curses.cbreak()
    stdscr.nodelay(1)


def unset_win():
    global stdscr
    curses.nocbreak()
    curses.echo()
    curses.endwin()


def convert_char(frame: np.ndarray):
    frame_b, frame_g, frame_r = cv2.split(frame)
    gray = (0.2126 * frame_r + 0.7152 * frame_g + 0.0722 * frame_b) / unit
    return gray.astype(int)


def main():
    global stdscr
    cap = cv2.VideoCapture(FILEPATH)

    while (cap.isOpened()):
        c = stdscr.getch()
        if c == ord('q'):
            break

        ret, frame = cap.read()
        if frame is None:
            break
        height, weight = stdscr.getmaxyx()
        height, weight = height, weight - 1
        frame = cv2.resize(frame, (weight, height))
        frame = convert_char(frame)

        for i in range(height):
            for j in range(weight):
                stdscr.addstr(i, j, ASCII_CHAR[frame[i][j]], curses.color_pair(1))
        stdscr.refresh()

    cap.release()


if __name__ == '__main__':
    try:
        set_win()
        main()
    except Exception:
        raise Exception
    finally:
        unset_win()
