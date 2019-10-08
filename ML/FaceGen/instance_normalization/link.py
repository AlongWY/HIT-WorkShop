import numpy

from chainer.backends import cuda
from chainer import configuration
from chainer import functions
from chainer import initializers
from chainer import link
from chainer.utils import argument
from chainer import variable


class InstanceNormalization(link.Link):
    """Instance normalization layer on outputs of convolution functions.
    It is recommended to use this normalization instead of batch normalization
    in generative models of what we call Style Transfer.
    """

    def __init__(self, size, decay=0.9, eps=2e-5, dtype=numpy.float32,
                 use_gamma=True, use_beta=True,
                 initial_gamma=None, initial_beta=None):
        super(InstanceNormalization, self).__init__()
        self.avg_mean = numpy.zeros(size, dtype=dtype)
        self.register_persistent('avg_mean')
        self.avg_var = numpy.zeros(size, dtype=dtype)
        self.register_persistent('avg_var')
        self.N = 0
        self.register_persistent('N')
        self.decay = decay
        self.eps = eps

        with self.init_scope():
            if use_gamma:
                if initial_gamma is None:
                    initial_gamma = 1
                initial_gamma = initializers._get_initializer(initial_gamma)
                initial_gamma.dtype = dtype
                self.gamma = variable.Parameter(initial_gamma, size)
            if use_beta:
                if initial_beta is None:
                    initial_beta = 0
                initial_beta = initializers._get_initializer(initial_beta)
                initial_beta.dtype = dtype
                self.beta = variable.Parameter(initial_beta, size)

    def __call__(self, x: variable.Variable, **kwargs):
        """__call__(self, x, finetune=False)
        Invokes the forward propagation of BatchNormalization.
        In training mode, the BatchNormalization computes moving averages of
        mean and variance for evaluation during training, and normalizes the
        input using batch statistics.
        .. warning::
           ``test`` argument is not supported anymore since v2.
           Instead, use ``chainer.using_config('train', False)``.
           See :func:`chainer.using_config`.
        Args:
            x (Variable): Input variable.
            finetune (bool): If it is in the training mode and ``finetune`` is
                ``True``, BatchNormalization runs in fine-tuning mode; it
                accumulates the input array to compute population statistics
                for normalization, and normalizes the input using batch
                statistics.
        """
        # check argument
        argument.check_unexpected_kwargs(
            kwargs, test='test argument is not supported anymore. '
                         'Use chainer.using_config')
        finetune, = argument.parse_kwargs(kwargs, ('finetune', False))

        b, c = x.shape[0], x.shape[1]
        reshaped_x = functions.reshape(x, (1, b * c, *x.shape[2:]))
        # Repeat stored stats and affine transform params
        mean = variable.Variable(self.xp.hstack([self.avg_mean] * b))
        var = variable.Variable(self.xp.hstack([self.avg_var] * b))

        if hasattr(self, 'gamma'):
            gamma = variable.Variable(self.xp.hstack([self.gamma.array] * b))
        else:
            with cuda.get_device_from_id(self._device_id):
                gamma = variable.Variable(self.xp.ones(
                    mean.shape, dtype=x.dtype))

        if hasattr(self, 'beta'):
            beta = variable.Variable(self.xp.hstack([self.beta.array] * b))
        else:
            with cuda.get_device_from_id(self._device_id):
                beta = variable.Variable(self.xp.zeros(
                    mean.shape, dtype=x.dtype))

        if configuration.config.train:
            if finetune:
                self.N += 1
                decay = 1. - 1. / self.N
            else:
                decay = self.decay

            ret = functions.batch_normalization(
                reshaped_x, gamma, beta, eps=self.eps, running_mean=mean,
                running_var=var, decay=decay)
        else:
            # Use running average statistics or fine-tuned statistics.
            mean = variable.Variable(self.avg_mean)
            var = variable.Variable(self.avg_var)
            ret = functions.fixed_batch_normalization(
                reshaped_x, gamma, beta, mean, var, self.eps)
        return functions.reshape(ret, x.shape)

    def start_finetuning(self):
        """Resets the population count for collecting population statistics.

        This method can be skipped if it is the first time to use the
        fine-tuning mode. Otherwise, this method should be called before
        starting the fine-tuning mode again.

        """
        self.N = 0
