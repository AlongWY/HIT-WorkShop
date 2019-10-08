using Plots;
using Latexify;
using Random;
using LinearAlgebra;
pyplot()

# range(0,2π,50)

# 定义一些常量、工具函数（添加噪声、计算均方误差）
MAX_ORDER = 9                         # 阶数
MAX_DATA_NUM = 20                     #
K = 10                                # K-折交叉验证（k-fold cross-validation）
MAX_NOISE = 0.2                       #
α = 0.05                              # 学习率
lr_decay = 0                          # 学习率权重衰减
Momentum = 0.9                        # 动量，为了快速收敛
MAX_ITER = 10000                      # 最大迭代次数
BATCH_SIZE = 25                       # 批次
rng = MersenneTwister(1234);
noise(x) = x + MAX_NOISE * randn(rng,Float64,size(x))

f(x) = sin(2π * x)

# 损失函数 无正则/有正则
E(y_real,y_predict) = (sum([Δy^2 for Δy in y_predict .- y_real])) / length(y_real)

# 使用开发集，验证集，测试集求解
# GetW(X_train,Y_train,λ)
function Calc(X_r, Y_r, GetW::Function)
    λs = []
    # 训练数据集（train dataset）：用来构建机器学习模型
    # 验证数据集（validation dataset）：辅助构建模型，用于在构建过程中评估模型，为模型提供无偏估计，进而调整模型超参数
    # 测试数据集（test dataset）：用来评估训练好的最终模型的性能
    train = 1:round(Integer,0.6MAX_DATA_NUM)
    valid = round(Integer,0.6MAX_DATA_NUM)+1:round(Integer,0.8MAX_DATA_NUM)
    test  = round(Integer,0.8MAX_DATA_NUM)+1:round(Integer,MAX_DATA_NUM)

    X_train,Y_train = X_r[train,:],Y_r[train]
    X_valid,Y_valid = X_r[valid,:],Y_r[valid]
    X_test, Y_test  = X_r[test,:], Y_r[test]

    W_hat = zeros(MAX_ORDER + 1,1)
    lnλ = 0 # 正则项系数的自然对数
    err = E(Y_valid,X_valid * W_hat)
    for t_lnλ in -30:0.01:10
        W = GetW(X_train,Y_train,exp(t_lnλ))
        new_error = E(Y_valid,X_valid * W)
        if new_error <= err
            err = new_error
            lnλ = t_lnλ
            W_hat = W
        end
        push!(λs, t_lnλ => new_error)
    end
    return W_hat, lnλ, λs, E(Y_test,X_test * W_hat)
end

# 用于绘图
function W_Plot(X,Y,W)
    x_Plot = 0:0.01:1
    X_Plot = reshape([xd^i for i in MAX_ORDER:-1:0 for xd in x_Plot],length(x_Plot),MAX_ORDER+1)
    Y_Plot = X_Plot * W

    scatter(X,Y,label="data")
    plot!(x_Plot,f.(x_Plot),label="real(x)")
    plot!(x_Plot,Y_Plot,label="predict(x)")
end

# 生成标签数据并打乱
x_s = shuffle(range(0,stop=1,length=MAX_DATA_NUM))
X_s = reshape([xd^i for i in MAX_ORDER:-1:0 for xd in x_s],length(x_s),MAX_ORDER+1)
Y_s = noise(f.(x_s))

train = 1:round(Integer,0.8MAX_DATA_NUM)
test = round(Integer,0.8MAX_DATA_NUM)+1:round(Integer,MAX_DATA_NUM)

X_train,Y_train = X_s[train,:],Y_s[train]
X_test,Y_test = X_s[test,:],Y_s[test]

# 解析解 无正则项
# $E_{\hat{W}} = \frac{1}{length(X)} ∑ (y - x\hat{w}))^2$
# $E_{\hat{W}} = \frac{1}{length(X)} (y - X\hat{W})^T(y - X\hat{W})$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = \frac{1}{length(X)} 2X^T(X\hat{W} - y)$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = 0$
#      ==> $\hat{W} = (X^TX)^{-1}X^Ty$

