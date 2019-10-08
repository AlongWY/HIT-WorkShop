import os
import random
import numpy as np
from PIL import Image
from chainer.dataset import dataset_mixin


class PixFaceDataset(dataset_mixin.DatasetMixin):
    def __init__(self, data_path='./face', data_range=(0, 300)):
        print("load dataset start")
        print("    from: %s" % data_path)
        print("    range: [%d, %d)" % (data_range[0], data_range[1]))
        self.data_path = data_path
        self.dataset = []
        for i in range(data_range[0], data_range[1]):
            img = Image.open(data_path + "/cmp_b%04d.jpg" % i)
            label = Image.open(data_path + "/cmp_b%04d.png" % i)
            w, h = img.size
            r = 286 / float(min(w, h))
            # resize images so that min(w, h) == 286
            img = img.resize((int(r * w), int(r * h)), Image.BILINEAR)
            label = label.resize((int(r * w), int(r * h)), Image.NEAREST)

            img = np.asarray(img).astype("f").transpose(2, 0, 1) / 128.0 - 1.0
            label_ = np.asarray(label)
            label = np.zeros((12, img.shape[1], img.shape[2])).astype("i")
            for j in range(12):
                label[j, :] = label_ == 0
            self.dataset.append((img, label))
        print("load dataset done")

    def __len__(self):
        return len(self.dataset)

    # return (label, img)
    def get_example(self, i, crop_width=256):
        _, h, w = self.dataset[i][0].shape
        x_l = np.random.randint(0, w - crop_width)
        x_r = x_l + crop_width
        y_l = np.random.randint(0, h - crop_width)
        y_r = y_l + crop_width
        return self.dataset[i][1][:, y_l:y_r, x_l:x_r], self.dataset[i][0][:, y_l:y_r, x_l:x_r]


class CelebADataset(dataset_mixin.DatasetMixin):
    def __init__(self, image_dir, attr_path, selected_attrs, mode, crop=178, size=128):
        self.image_dir = image_dir
        self.attr_path = attr_path
        self.selected_attrs = selected_attrs
        self.mode = mode
        self.crop_to = crop
        self.resize_to = size
        self.train_dataset = []
        self.test_dataset = []
        self.attr2idx = {}
        self.idx2attr = {}
        self.process()
        self.num_images = len(self.train_dataset) if mode == 'train' else len(self.test_dataset)

    def __len__(self):
        return self.num_images

    def get_example(self, i):
        dataset = self.train_dataset if self.mode == 'train' else self.test_dataset
        filename, label = dataset[i]
        image = Image.open(os.path.join(self.image_dir, filename))

        w, h = image.size
        x_l = w - self.crop_to
        x_r = x_l + self.crop_to
        y_l = h - self.crop_to
        y_r = y_l + self.crop_to
        image = image.crop((x_l, y_l, x_r, y_r))
        image = image.resize((self.resize_to, self.resize_to), Image.BILINEAR)
        image = np.asarray(image).astype("f")
        image = image.transpose(2, 0, 1) / 128.0 - 1.0

        return image, np.asarray(label).astype('i')

    def process(self):
        lines = [line.rstrip() for line in open(self.attr_path, 'r')]
        all_attr_names = lines[1].split()
        for i, attr_name in enumerate(all_attr_names):
            self.attr2idx[attr_name] = i
            self.idx2attr[i] = attr_name

        lines = lines[2:]
        random.seed(1234)
        random.shuffle(lines)
        for i, line in enumerate(lines):
            split = line.split()
            filename = split[0]
            values = split[1:]

            label = []
            for attr_name in self.selected_attrs:
                idx = self.attr2idx[attr_name]
                label.append(values[idx] == '1')

            if (i + 1) < 2000:
                self.test_dataset.append([filename, label])
            else:
                self.train_dataset.append([filename, label])

        print('Finished preprocessing the CelebA dataset...')
