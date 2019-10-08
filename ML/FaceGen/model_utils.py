import os
import numpy as np
from PIL import Image


def crop(img: Image.Image, crop: int = 178):
    width, height = img.size
    left = (width - crop) / 2
    top = (height - crop) / 2
    right = (width + crop) / 2
    bottom = (height + crop) / 2

    img = img.crop((left, top, right, bottom))
    return img


def resize(img: Image.Image, size: int = 128):
    w, h = img.size
    img = img.resize((size, size), Image.BILINEAR)
    return img


def transpose(img: Image.Image):
    img = np.asarray(img)
    img = img.transpose((2, 0, 1))  # (H,W,ch) -> (ch,H,W)
    img = img / 128.0 - 1.0
    return img


# PIL 读取图片，生成的给生成器 PIX STAR
def preprocess_img(img: Image.Image, resize_to: int = 128, crop_to: int = None, P: bool = False) -> np.ndarray:
    if crop_to is not None:
        img = crop(img, crop=crop_to)
    img = resize(img, resize_to)
    res = img.convert("P") if P else transpose(img)
    return res


def save_image(x, name, label, dst, mode=None):
    _, C, H, W = x.shape
    x = x.reshape(C, H, W)
    x = x.transpose(1, 2, 0)
    if C == 1:
        x = x.reshape((1 * H, 1 * W))
    else:
        x = x.reshape((1 * H, 1 * W, C))

    preview_path = dst + '/image_{}_{}.png'.format(name, label)
    if not os.path.exists(dst):
        os.makedirs(dst)
    img = Image.fromarray(x, mode=mode).convert('RGB')
    img.save(preview_path)
    return img
