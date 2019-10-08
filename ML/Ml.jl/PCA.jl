using Random;
using LinearAlgebra;
using Statistics;
using ImageView, Images, Gtk.ShortNames;

#===============================================#
#                                               #
#                Tools functions                #
#                                               #
#===============================================#
function load_mnist(path, kind)
    labels_path = joinpath(path, "$(kind)-labels-idx1-ubyte")
    images_path = joinpath(path, "$(kind)-images-idx3-ubyte")

    labels = []
    open(labels_path, "r") do labels_file
        num, magic = reinterpret(Int32, reverse(read(labels_file,8)))
        labels = read(labels_file, num)
    end

    images = []
    open(images_path, "r") do images_file
        cols, rows, num, magic = reinterpret(Int32, reverse(read(images_file,16)))
        images = read(images_file, num * rows * cols)
        images = reshape(images, (rows * cols, num))
        # images->reshape(num, rows, cols)
    end

    return labels, images
end

#===============================================#
#                                               #
#                      PCA                      #
#                                               #
#===============================================#
function PCA(Data, Top_n_feat=10)
    # 特征标准化，平衡各个特征尺度
    μ = mean(Data, dims=2)
    mean_removed = Data .- μ

    # 计算协方差矩阵（Find covariance matrix）
    Σ = cov(mean_removed, dims=2)

    # 计算特征值(Find eigenvalues and eigenvectors)
    U, S, Vt = svd(Σ)

    # 降维
    Ur = U[:,1:Top_n_feat]
    DataReduce = Ur' * mean_removed

    # 恢复原数据
    Rebuild = (Ur * DataReduce) .+ μ
    return DataReduce, Rebuild
end

labels, images = load_mnist("data/mnist","train")

grid, frames, canvases = canvasgrid((2,2))  # 2 row, 2 columns
imshow(canvases[1,1], reshape(images[:,610],28,28)')

DataReduce, Rebuild = PCA(images, 1)
imshow(canvases[1,2], reshape(Rebuild[:,610],28,28)')

DataReduce, Rebuild = PCA(images, 50)
imshow(canvases[2,1], reshape(Rebuild[:,610],28,28)')

DataReduce, Rebuild = PCA(images, 100)
imshow(canvases[2,2], reshape(Rebuild[:,610],28,28)')

win = Window(grid)
Gtk.showall(win)

MSE = sum((Rebuild .- images) .^ 2 , dims = 1) ./ 784
MSE_means = sum(MSE) ./ 60000
