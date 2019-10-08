#!/usr/bin/python

import chainer
import numpy as np
from PIL import Image
from chainer import Variable
from net import PixEncoder, PixDecoder
from model_utils import crop, transpose, save_image, resize, preprocess_img
from face_features import make_simple_image


def pix_out_image(enc, dec):
    def make_image(pic, dst, name):
        xp = enc.xp
        w_in = 256
        w_out = 256
        in_ch = 12
        out_ch = 3

        pic = xp.asarray(pic)
        x_in = xp.zeros((1, in_ch, w_in, w_in)).astype("f")
        label = np.zeros((12, pic.shape[0], pic.shape[1])).astype("i")
        for j in range(12):
            label[j, :] = pic == 0
        x_in[0, :] = label
        x_in = Variable(x_in)

        z = enc(x_in)
        x_out = dec(z)

        gen_all = xp.zeros((1, out_ch, w_out, w_out)).astype("f")
        gen_all[0, :] = x_out.array[0, :]

        x = xp.asarray(np.clip(gen_all * 128 + 128, 0.0, 255.0), dtype=np.uint8)

        return save_image(x, name, "pix", dst)

    return make_image


def get_generator(enc_model: str, dec_model: str, in_ch: int = 12, out_ch: int = 3):
    enc = PixEncoder(in_ch)
    dec = PixDecoder(out_ch)
    chainer.serializers.load_npz(enc_model, enc)
    chainer.serializers.load_npz(dec_model, dec)

    return pix_out_image(enc, dec)


if __name__ == '__main__':
    filename = 'pix/2.png'
    image = Image.open(filename)
    image = preprocess_img(image, resize_to=256, P=True)
    gen_image = get_generator(enc_model="model/pix_enc.npz", dec_model="model/pix_dec.npz")
    gen_image(image, "pix", "Test3")
