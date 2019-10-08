# !usr/bin/python3
# coding: utf-8

import requests
import cv2
import json
import os
import shutil
import numpy as np
from PIL import Image

# from matplotlib import pyplot as plt

API_KEY = 'WRg5cwms3We9MiZV9CHaJZH53kc30VFI'
API_SECRET = 'a1S4bXBNjNZWHFAJRZUclUuYuaIV-aps'
URL = 'https://api-cn.faceplusplus.com/facepp/v3/detect?api_key=%s&api_secret=%s&' \
      'return_attributes=gender,age,eyestatus,emotion,ethnicity,beauty,skinstatus' \
      '&return_landmark=%d' \
      % (API_KEY, API_SECRET, 2)
PATH = "./pics"
DST_PATH = "./face"

LEFT_EYEBROW_UP = ["left_eyebrow_left_corner", "left_eyebrow_upper_left_quarter", "left_eyebrow_upper_middle",
                   "left_eyebrow_upper_right_quarter", "left_eyebrow_upper_right_corner"]
LEFT_EYEBROW_DOWN = ["left_eyebrow_left_corner", "left_eyebrow_lower_left_quarter", "left_eyebrow_lower_middle",
                     "left_eyebrow_lower_right_quarter", "left_eyebrow_lower_right_corner"]
RIGHT_EYEBROW_UP = ["right_eyebrow_right_corner", "right_eyebrow_upper_right_quarter", "right_eyebrow_upper_middle",
                    "right_eyebrow_upper_left_quarter", "right_eyebrow_upper_left_corner"]
RIGHT_EYEBROW_DOWN = ["right_eyebrow_right_corner", "right_eyebrow_lower_right_quarter", "right_eyebrow_lower_middle",
                      "right_eyebrow_lower_left_quarter", "right_eyebrow_lower_left_corner"]
LEFT_EYE = ["left_eye_left_corner", "left_eye_upper_left_quarter", "left_eye_top", "left_eye_upper_right_quarter",
            "left_eye_right_corner", "left_eye_lower_right_quarter", "left_eye_bottom", "left_eye_lower_left_quarter",
            "left_eye_left_corner"]
RIGHT_EYE = ["right_eye_left_corner", "right_eye_upper_left_quarter", "right_eye_top", "right_eye_upper_right_quarter",
             "right_eye_right_corner", "right_eye_lower_right_quarter", "right_eye_bottom",
             "right_eye_lower_left_quarter", "right_eye_left_corner"]
MOUTH_UP_TOP = ["mouth_left_corner", "mouth_upper_lip_left_contour2", "mouth_upper_lip_left_contour1",
                "mouth_upper_lip_top", "mouth_upper_lip_right_contour1", "mouth_upper_lip_right_contour2",
                "mouth_right_corner"]
MOUTH_UP_BOTTOM = ["mouth_left_corner", "mouth_upper_lip_left_contour3", "mouth_upper_lip_left_contour4",
                   "mouth_upper_lip_right_contour4", "mouth_upper_lip_right_contour3",
                   "mouth_right_corner"]
MOUTH_DOWN_TOP = ["mouth_left_corner", "mouth_lower_lip_left_contour1",
                  "mouth_lower_lip_top", "mouth_lower_lip_right_contour1", "mouth_right_corner"]
MOUTH_DOWN_BOTTOM = ["mouth_left_corner", "mouth_lower_lip_left_contour2", "mouth_lower_lip_left_contour3",
                     "mouth_lower_lip_bottom", "mouth_lower_lip_right_contour3", "mouth_lower_lip_right_contour2",
                     "mouth_right_corner"]


