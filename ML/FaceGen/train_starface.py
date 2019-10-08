#!/usr/bin/env python

# python train_pixface.py -g 0 -i ./face --out result_face --snapshot_interval 10000

from __future__ import print_function
import argparse
import os

import chainer
from chainer import training
from chainer.training import extensions
from chainer import serializers

from net import StarGenerator
from net import StarDiscriminator
from updater import StarUpdater

from face_dataset import CelebADataset
from face_visualizer import star_out_image
import numpy as np


def main():
    parser = argparse.ArgumentParser(description='chainer implementation of StarGan')

    # Model configuration
    parser.add_argument('--c_dim', type=int, default=5, help='dimension of domain labels')
    parser.add_argument('--crop_size', type=int, default=178, help='crop size for the CelebA dataset')
    parser.add_argument('--image_size', type=int, default=128, help='image resolution')
    parser.add_argument('--g_conv_dim', type=int, default=64, help='number of conv filters in the first layer of G')
    parser.add_argument('--d_conv_dim', type=int, default=64, help='number of conv filters in the first layer of D')
    parser.add_argument('--g_repeat_num', type=int, default=6, help='number of residual blocks in G')
    parser.add_argument('--d_repeat_num', type=int, default=6, help='number of strided conv layers in D')
    parser.add_argument('--lambda_cls', type=float, default=1, help='weight for domain classification loss')
    parser.add_argument('--lambda_rec', type=float, default=10, help='weight for reconstruction loss')
    parser.add_argument('--lambda_gp', type=float, default=10, help='weight for gradient penalty')

    # Training configuration.
    parser.add_argument('--batchsize', '-b', type=int, default=16, help='Number of images in each mini-batch')
    parser.add_argument('--num_iters', type=int, default=200000, help='number of total iterations for training D')
    parser.add_argument('--num_iters_decay', type=int, default=100000, help='number of iterations for decaying lr')
    parser.add_argument('--g_lr', type=float, default=0.0001, help='learning rate for G')
    parser.add_argument('--d_lr', type=float, default=0.0001, help='learning rate for D')
    parser.add_argument('--n_critic', type=int, default=5, help='number of D updates per each G update')
    parser.add_argument('--beta1', type=float, default=0.5, help='beta1 for Adam optimizer')
    parser.add_argument('--beta2', type=float, default=0.999, help='beta2 for Adam optimizer')
    parser.add_argument('--selected_attrs', '--list', nargs='+', help='selected attributes for the CelebA dataset',
                        default=['Black_Hair', 'Blond_Hair', 'Brown_Hair', 'Male', 'Young'])

    # Miscellaneous.
    parser.add_argument('--gpu', '-g', type=int, default=-1, help='GPU ID (negative value indicates CPU)')
    parser.add_argument('--out', '-o', default='star_result', help='Directory to output the result')
    parser.add_argument('--resume', '-r', default='', help='Resume the training from snapshot')
    parser.add_argument('--gen_resume', default='', help='Resume the gen from snapshot')
    parser.add_argument('--dis_resume', default='', help='Resume the dis from snapshot')
    parser.add_argument('--seed', type=int, default=0, help='Random seed')

    # Directories.
    parser.add_argument('--image_dir', type=str, default='/home/along/CelebA/Img/img_align_celeba/')
    parser.add_argument('--attr_path', type=str, default='/home/along/CelebA/Anno/list_attr_celeba.txt')

    # Step size.
    parser.add_argument('--log_step', type=int, default=10, help='Interval of displaying log to console')
    parser.add_argument('--sample_step', type=int, default=1000, help='Interval of example')
    parser.add_argument('--model_save_step', type=int, default=10000, help='Interval of snapshot')
    parser.add_argument('--lr_update_step', type=int, default=1000)

    args = parser.parse_args()

    print('GPU: {}'.format(args.gpu))
    print('# Minibatch-size: {}'.format(args.batchsize))
    print('# epoch: {}'.format(args.num_iters))
    print('')

    # Set up a neural network to train
    gen = StarGenerator(args.g_conv_dim, args.c_dim, args.g_repeat_num)
    dis = StarDiscriminator(args.image_size, args.d_conv_dim, args.c_dim, args.d_repeat_num)

    if args.gpu >= 0:
        chainer.cuda.get_device(args.gpu).use()  # Make a specified GPU current
        gen.to_gpu()  # Copy the model to the GPU
        dis.to_gpu()

    # Setup an optimizer
    def make_optimizer(model, alpha=0.0002, beta1=args.beta1, beta2=args.beta2):
        optimizer = chainer.optimizers.Adam(alpha=alpha, beta1=beta1, beta2=beta2)
        optimizer.setup(model)
        optimizer.add_hook(chainer.optimizer.WeightDecay(alpha / args.num_iters_decay), 'hook_dec')
        return optimizer

    opt_gen = make_optimizer(gen, alpha=args.g_lr)
    opt_dis = make_optimizer(dis, alpha=args.d_lr)

    train_d = CelebADataset(args.image_dir, args.attr_path, args.selected_attrs, 'train',
                            crop=args.crop_size, size=args.image_size)
    test_d = CelebADataset(args.image_dir, args.attr_path, args.selected_attrs, 'test',
                           crop=args.crop_size, size=args.image_size)

    train_iter = chainer.iterators.SerialIterator(train_d, args.batchsize)
    test_iter = chainer.iterators.SerialIterator(test_d, args.batchsize)

    # Set up a trainer
    updater = StarUpdater(
        models=(gen, dis),
        iterator={
            'main': train_iter,
            'test': test_iter},
        optimizer={
            'opt_gen': opt_gen,
            'opt_dis': opt_dis},
        params={
            'c_dim': args.c_dim,
            'lambda_cls': args.lambda_cls,
            'lambda_rec': args.lambda_rec,
            'lambda_gp': args.lambda_gp,

            'num_iters': args.num_iters,
            'image_size': args.image_size,
            'selected_attrs': args.selected_attrs,
            'num_iters_decay': args.num_iters_decay,
            'n_critic': args.n_critic,

            'g_lr': args.g_lr,
            'd_lr': args.d_lr,
            'lr_update_step': args.lr_update_step
        },
        device=args.gpu)
    trainer = training.Trainer(updater, (args.num_iters, 'epoch'), out=args.out)

    sample_interval = (args.sample_step, 'iteration')
    snapshot_interval = (args.model_save_step, 'iteration')
    display_interval = (args.log_step, 'iteration')
    trainer.extend(extensions.snapshot(
        filename='star_snapshot_{.updater.iteration}.npz'),
        trigger=snapshot_interval)
    trainer.extend(extensions.snapshot_object(
        gen, 'star_gen_{.updater.iteration}.npz'), trigger=snapshot_interval)
    trainer.extend(extensions.snapshot_object(
        dis, 'star_dis_{.updater.iteration}.npz'), trigger=snapshot_interval)
    trainer.extend(extensions.LogReport(trigger=display_interval))
    trainer.extend(extensions.PrintReport([
        'epoch', 'iteration', 'D/loss_real', 'D/loss_fake', 'D/loss_cls', 'D/loss_gp',
        'G/loss_fake', 'G/loss_rec', 'G/loss_cls', 'lr_d', 'lr_g'
    ]), trigger=display_interval)
    trainer.extend(extensions.ProgressBar(update_interval=10))
    trainer.extend(star_out_image(updater, gen, args.out, att_num=args.c_dim, image_size=args.image_size),
                   trigger=sample_interval)

    if args.resume:
        # Resume from a snapshot
        chainer.serializers.load_npz(args.resume, trainer)
    if args.gen_resume:
        chainer.serializers.load_npz(args.gen_resume, gen)
    if args.dis_resume:
        chainer.serializers.load_npz(args.dis_resume, dis)

    # Run the training
    trainer.run()


if __name__ == '__main__':
    main()