W_hat = (X_train' * X_train)^-1 * X_train' * Y_train

W_Plot(x_s,Y_s,W_hat)
savefig("ResolveNoLambda:$(E(Y_test,X_test * W_hat))")


# 解析解 有正则项
# $E_{\hat{W}} = \frac{1}{length(X)} [ ∑(x\hat{w} -y)^2 + λ∑\hat{w}^2 ]$
# $E_{\hat{W}} = \frac{1}{length(X)} [ (X\hat{W} - y)^T(X\hat{W} - y) + λ\hat{W}^T * \hat{W}]$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = \frac{1}{length(X)} [2X^T(X\hat{W} - y) + 2λ\hat{W}]$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = 0$
#      ==> $\hat{W} = (X^T * X + λI )^{-1}X^Ty$

GetW(X_train,Y_train,λ) = (X_train' * X_train + Matrix(I,MAX_ORDER + 1,MAX_ORDER + 1) * λ)^-1 * X_train' * Y_train

W_hat,lnλ,λs,err = Calc(X_s,Y_s,GetW)

plot([x[1] for x in λs],[y[2] for y in λs])
savefig("ResolveWithLambda-lambda-err")
W_Plot(x_s,Y_s,W_hat)
savefig("ResolveWithLambda:$(E(Y_test,X_test * W_hat))")

# 梯度下降 无正则项
# $\hat{W} = \hat{W} - α\frac{∂E_{\hat{W}}}{∂\hat{W}}$
W_hat = ones(MAX_ORDER+1)
v = zeros(MAX_ORDER+1)

for i = 1:MAX_ITER
    global W_hat,v
    ∂E_W = (X_train' * (X_train * W_hat - Y_train)) * (2 / MAX_DATA_NUM)
    lr_i = α / (1 + lr_decay * i)
    v = Momentum * v - lr_i * ∂E_W
    W_hat += v
end

W_Plot(x_s,Y_s,W_hat)
savefig("SGDNoLambda:$(E(Y_test,X_test * W_hat))")

# 梯度下降 有正则项
# $E_{\hat{W}} = \frac{1}{length(X)} [ ∑(x\hat{w} -y)^2 + λ∑\hat{w}^2 ]$
# $E_{\hat{W}} = \frac{1}{length(X)} [ (X\hat{W} - y)^T(X\hat{W} - y) + λ\hat{W}^T * \hat{W}]$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = \frac{1}{length(X)} [2X^T(X\hat{W} - y) + 2λ\hat{W}]$

function GetW(X_train,Y_train,λ)
    W_hat = zeros(MAX_ORDER + 1)
    v = zeros(MAX_ORDER + 1)

    for i = 1:MAX_ITER
        ∂E_W = (X_train' * (X_train * W_hat - Y_train) + λ * W_hat) * (2 / MAX_DATA_NUM)
        lr_i = α / (1 + lr_decay * i)
        v = Momentum * v - lr_i * ∂E_W
        W_hat += v
    end
    return W_hat
end

W_hat,lnλ,λs,err = Calc(X_s,Y_s,GetW)

plot([x[1] for x in λs],[y[2] for y in λs])
savefig("SGDWithLambda-lambda-err")
W_Plot(x_s,Y_s,W_hat)
savefig("SGDWithLambda:$(E(Y_test,X_test * W_hat))")

# 随机梯度下降 无正则项
# $\hat{W} = \hat{W} - α\frac{∂E_{\hat{W}}}{∂\hat{W}}$

W_hat = zeros(MAX_ORDER+1)
v = zeros(MAX_ORDER+1)

for i = 1:MAX_ITER
    global W_hat,v
    p = rand(1:length(Y_train),BATCH_SIZE)
    ∂E_W = (X_train[p,:]' * (X_train[p,:] * W_hat - Y_train[p])) * (2 / BATCH_SIZE)
    lr_i = α / (1 + lr_decay * i)
    v = Momentum * v - lr_i * ∂E_W
    W_hat += v
end

W_Plot(x_s,Y_s,W_hat)
E(Y_test,X_test * W_hat)

# 随机梯度下降 有正则项
# $E_{\hat{W}} = \frac{1}{length(X)} [ ∑(x\hat{w} -y)^2 + λ∑\hat{w}^2 ]$
# $E_{\hat{W}} = \frac{1}{length(X)} [ (X\hat{W} - y)^T(X\hat{W} - y) + λ\hat{W}^T * \hat{W}]$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = \frac{1}{length(X)} [2X^T(X\hat{W} - y) + 2λ\hat{W}]$

function GetW(X_train,Y_train,λ)
    W_hat = zeros(MAX_ORDER+1)
    v = zeros(MAX_ORDER+1)

    for i = 1:MAX_ITER
        p = rand(1:length(Y_train),BATCH_SIZE)
        ∂E_W = (X_train[p,:]' * (X_train[p,:] * W_hat - Y_train[p]) + λ * W_hat) * (2 / BATCH_SIZE)
        lr_i = α / (1 + lr_decay * i)
        v = Momentum * v - lr_i * ∂E_W
        W_hat += v
    end
    return W_hat
end

W_hat,lnλ,λs,err = Calc(X_s,Y_s,GetW)

plot([x[1] for x in λs],[y[2] for y in λs])
W_Plot(x_s,Y_s,W_hat)

# 共轭梯度下降 无正则项
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = \frac{1}{length(X)} 2X^T(X\hat{W} - y)$
# $X^T X \hat{W} = X^T Y$
#    $A  X  = b$

A = X_train' * X_train
b = X_train' * Y_train
W_hat = ones(MAX_ORDER+1)

rk = b - A * W_hat
pk = rk

while true
    global W_hat,rk,pk
    αk = rk' * rk / (pk' * A * pk)
    W_hat = W_hat + αk * pk
    rk2 = rk -  αk * A * pk

    rk_l2 = sum([x^2 for x in rk2])
    if rk_l2 < 1e-3 || isnan(rk_l2)
        break
    end

    Βk = rk2' * rk2 /  (rk' * rk)
    pk2 = rk2 + Βk * pk

    rk = rk2
    pk = pk2
end

W_Plot(x_s,Y_s,W_hat)
savefig("ConjugationNoLambda:$(E(Y_test,X_test * W_hat))")

# 共轭梯度下降 有正则项
# $E_{\hat{W}} = \frac{1}{length(X)} [ ∑(x\hat{w} -y)^2 + λ∑\hat{w}^2 ]$
# $E_{\hat{W}} = \frac{1}{length(X)} [ (X\hat{W} - y)^T(X\hat{W} - y) + λ\hat{W}^T * \hat{W}]$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = \frac{1}{length(X)} [2X^T(X\hat{W} - y) + 2λ\hat{W}]$
# $\frac{∂E_{\hat{W}}}{∂\hat{W}} = 0$
#      ==> $\hat{W} = (X^T * X + λI )^{-1}X^Ty$
#      ==> $(X^T * X + λI ) \hat{W} = X^Ty$
#                    $A  X  = b$

function GetW(X_train,Y_train,λ)
    A = X_train' * X_train + Matrix(I,MAX_ORDER + 1,MAX_ORDER + 1) * λ
    b = X_train' * Y_train
    W_hat = ones(MAX_ORDER+1)

    rk = b - A * W_hat
    pk = rk

    while true
        αk = rk' * rk / (pk' * A * pk)
        W_hat = W_hat + αk * pk
        rk2 = rk -  αk * A * pk

        rk_l2 = sum([x^2 for x in rk2])
        if rk_l2 < 0.01 || isnan(rk_l2)
            break
        end

        Βk = rk2' * rk2 /  (rk' * rk)
        pk2 = rk2 + Βk * pk

        rk = rk2
        pk = pk2
    end
    return W_hat
end

W_hat,lnλ,λs,err = Calc(X_s,Y_s,GetW)

plot([x[1] for x in λs],[y[2] for y in λs])
savefig("ConjugationWithLambda-Lambda-err")
W_Plot(x_s,Y_s,W_hat)
savefig("ConjugationWithLambda:$(E(Y_test,X_test * W_hat))")
