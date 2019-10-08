#!/usr/bin/env python

from __future__ import print_function

import chainer
import chainer.functions as F
from chainer import Variable
import chainer.computational_graph as c


class PixFaceUpdater(chainer.training.StandardUpdater):

    def __init__(self, *args, **kwargs):
        self.enc, self.dec, self.dis = kwargs.pop('models')
        super(PixFaceUpdater, self).__init__(*args, **kwargs)

    def loss_enc(self, enc, x_out, t_out, y_out, lam1=100, lam2=1):
        batchsize, _, w, h = y_out.array.shape
        loss_rec = lam1 * (F.mean_absolute_error(x_out, t_out))
        loss_adv = lam2 * F.sum(F.softplus(-y_out)) / batchsize / w / h
        loss = loss_rec + loss_adv
        chainer.report({'loss': loss}, enc)
        return loss

    def loss_dec(self, dec, x_out, t_out, y_out, lam1=100, lam2=1):
        batchsize, _, w, h = y_out.array.shape
        loss_rec = lam1 * (F.mean_absolute_error(x_out, t_out))
        loss_adv = lam2 * F.sum(F.softplus(-y_out)) / batchsize / w / h
        loss = loss_rec + loss_adv
        chainer.report({'loss': loss}, dec)
        return loss

    def loss_dis(self, dis, y_in, y_out):
        batchsize, _, w, h = y_in.array.shape

        L1 = F.sum(F.softplus(-y_in)) / batchsize / w / h
        L2 = F.sum(F.softplus(y_out)) / batchsize / w / h
        loss = L1 + L2
        chainer.report({'loss': loss}, dis)
        return loss

    def update_core(self):
        enc_optimizer = self.get_optimizer('enc')
        dec_optimizer = self.get_optimizer('dec')
        dis_optimizer = self.get_optimizer('dis')

        enc, dec, dis = self.enc, self.dec, self.dis
        xp = enc.xp

        batch = self.get_iterator('main').next()
        batchsize = len(batch)
        in_ch = batch[0][0].shape[0]
        out_ch = batch[0][1].shape[0]
        w_in = 256
        w_out = 256

        x_in = xp.zeros((batchsize, in_ch, w_in, w_in)).astype("f")
        t_out = xp.zeros((batchsize, out_ch, w_out, w_out)).astype("f")

        for i in range(batchsize):
            x_in[i, :] = xp.asarray(batch[i][0])
            t_out[i, :] = xp.asarray(batch[i][1])
        x_in = Variable(x_in)

        z = enc(x_in)
        x_out = dec(z)

        y_fake = dis(x_in, x_out)
        y_real = dis(x_in, t_out)

        enc_optimizer.update(self.loss_enc, enc, x_out, t_out, y_fake)
        for z_ in z:
            z_.unchain_backward()
        dec_optimizer.update(self.loss_dec, dec, x_out, t_out, y_fake)
        x_in.unchain_backward()
        x_out.unchain_backward()
        dis_optimizer.update(self.loss_dis, dis, y_real, y_fake)