def draw_curve(points: list, img: np.ndarray, list_size: int = 0, step: float = 0.005, scale: float = 0.6):
    list_size = len(points) if list_size == 0 else list_size
    origin_points = points
    mid_points = [None] * list_size
    for i in range(list_size):
        if i == list_size - 1:
            next_i = i
        else:
            next_i = (i + 1) % list_size
        mid_point = {'x': (origin_points[i]['x'] + origin_points[next_i]['x']) / 2,
                     'y': (origin_points[i]['y'] + origin_points[next_i]['y']) / 2}
        mid_points[i] = mid_point

    extra_points = [None] * (list_size * 2)
    for i in range(list_size):
        if i == list_size - 1:
            next_i = i
        else:
            next_i = (i + 1) % list_size
        if i == 0:
            back_i = i
        else:
            back_i = (i + list_size - 1) % list_size
        mid_point = {'x': (mid_points[i]['x'] + mid_points[back_i]['x']) / 2,
                     'y': (mid_points[i]['y'] + mid_points[back_i]['y']) / 2}
        offset = {'x': origin_points[i]['x'] - mid_point['x'],
                  'y': origin_points[i]['y'] - mid_point['y']}

        extra_index = i * 2
        extra_points[extra_index] = {'y': mid_points[back_i]['y'] + offset['y'],
                                     'x': mid_points[back_i]['x'] + offset['x']}

        add = {'y': (extra_points[extra_index]['y'] - origin_points[i]['y']) * scale,
               'x': (extra_points[extra_index]['x'] - origin_points[i]['x']) * scale}

        extra_points[extra_index] = {'y': origin_points[i]['y'] + add['y'],
                                     'x': origin_points[i]['x'] + add['x']}

        extra_next_i = (extra_index + 1) % (2 * list_size)

        extra_points[extra_next_i] = {'y': mid_points[i]['y'] + offset['y'],
                                      'x': mid_points[i]['x'] + offset['x']}

        add = {'y': (extra_points[extra_next_i]['y'] - origin_points[i]['y']) * scale,
               'x': (extra_points[extra_next_i]['x'] - origin_points[i]['x']) * scale}

        extra_points[extra_next_i] = {'y': origin_points[i]['y'] + add['y'],
                                      'x': origin_points[i]['x'] + add['x']}

    control_points = [None] * 4
    for i in range(list_size - 1):
        img[points[i]['y']][points[i]['x']] = 0
        extra_index = 2 * i
        extra_next_i = (extra_index + 2) % (2 * list_size)
        next_i = (i + 1) % list_size
        control_points[0] = origin_points[i]
        control_points[1] = extra_points[extra_index + 1]
        control_points[2] = extra_points[extra_next_i]
        control_points[3] = origin_points[next_i]
        u = 1.0
        while u > 0:
            point = bezier3func(u, control_points)
            img[point['y']][point['x']] = 0
            u -= step


# 三次贝塞尔曲线
def bezier3func(uu: float, control_points: list) -> dict:
    part0 = {'x': control_points[0]['x'] * uu * uu * uu, 'y': control_points[0]['y'] * uu * uu * uu}
    part1 = {'x': 3 * control_points[1]['x'] * uu * uu * (1 - uu), 'y': 3 * control_points[1]['y'] * uu * uu * (1 - uu)}
    part2 = {'x': 3 * control_points[2]['x'] * uu * (1 - uu) * (1 - uu),
             'y': 3 * control_points[2]['y'] * uu * (1 - uu) * (1 - uu)}
    part3 = {'x': control_points[3]['x'] * (1 - uu) * (1 - uu) * (1 - uu),
             'y': control_points[3]['y'] * (1 - uu) * (1 - uu) * (1 - uu)}

    return {'x': int(part0['x'] + part1['x'] + part2['x'] + part3['x']),
            'y': int(part0['y'] + part1['y'] + part2['y'] + part3['y'])}


def get_features(img_path: str) -> json:
    files = {'image_file': open(img_path, 'rb')}
    r = requests.post(url=URL, files=files).json().get('faces')
    if r is None:
        r = requests.post(url=URL, files=files).json().get('faces')
        print(img_path + ' has error')
    return r


