#!/usr/bin/env python

import os
import random

import numpy as np
from PIL import Image

import chainer
import chainer.cuda
from chainer import Variable


def pix_out_image(updater, enc, dec, rows, cols, seed, dst):
    @chainer.training.make_extension()
    def make_image(trainer):
        np.random.seed(seed)
        n_images = rows * cols
        xp = enc.xp

        w_in = 256
        w_out = 256
        in_ch = 12
        out_ch = 3

        in_all = np.zeros((n_images, in_ch, w_in, w_in)).astype("i")
        gt_all = np.zeros((n_images, out_ch, w_out, w_out)).astype("f")
        gen_all = np.zeros((n_images, out_ch, w_out, w_out)).astype("f")

        for it in range(n_images):
            batch = updater.get_iterator('test').next()
            batchsize = len(batch)

            x_in = xp.zeros((batchsize, in_ch, w_in, w_in)).astype("f")
            t_out = xp.zeros((batchsize, out_ch, w_out, w_out)).astype("f")

            for i in range(batchsize):
                x_in[i, :] = xp.asarray(batch[i][0])
                t_out[i, :] = xp.asarray(batch[i][1])
            x_in = Variable(x_in)

            z = enc(x_in)
            x_out = dec(z)

            in_all[it, :] = x_in.data[0, :]
            gt_all[it, :] = t_out[0, :]
            gen_all[it, :] = x_out.data[0, :]

        def save_image(x, name, mode=None):
            _, C, H, W = x.shape
            x = x.reshape((rows, cols, C, H, W))
            x = x.transpose(0, 3, 1, 4, 2)
            if C == 1:
                x = x.reshape((rows * H, cols * W))
            else:
                x = x.reshape((rows * H, cols * W, C))

            preview_dir = '{}/preview'.format(dst)
            preview_path = preview_dir + \
                           '/image_{}_{:0>8}.png'.format(name, trainer.updater.iteration)
            if not os.path.exists(preview_dir):
                os.makedirs(preview_dir)
            Image.fromarray(x, mode=mode).convert('RGB').save(preview_path)

        x = np.asarray(np.clip(gen_all * 128 + 128, 0.0, 255.0), dtype=np.uint8)
        save_image(x, "gen")

        x = np.ones((n_images, 3, w_in, w_in)).astype(np.uint8) * 255
        x[:, 0, :, :] = 0
        for i in range(12):
            x[:, 0, :, :] += np.uint8(15 * i * in_all[:, i, :, :])
        save_image(x, "in", mode='HSV')

        x = np.asarray(np.clip(gt_all * 128 + 128, 0.0, 255.0), dtype=np.uint8)
        save_image(x, "gt")

    return make_image


def star_out_image(updater, gen, dst, att_num=5, image_size=128, rows=4, cols=4, random_att=True):
    @chainer.training.make_extension()
    def make_image(trainer):
        xp = gen.xp
        n_images = rows * cols

        in_all = np.zeros((n_images, 3, image_size, image_size)).astype("f")
        gen_all = np.zeros((n_images, 3, image_size, image_size)).astype("f")

        def save_image(x, name, att, mode=None):
            _, C, H, W = x.shape
            x = x.reshape((rows, cols, C, H, W))
            x = x.transpose(0, 3, 1, 4, 2)
            if C == 1:
                x = x.reshape((rows * H, cols * W))
            else:
                x = x.reshape((rows * H, cols * W, C))

            preview_dir = '{}/preview'.format(dst)
            preview_path = preview_dir + \
                           '/image_{}_{:0>8}_{}.png'.format(name, trainer.updater.iteration, att)
            if not os.path.exists(preview_dir):
                os.makedirs(preview_dir)
            Image.fromarray(x, mode=mode).convert('RGB').save(preview_path)

        batch = updater.get_iterator('test').next()
        batchsize = len(batch)
        x_in = xp.zeros((batchsize, 3, image_size, image_size)).astype("f")
        for i in range(batchsize):
            in_all[i, :] = x_in[i, :] = xp.asarray(batch[i][0])

        x_in = Variable(x_in)
        att = np.array([[int(k) for k in j]
                        for j in [bin(i)[2:].zfill(att_num)
                                  for i in range(2 ** att_num)]]).astype("f")
        if random_att:
            random_select = random.randint(0, len(att))
            att_part = xp.array(np.broadcast_to(att[random_select], (batchsize, att_num)))
            x_out = gen(x_in, att_part)

            for it in range(min(n_images, batchsize)):
                gen_all[it, :] = x_out.array[it, :]
                x = np.asarray(np.clip(gen_all * 128 + 128, 0.0, 255.0), dtype=np.uint8)
                save_image(x, "gen", random_select)
        else:
            for select in range(2 ** att_num):
                att_part = xp.array(np.broadcast_to(att[select], (batchsize, att_num)))
                x_out = gen(x_in, att_part)

                for it in range(min(n_images, batchsize)):
                    gen_all[it, :] = x_out.array[it, :]
                    x = np.asarray(np.clip(gen_all * 128 + 128, 0.0, 255.0), dtype=np.uint8)
                    save_image(x, "gen", select)

        x = np.asarray(np.clip(in_all * 128 + 128, 0.0, 255.0), dtype=np.uint8)
        save_image(x, "gt", "origin")

    return make_image
