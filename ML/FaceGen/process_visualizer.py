# !usr/bin/python3
# coding: utf-8

import matplotlib.pyplot as plt
import json

lambda_cls = 1
lambda_rec = 10
lambda_gp = 10


def main():
    with open('model/log', 'r')  as f:
        log = json.load(f)

    iter = []
    d_loss_cls = []
    d_loss_gp = []
    d_loss_adv = []

    g_loss_adv = []
    g_loss_cls = []
    g_loss_rec = []

    for data in log:
        if data['iteration'] % 5 == 1:
            continue
        iter.append(data['iteration'])

        d_loss_cls.append(data['D/loss_cls'])
        d_loss_gp.append(data['D/loss_gp'])
        d_loss_adv.append(- data['D/loss_real'] - data['D/loss_fake'] - lambda_gp * data['D/loss_gp'])

        g_loss_adv.append(data['G/loss_fake'])
        g_loss_cls.append(data['G/loss_cls'])
        g_loss_rec.append(data['G/loss_rec'])

    plt.plot(iter, [0] * len(iter))
    plt.plot(iter, d_loss_adv, label='D/loss_adv', color='black')
    plt.plot(iter, g_loss_adv, label='G/loss_adv', color='green')
    plt.ylim((-10, 10))
    plt.show()

    plt.plot(iter, [0] * len(iter))
    plt.plot(iter, d_loss_cls, label='D/loss_cls', color='red')
    plt.plot(iter, d_loss_gp, label='D/loss_gp', color='orange')
    plt.plot(iter, g_loss_cls, label='G/loss_cls', color='green')
    plt.plot(iter, g_loss_rec, label='G/loss_rec', color='black')
    plt.ylim((0, 1))
    plt.show()


if __name__ == '__main__':
    main()
