#!/usr/bin/env python

from __future__ import print_function

import math
import chainer
from chainer import cuda
import numpy as np
import chainer.functions as F
import chainer.links as L
from instance_normalization import InstanceNormalization
from chainer import Variable


# U-net https://arxiv.org/pdf/1611.07004v1.pdf

# convolution-batchnormalization-(dropout)-relu
class PixCBR(chainer.Chain):
    def __init__(self, ch0, ch1, bn=True, sample='down', activation=F.relu, dropout=False):
        layers = {}
        self.bn = bn
        self.dropout = dropout
        self.activation = activation
        w = chainer.initializers.Normal(0.02)
        if sample == 'down':
            layers['c'] = L.Convolution2D(ch0, ch1, 4, 2, 1, initialW=w)
        else:
            layers['c'] = L.Deconvolution2D(ch0, ch1, 4, 2, 1, initialW=w)
        if bn:
            layers['batchnorm'] = L.BatchNormalization(ch1)
        super(PixCBR, self).__init__(**layers)

    def __call__(self, x):
        h = self.c(x)
        if self.bn:
            h = self.batchnorm(h)
        if self.dropout:
            h = F.dropout(h)
        if not self.activation is None:
            h = self.activation(h)
        return h


class PixEncoder(chainer.Chain):
    def __init__(self, in_ch):
        layers = {}
        w = chainer.initializers.Normal(0.02)
        layers['c0'] = L.Convolution2D(in_ch, 64, 3, 1, 1, initialW=w)
        layers['c1'] = PixCBR(64, 128, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c2'] = PixCBR(128, 256, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c3'] = PixCBR(256, 512, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c4'] = PixCBR(512, 512, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c5'] = PixCBR(512, 512, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c6'] = PixCBR(512, 512, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c7'] = PixCBR(512, 512, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        super(PixEncoder, self).__init__(**layers)

    def __call__(self, x):
        hs = [F.leaky_relu(self.c0(x))]
        for i in range(1, 8):
            hs.append(self['c%d' % i](hs[i - 1]))
        return hs


class PixDecoder(chainer.Chain):
    def __init__(self, out_ch):
        layers = {}
        w = chainer.initializers.Normal(0.02)
        layers['c0'] = PixCBR(512, 512, bn=True, sample='up', activation=F.relu, dropout=True)
        layers['c1'] = PixCBR(1024, 512, bn=True, sample='up', activation=F.relu, dropout=True)
        layers['c2'] = PixCBR(1024, 512, bn=True, sample='up', activation=F.relu, dropout=True)
        layers['c3'] = PixCBR(1024, 512, bn=True, sample='up', activation=F.relu, dropout=False)
        layers['c4'] = PixCBR(1024, 256, bn=True, sample='up', activation=F.relu, dropout=False)
        layers['c5'] = PixCBR(512, 128, bn=True, sample='up', activation=F.relu, dropout=False)
        layers['c6'] = PixCBR(256, 64, bn=True, sample='up', activation=F.relu, dropout=False)
        layers['c7'] = L.Convolution2D(128, out_ch, 3, 1, 1, initialW=w)
        super(PixDecoder, self).__init__(**layers)

    def __call__(self, hs):
        h = self.c0(hs[-1])
        for i in range(1, 8):
            h = F.concat([h, hs[-i - 1]])
            if i < 7:
                h = self['c%d' % i](h)
            else:
                h = self.c7(h)
        return h


class PixDiscriminator(chainer.Chain):
    def __init__(self, in_ch, out_ch):
        layers = {}
        w = chainer.initializers.Normal(0.02)
        layers['c0_0'] = PixCBR(in_ch, 32, bn=False, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c0_1'] = PixCBR(out_ch, 32, bn=False, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c1'] = PixCBR(64, 128, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c2'] = PixCBR(128, 256, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c3'] = PixCBR(256, 512, bn=True, sample='down', activation=F.leaky_relu, dropout=False)
        layers['c4'] = L.Convolution2D(512, 1, 3, 1, 1, initialW=w)
        super(PixDiscriminator, self).__init__(**layers)

    def __call__(self, x_0, x_1):
        h = F.concat([self.c0_0(x_0), self.c0_1(x_1)])
        h = self.c1(h)
        h = self.c2(h)
        h = self.c3(h)
        h = self.c4(h)
        # h = F.average_pooling_2d(h, h.data.shape[2], 1, 0)
        return h


# CONV-(N256,K3x3,S1,P1) IN ReLU
class StarResBlock(chainer.Chain):
    def __init__(self, in_ch, out_ch, activation=F.relu, slope=None, noise=False):
        layers = {}
        self.slope = slope
        self.activation = activation
        # w = chainer.initializers.Uniform(scale=math.sqrt(1 / ch / 3 / 3))
        layers['c0'] = L.Convolution2D(in_ch, out_ch, ksize=3, stride=1, pad=1, nobias=True)
        layers['c1'] = InstanceNormalization(out_ch, use_gamma=noise, use_beta=noise)
        layers['c2'] = L.Convolution2D(in_ch, out_ch, ksize=3, stride=1, pad=1, nobias=True)
        layers['c3'] = InstanceNormalization(out_ch, use_gamma=noise, use_beta=noise)
        super(StarResBlock, self).__init__(**layers)

    def __call__(self, x):
        h = self.c0(x)
        h = self.c1(h)
        if self.slope == None:
            h = self.activation(h)
        else:
            h = self.activation(h, self.slope)
        h = self.c2(h)
        h = self.c3(h)
        return x + h


class StarCBR(chainer.Chain):
    def __init__(self, ch0, ch1, conv_config, sample='down', ins=True, activation=F.relu, slope=None, dropout=False,
                 nobias=True, noise=False):
        layers = {}
        self.ins = ins
        self.slope = slope
        self.sample = sample
        self.dropout = dropout
        self.activation = activation
        w = chainer.initializers.Normal(0.02)
        k, s, p = conv_config
        if sample == 'down':
            layers['c1'] = L.Convolution2D(ch0, ch1, ksize=k, stride=s, pad=p, nobias=nobias)
        else:
            layers['c1'] = L.Deconvolution2D(ch0, ch1, ksize=k, stride=s, pad=p, nobias=nobias)
        if self.ins:
            layers['c2'] = InstanceNormalization(ch1, use_gamma=noise, use_beta=noise)

        super(StarCBR, self).__init__(**layers)

    def __call__(self, x):
        h = self.c1(x)
        if self.ins:
            h = self.c2(h)
        h = F.dropout(h) if self.dropout else h
        if self.slope == None:
            h = self.activation(h)
        else:
            h = self.activation(h, self.slope)
        return h


class StarGenerator(chainer.Chain):
    def __init__(self, conv_dim=64, c_dim=5, repeat_num=6):
        layers = {}
        layers['c0'] = StarCBR(3 + c_dim, conv_dim, (7, 1, 3),
                               sample='down', ins=True, nobias=True)

        # Down-sampling layers
        curr_dim = conv_dim
        for i in range(1, 3):
            layers['c%d' % i] = StarCBR(curr_dim, curr_dim * 2, (4, 2, 1),
                                        sample='down', ins=True, nobias=True)
            curr_dim = curr_dim * 2

        for i in range(3, 3 + repeat_num):
            layers['c%d' % i] = StarResBlock(curr_dim, curr_dim, noise=True, activation=F.relu)

        # Up-sampling layers
        for i in range(3 + repeat_num, 5 + repeat_num):
            layers['c%d' % i] = StarCBR(curr_dim, curr_dim // 2, (4, 2, 1), sample='up',
                                        ins=True, nobias=True)
            curr_dim = curr_dim // 2

        layers['c%d' % (5 + repeat_num)] = StarCBR(curr_dim, 3, (7, 1, 3), sample='down',
                                                   activation=F.tanh, ins=False, nobias=True)
        self.c_num = 6 + repeat_num
        super(StarGenerator, self).__init__(**layers)

    def __call__(self, x: np.ndarray, c: np.ndarray):
        B, ch, W, H = x.shape
        B, A = c.shape
        c = self.xp.broadcast_to(c, (W, H, B, A))
        c = self.xp.transpose(c, (2, 3, 0, 1))
        h = F.concat((x, c), axis=1)
        for i in range(self.c_num):
            h = self['c%d' % i](h)
        return h


class StarDiscriminator(chainer.Chain):
    def __init__(self, image_size=128, conv_dim=64, c_dim=5, repeat_num=6):
        layers = {}
        layers['c0'] = StarCBR(3, conv_dim, (4, 2, 1), sample='down', activation=F.leaky_relu,
                               slope=0.01, ins=False, nobias=False)

        curr_dim = conv_dim
        for i in range(1, repeat_num):
            layers['c%d' % i] = StarCBR(curr_dim, curr_dim * 2, (4, 2, 1), activation=F.leaky_relu,
                                        slope=0.01, ins=False, nobias=True)
            curr_dim = curr_dim * 2

        kernel_size = int(image_size / np.power(2, repeat_num))
        self.conv_1 = L.Convolution2D(curr_dim, 1, ksize=3, stride=1, pad=1, nobias=False)
        self.conv_2 = L.Convolution2D(curr_dim, c_dim, ksize=kernel_size, nobias=False)

        self.layer_num = repeat_num
        super(StarDiscriminator, self).__init__(**layers)

    def __call__(self, x):
        h = x
        for i in range(self.layer_num):
            h = self['c%d' % i](h)
        out_src = self.conv_1(h)
        out_cls = self.conv_2(h)
        c, d, _, _ = out_cls.shape
        return out_src, F.reshape(out_cls, (c, d))