class StarUpdater(chainer.training.StandardUpdater):
    def __init__(self, *args, **kwargs):
        self.gen, self.dis = kwargs.pop('models')
        params = kwargs.pop('params')

        # Model configuration
        self.c_dim = params['c_dim']
        self.lambda_cls = params['lambda_cls']
        self.lambda_rec = params['lambda_rec']
        self.lambda_gp = params['lambda_gp']

        # Training configurations
        self.num_iters = params['num_iters']
        self.image_size = params['image_size']
        self.selected_attrs = params['selected_attrs']
        self.num_iters_decay = params['num_iters_decay']

        # the cycle to train generator
        self.cycle = 0
        self.n_critic = params['n_critic']
        self.lr_update_step = params['lr_update_step']
        self.g_lr_decay = params['g_lr'] / self.num_iters_decay
        self.d_lr_decay = params['d_lr'] / self.num_iters_decay

        # xp
        self.xp = self.gen.xp

        super(StarUpdater, self).__init__(*args, **kwargs)

    def unpack(self, data: list):
        xp = self.xp
        batch_size = len(data)

        c, h, w = data[0][0].shape
        l = data[0][1].shape[0]

        img = xp.zeros((batch_size, c, h, w)).astype("f")
        label = xp.zeros((batch_size, l)).astype('i')

        for i in range(batch_size):
            img[i, :] = xp.asarray(data[i][0])
            label[i, :] = xp.asarray(data[i][1])

        return img, label

    def loss_func_rec_l1(self, x_out, target):
        return F.mean_absolute_error(x_out, target)

    def loss_func_adv_dis_fake(self, y_fake):
        return F.mean(y_fake)

    def loss_func_adv_dis_real(self, y_real):
        return F.mean(y_real)

    def loss_func_adv_gen(self, y_fake):
        return F.mean(y_fake)

    def classification_loss(self, input: Variable, target: Variable):
        return F.sigmoid_cross_entropy(input, target)

    def gradient_penalty(self, y: chainer.Variable, x: chainer.Variable):
        """Compute gradient penalty: (L2_norm(dy/dx) - 1)**2."""
        xp = self.xp
        weight = [Variable(xp.ones(y.shape, dtype='f'))]
        dydx, = chainer.grad(outputs=[y], inputs=[x], grad_outputs=weight, enable_double_backprop=True)
        dydx = F.sqrt(F.sum(dydx * dydx, axis=(1, 2, 3)))
        return F.mean_squared_error(dydx, xp.ones_like(dydx.array))

    def update_core(self):
        xp = self.xp
        w_in = self.image_size
        opt_gen = self.get_optimizer('opt_gen')
        opt_dis = self.get_optimizer('opt_dis')

        # =================================================================================== #
        #                             1. Preprocess input data                                #
        # =================================================================================== #

        # Fetch real images and labels.
        batch = self.get_iterator('main').next()
        batch_size = len(batch)
        x_real, label_org = self.unpack(batch)

        # Generate target domain labels randomly.
        rand_idx = xp.random.permutation(len(label_org))
        label_trg = label_org[rand_idx]

        c_org = label_org
        c_trg = label_trg

        c_trg = c_trg.astype('f')
        c_org = c_org.astype('f')

        # =================================================================================== #
        #                             2. Train the discriminator                              #
        # =================================================================================== #

        # Compute loss with real images.
        out_src, out_cls = self.dis(x_real)

        # FOR DEBUG
        # g = c.build_computational_graph([out_src, out_cls])
        # with open('star_result/dis', 'w') as o:
        #     o.write(g.dump())

        d_loss_real = - self.loss_func_adv_dis_real(out_src)
        d_loss_cls = self.classification_loss(out_cls, label_org)

        # Compute loss with fake images
        x_fake = self.gen(x_real, c_trg)

        # FOR DEBUG
        # g = c.build_computational_graph(x_fake)
        # with open('star_result/gen', 'w') as o:
        #     o.write(g.dump())

        x_fake.unchain_backward()
        out_src, out_cls = self.dis(x_fake)
        d_loss_fake = self.loss_func_adv_dis_fake(out_src)

        # WGAN
        alpha = xp.random.uniform(0, 1, size=len(x_real)).astype("f")[:, None, None, None]
        x_hat = Variable(alpha * x_real.array + (1 - alpha) * x_fake.array)
        out_src, _ = self.dis(x_hat)
        d_loss_gp = self.gradient_penalty(out_src, x_hat)

        # Backward and optimize.
        d_loss = d_loss_real + d_loss_fake + self.lambda_gp * d_loss_gp + self.lambda_cls * d_loss_cls
        self.dis.cleargrads()
        d_loss.backward()
        opt_dis.update()

        # Logging
        chainer.reporter.report({'D/loss_real': d_loss_real})
        chainer.reporter.report({'D/loss_fake': d_loss_fake})
        chainer.reporter.report({'D/loss_cls': d_loss_cls})
        chainer.reporter.report({'D/loss_gp': d_loss_gp})

        # =================================================================================== #
        #                               3. Train the generator                                #
        # =================================================================================== #

        if (self.cycle + 1) % self.n_critic == 0:
            # Original-to-target domain.
            x_fake = self.gen(x_real, c_trg)
            out_src, out_cls = self.dis(x_fake)
            out_cls = out_cls[:, :self.c_dim]
            g_loss_fake = - self.loss_func_adv_gen(out_src)
            g_loss_cls = self.classification_loss(out_cls, label_trg)

            # Target-to-original domain
            x_reconst = self.gen(x_fake, c_org)
            g_loss_rec = self.loss_func_rec_l1(x_reconst, x_real)

            # Backward and optimize
            g_loss = g_loss_fake + self.lambda_rec * g_loss_rec + self.lambda_cls * g_loss_cls
            self.gen.cleargrads()
            g_loss.backward()
            opt_gen.update()

            chainer.reporter.report({'G/loss_fake': g_loss_fake})
            chainer.reporter.report({'G/loss_cls': g_loss_rec})
            chainer.reporter.report({'G/loss_rec': g_loss_cls})

        # =================================================================================== #
        #                                 4. Miscellaneous                                    #
        # =================================================================================== #

        # Decay learning rates.
        if (self.cycle + 1 % self.lr_update_step == 0) and (self.cycle + 1) > (self.num_iters - self.num_iters_decay):
            if opt_gen.alpha > 10e-8:
                opt_gen.alpha -= self.g_lr_decay
            else:
                opt_gen.alpha = 10e-8

            if opt_dis.alpha > 10e-8:
                opt_dis.alpha -= self.d_lr_decay
            else:
                opt_dis.alpha = 10e-8

        chainer.reporter.report({'lr_g': opt_gen.alpha})
        chainer.reporter.report({'lr_d': opt_dis.alpha})

        self.cycle += 1