def gen_src(img: np.ndarray, json_src: json) -> np.ndarray:
    for i, val in enumerate(json_src):
        left_eyebrow_up = [val["landmark"][key] for key in LEFT_EYEBROW_UP]
        left_eyebrow_down = [val["landmark"][key] for key in LEFT_EYEBROW_DOWN]
        right_eyebrow_up = [val["landmark"][key] for key in RIGHT_EYEBROW_UP]
        right_eyebrow_down = [val["landmark"][key] for key in RIGHT_EYEBROW_DOWN]

        contour = [val["landmark"]["contour_left{}".format(count)] for count in range(1, 17)]
        contour.append(val["landmark"]["contour_chin"])
        contour += (val["landmark"]["contour_right{}".format(count)] for count in range(16, 0, -1))

        nose_bridge = [val["landmark"]["nose_bridge{}".format(count)] for count in range(1, 4)]
        nose_bridge.append(val["landmark"]["nose_tip"])

        nose = [val["landmark"]["nose_left_contour{}".format(count)] for count in range(1, 6)]
        nose.append(val["landmark"]["nose_middle_contour"])
        nose += (val["landmark"]["nose_right_contour{}".format(count)] for count in range(5, 0, -1))

        left_eye = [val["landmark"][key] for key in LEFT_EYE]
        right_eye = [val["landmark"][key] for key in RIGHT_EYE]
        mouth_up_top = [val["landmark"][key] for key in MOUTH_UP_TOP]
        mouth_up_bottom = [val["landmark"][key] for key in MOUTH_UP_BOTTOM]
        mouth_down_top = [val["landmark"][key] for key in MOUTH_DOWN_TOP]
        mouth_down_bottom = [val["landmark"][key] for key in MOUTH_DOWN_BOTTOM]

        draw_curve(left_eyebrow_up, img)
        draw_curve(left_eyebrow_down, img)
        draw_curve(right_eyebrow_up, img)
        draw_curve(right_eyebrow_down, img)
        draw_curve(left_eye, img)
        draw_curve(right_eye, img)

        draw_curve(contour, img)
        draw_curve(nose_bridge, img)
        draw_curve(nose, img)

        draw_curve(mouth_up_top, img)
        draw_curve(mouth_up_bottom, img)
        draw_curve(mouth_down_top, img)
        draw_curve(mouth_down_bottom, img)
    return img


def save_features():
    count = 0
    files = os.listdir(path=PATH)
    for file in files:
        shutil.copyfile(PATH + '/' + file, DST_PATH + "/cmp_b%04d.jpg" % count)
        faces = get_features(DST_PATH + "/cmp_b%04d.jpg" % count)
        with open(DST_PATH + "/cmp_b%04d.json" % count, 'w') as json_file:
            json.dump(faces, json_file)
        count += 1
        if (count + 1) % 10 == 0:
            print("已经处理了%04d张图片" % (count + 1))


def make_features():
    for count in range(0):
        with open(DST_PATH + "/cmp_b%04d.json" % count, 'r')  as f:
            json_file = json.load(f)
        target = cv2.imread(DST_PATH + "/cmp_b%04d.jpg" % count)
        img = gen_src(target, json_file)
        cv2.imwrite(DST_PATH + "/cmp_b%04d1.png" % count, img)

        pic = Image.open("face/cmp_b%04d1.png" % count)
        pic_P = pic.convert("P")
        pic_P.save("./face/cmp_b%04d1.png" % count, "PNG")
        if (count + 1) % 10 == 0:
            print("已经处理了%04d张图片" % (count + 1))


def convert(img: np.ndarray, threshold=70, ratio=3, kernel_size=3) -> np.ndarray:
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    detected_edges = cv2.GaussianBlur(gray, (3, 3), 0)
    detected_edges = cv2.Canny(detected_edges, threshold, threshold * ratio, apertureSize=kernel_size)
    dst = cv2.bitwise_and(img, img, mask=detected_edges)
    dst = 255 - dst
    dst[dst != 255] = 0
    return dst


# 使用图片生成简笔画
# image_path 是用来生成简笔画的原图片路径
# save_path 是用来保存简笔画的路径
# copy_path 把原来的图片重新保存到的路径
# json_file 如果json已经有了，可以传到这里来  json_file = json.load(json_path) 读取了之后再传入
def make_simple_image(image_path: str, save_path: str, copy_path: str = None, json_file=None):
    img = cv2.imread(image_path)
    if copy_path is not None:
        cv2.imwrite(copy_path, img)
    img = convert(img)  ## 头发版
    if json_file is None:
        json_file = get_features(image_path)
    img = gen_src(img, json_file)
    res = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    cv2.imwrite(save_path, res)
    return res


if __name__ == '__main__':
    filename = "pix/image_Test2_pix.png"
    # make_simple_image(image_path='photos/1.png', save_path="pix/1.png")
    json_file = get_features(filename)
    print(json_file)
    make_simple_image(filename, "pix/2.png")

    pic = Image.open("pix/2.png")
    pic_P = pic.convert("P")
    pic_P.save("pix/2.png", "PNG")
