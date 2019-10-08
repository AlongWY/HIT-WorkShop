using Plots;
using Random;
using LinearAlgebra;
pyplot()

#===============================================#
#                                               #
#                Plots Settings                 #
#                                               #
#===============================================#

upscale = 2                             # 8x upscaling in resolution
fntsm = Plots.font("sans-serif", 10.0*upscale)
fntlg = Plots.font("sans-serif", 14.0*upscale)
default(titlefont=fntlg, guidefont=fntlg, tickfont=fntsm, legendfont=fntsm)
default(size=(800*upscale,600*upscale)) # Plot canvas size
default(dpi=300)                        # Only for PyPlot - presently broken

#===============================================#
#                                               #
#                  Definations                  #
#                                               #
#===============================================#

CORE_BIAS = 3
SAMPLE_SIZE = 50
FEATURE_DIM = 2
NUM_CLASSES = 2
λ = 0.1             # normalize
α = 0.001           # learning rate

#===============================================#
#                                               #
#                Tools functions                #
#                                               #
#===============================================#

function generate_dataset(sample_size::Int, feature_dim::Int=FEATURE_DIM, num_classes::Int=NUM_CLASSES)
    # random get
    Y = rand(1:num_classes,sample_size)
    # make features
    X = randn(sample_size,feature_dim) .+ (CORE_BIAS .* Y)
    # make label
    Y = Int.([y == class for y in Y, class = 1:num_classes])
    return hcat(ones(SAMPLE_SIZE,1), X) ,Y
end

# Gradient Descent
# π: predict(θ,X)
# ∇: gradient(π(θ,X),X,Y)
# loss: loss(π(θ,X),X,Y)
function gradient_descent(X,Y,π::Function,∇::Function,loss::Function,θ::Array,λ = 0)
    LR_DECAY = 0.1                  # lr_decay
    MOMENTUM = 0.9                  # Momentum
    i = 0

    P = π(θ,X)
    losses = Float64[loss(P,X,Y)]
    v = zeros(size(θ))
    while losses[end] > 0.1
        p = π(θ,X)
        Δ = ∇(p,X,Y) + λ * θ ./ length(Y)        # normalize, if λ = 0, equal to no normalization
        lr_i = α / (1 + LR_DECAY * i)
        v = MOMENTUM * v - lr_i * Δ
        θ += v
        push!(losses,loss(p,X,Y) + λ * sum(θ .* θ) ./ 2length(Y))
        println(losses[end])
    end
    return θ,losses
end

function gradient_descent(X,Y,π::Function,∇::Function,loss::Function,λ = 0)
    θ = ones(size(X,2),size(Y,2))
    return gradient_descent(X,Y,π,∇,loss,θ,λ)
end

function newtown(X,Y,π::Function,loss::Function,θ::Array,λ = 0)
    LR_DECAY = 0                      # lr_decay
    MOMENTUM = 0                      # Momentum
    i = 0

    P = π(θ,X)
    losses = Float64[loss(P,X,Y)]
    v = zeros(size(θ))
    while losses[end] > 0.1
        P = π(θ,X)
        A = Diagonal((P .* (1 .- P))[:,])
        U = X' * (Y .- P)
        H = X' * A * X
        lr_i = α / (1 + LR_DECAY * i)
        v = MOMENTUM * v + lr_i * (H^-1 * U - λ * θ ./ length(Y))
        θ = θ + v
        i = i + 1
        push!(losses,loss(P,X,Y) + λ * sum(θ .* θ) ./ 2length(Y))
        println(losses[end])
    end
    return θ,losses
end

function newtown(X,Y,π::Function,loss::Function,λ = 0)
    θ = ones(size(X,2),size(Y,2))
    return newtown(X,Y,π,loss,θ,λ)
end

function draw(θ,X,Y,losses,name::String)
    # draw the line and points
    x1 = Array(1:1:7)
    x2 = (- θ[1,1] .- θ[2,1] .* x1) ./ θ[3,1]
    colors = [[:red, :green][l+1] for l in Y[:,1]]
    scatter(X[:,2], X[:,3],  markercolor = colors)
    plot!(x1, x2)
    savefig("$name-plot")
    # losses plot
    plot(1:size(losses,1),losses[1:end])
    savefig("$name-loss")
end

#===============================================#
#                                               #
#               Logistic Regression             #
#                                               #
#===============================================#

# generate data
X,Y = generate_dataset(SAMPLE_SIZE,FEATURE_DIM,2)
Y = Y[:,1]

# class model
sigmoid(θ,X) = 1 ./ (1 .+ exp.(- X * θ))

# gradient
# ∇(sigmoid(θ,X),X,Y)
∇(π,X,Y) = X' * (π - Y) ./ size(Y,1)

# loss function
# loss(sigmoid(θ,X),X,Y)
loss(π,X,Y) = - sum(Y .* log.(π) + (1 .- Y) .* log.(1 .- π)) ./ length(Y)

θ, losses = gradient_descent(X,Y,sigmoid,∇,loss)
draw(θ,X,Y,losses,"grad")

θ, losses = newtown(X,Y,sigmoid,loss)
draw(θ,X,Y,losses,"newtown")

#===============================================#
#                                               #
#                    Test  UCI                  #
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

X = Matrix{Float64}(datasets[:,1:end-1])
X = hcat(ones(size(X,1),1), X)
Y,classes = toLabel(labels)
Y = Y[:,1]

θ, losses = gradient_descent(X,Y,sigmoid,∇,loss)
draw(θ,X,Y,losses,"iris")

#===============================================#
#                                               #
#                Softmax Regression             #
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

# class model
function softmax(θ,X)
    Y = exp.(X * θ)
    total = sum(Y,dims = 2)
    return Y ./ total
end

# gradient
# ∇(softmax(θ,X),X,Y)
∇(π,X,Y) = X' * (π - Y) ./ length(Y)

# loss function
# loss(softmax(θ,X),X,Y)
loss(π,X,Y) = - sum(Y .* log.(π)) ./ length(Y)

datasets = CSV.read("data/iris.csv");
labels = Array{String}(copy(datasets[:Species]))

# generate data
X = Matrix{Float64}(datasets[:,1:end-1])
# set bias row
X = hcat(ones(size(X,1),1), X)
Y,classes = toLabel(labels)

θ, losses = gradient_descent(X,Y,softmax,∇,loss)
θ, losses = gradient_descent(X,Y,softmax,∇,loss,θ)

# softmax
softmax(θ,X)

# losses plot
plot(1:size(losses,1),losses)
