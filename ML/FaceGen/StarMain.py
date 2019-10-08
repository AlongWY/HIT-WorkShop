#!/usr/bin/python

import chainer
import numpy as np
from PIL import Image
from chainer import Variable
from net import StarGenerator
from model_utils import crop, transpose, save_image, resize, preprocess_img


def make_gen_fun(gen, att_num=20, image_size=128):
    def make_image(pic, att, dst, name):
        xp = gen.xp

        x_in = xp.zeros((1, 3, image_size, image_size)).astype("f")
        x_in[0, :] = xp.asarray(pic)
        att_in = xp.zeros((1, att_num)).astype("f")
        att_in[0, :] = xp.asarray(att)
        x_in = Variable(x_in)
        gen_out = gen(x_in, att_in).data

        x = np.asarray(np.clip(gen_out * 128 + 128, 0.0, 255.0), dtype=np.uint8)
        return save_image(x, name=name, label=att, dst=dst)

    return make_image


def get_generator(model_path: str, att_num: int = 20, image_size: int = 128):
    gen = StarGenerator(64, att_num, 6)
    chainer.serializers.load_npz(model_path, gen)
    return make_gen_fun(gen, att_num, image_size)


if __name__ == '__main__':
    filename = 'photos/1.png'
    gen_image = get_generator(model_path="model/star_gen.npz", image_size=256)
    image = Image.open(filename)
    image = preprocess_img(image, crop_to=256, resize_to=256)

    # "5_o_Clock_Shadow" "Attractive" "Bags_Under_Eyes" "Bangs" "Big_Lips"
    # "Big_Nose" "Black_Hair" "Bushy_Eyebrows" "Chubby" "Double_Chin"
    # "Eyeglasses" "High_Cheekbones" "Male" "Mouth_Slightly_Open" "Narrow_Eyes"
    # "Pale_Skin" "Pointy_Nose" "Receding_Hairline" "Straight_Hair" "Young"
    for i in range(5):
        image = gen_image(image, [0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0,
                                  1, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0], "Test", name=i)
        image = transpose(image)
