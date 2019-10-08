using Plots;
using Random;
using LinearAlgebra;
using Statistics;
pyplot();

#===============================================#
#                                               #
#                Tools functions                #
#                                               #
#===============================================#

function generate_dataset(sample_size::Int, feature_dim::Int, num_classes::Int)
    CORE_BIAS = 5
    # random get
    Y = rand(1:num_classes,sample_size)
    # make features
    X = randn(sample_size,feature_dim) .+ (CORE_BIAS .* Y)
    # make label
    Y = Int.([y == class for y in Y, class = 1:num_classes])
    return  X, Y
end

function rand_center(data,k)
    n = size(data,2)
    centroids = zeros(k,n)

    for i in 1:n
        dmin, dmax = minimum(data[1:end,i]), maximum(data[1:end,i])
        centroids[:,i] = dmin .+ (dmax - dmin) * rand(k)
    end
    return centroids
end

function draw(X,label,centroids)
    # draw the line and points
    color_list = [:red, :green, :blue, :yellow, :pink]
    colors = [color_list[l] for l in label]
    scatter(X[:,1], X[:,2],  markercolor = colors)
    scatter!(centroids[:,1],centroids[:,2],markercolor=color_list,marker=10)
end

#===============================================#
#                                               #
#                     K-Means                   #
#                                               #
#===============================================#

distance(P1,P2) = sqrt(sum((P1 - P2).^2))

function kmeans(X,NUM_CLASSES)
    centroids = rand_center(X,NUM_CLASSES)      # center points
    labels = zeros(Int,size(X,1))               # labels
    assignment = zeros(size(X,1))               # 方差

    while true
        old_labels = deepcopy(labels)
        for i in 1:size(X,1)
            min_dist, min_index = Inf, -1
            for j in 1:NUM_CLASSES
                dist = distance(X[i,:],centroids[j,:])
                if dist < min_dist
                    min_dist, min_index = dist, j
                    labels[i] = j
                    assignment[i] = dist^2
                end
            end
        end

        for m in 1:NUM_CLASSES
            M_class = X[[i for (i,value) in enumerate(labels) if value == m],:]
            centroids[m,:] = mean(M_class,dims = 1)
        end
        if old_labels == labels
            break
        end
    end
    return centroids,labels
end

SAMPLE_SIZE = 50
FEATURE_DIM = 2
NUM_CLASSES = 2
X,Y = generate_dataset(SAMPLE_SIZE,FEATURE_DIM,NUM_CLASSES)
centroids,labels = kmeans(X,NUM_CLASSES)
draw(X,labels,centroids)

#===============================================#
#                                               #
#                       EM                      #
#                                               #
#===============================================#

function Init_params(Data, K)
    N, D = size(Data)            # 元素数目，元素特性数目
    πs = zeros(1, K)             # k类GMM所占权重（influence factor）
    μs = rand_center(Data,K)     # k*D, 即k类的中心点
    Σs = zeros(K, D, D)          # k类GMM的协方差矩阵，每个是D*D的

    Dist = repeat(sum(Data .^ 2, dims = 2), outer = (1,K)) + repeat(sum(μs' .^ 2, dims = 1), outer = (N,1)) - 2Data * μs'
    labels = mapslices(argmin, Dist, dims = 2)

    for k in 1:K
        Xk = Data[[index for (index,value) in enumerate(labels) if value == k],:]
        πs[k] = size(Xk,1) / N
        Σs[k,:,:] = cov(Xk)
    end
    return μs, πs, Σs
end

function Gaussian(Data,μ,Σ)
    D = size(Data,1)          # 元素数目，元素特性数目
    detΣ = det(Σ)
    if detΣ == 0              # 以防行列式为0
        Σ = Σ + Matrix{Float64}(I, D, D) .* 0.01
        detΣ = det(Σ)
    end
    dist = (Data - μ)
    return (2π)^-0.5D * detΣ^-0.5 * exp(-0.5 * dist' * Σ^-1 * dist)
end

function γ(Data,πs,μs,Σs,K)
    N, D = size(Data)         # 元素数目，元素特性数目
    γs = zeros(N,K)
    for i = 1:N
        res = [πs[k] * Gaussian(Data[i,:],μs[k,:],Σs[k,:,:]) for k in 1:K]
        γs[[i],:] = res ./ sum(res)
    end
    return γs
end

function GMM(Data,K)
    N, D = size(Data)
    μs, πs, Σs = Init_params(Data, K)

    losses = Float64[]
    γs = zeros(N,K)
    for i in 1:20
        # E
        γs = γ(Data,πs,μs,Σs,K)

        # M
        Nk = sum(γs, dims = 1)
        πs = Nk ./ sum(Nk)

        for k in 1:K
            μs[[k],:] = Nk[k]^-1 .* sum(γs[:,k] .* Data, dims = 1)

            dist = γs[:,[k]].^0.5 .* (Data .- μs[[k],:])
            Σs[k,:,:] = Nk[k]^-1 .* dist' * dist
        end

        loglikelyhood = sum([log(sum([πs[k] * Gaussian(Data[n,:],μs[k,:],Σs[k,:,:]) for k in 1:K])) for n in 1:N])
        push!(losses,-loglikelyhood)
    end

    return μs, mapslices(argmax,γs,dims = 2), losses
end

SAMPLE_SIZE = 100
FEATURE_DIM = 2
NUM_CLASSES = 3
X,Y = generate_dataset(SAMPLE_SIZE,FEATURE_DIM,NUM_CLASSES)
centroids,labels,losses = GMM(X,NUM_CLASSES)
draw(X,labels,centroids)
plot(1:length(losses),losses)

#===============================================#
#                                               #
#                       UCI                     #
#                                               #
#===============================================#

using DataFrames, CSV;
function toLabel(labels)
    classes = unique(labels)
    function λ(label)
        for (index, value) in enumerate(classes)
            if label == value
                return index
            end
        end
    end
    return Int.([y == class for y in λ.(labels), class = 1:size(classes,1)]),classes
end

datasets = CSV.read("data/iris-cut.csv");
labels = Array{String}(copy(datasets[:Species]))

X = Matrix{Float64}(datasets[:,1:2])
Y,classes = toLabel(labels)
real_label = (Y[:,1] .+ 1)

centroids,labels,losses = GMM(X,2)
draw(X,labels,centroids)
plot(1:length(losses),losses)
sum(Int.(real_label .== labels))
