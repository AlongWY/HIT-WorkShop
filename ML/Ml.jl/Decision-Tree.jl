using DataFrames, CSV;
using Serialization;

function calcEnt(datasets)
    num_samples = size(datasets,1)
    label_counts = Dict()
    for label in datasets[end]
        if label in keys(label_counts)
            label_counts[label] += 1
        else
            label_counts[label] = 1
        end
    end
    # $ Ent(D) = - ∑_{p ∈ P} p log_2(p) $
    return - sum([p * log2(p) for p in values(label_counts) ./ num_samples ])
end

function calcGain(datasets,class,ent = calcEnt(datasets))
    num_samples = size(datasets,1)
    groups = groupby(datasets, class)
    entries = [(size(subsets,1),calcEnt(subsets)) for subsets in groups]
    # $Gain(D,a) = Ent(D) - ∑_{v ∈ V} |D^v| / |D| Ent(D^v)$
    return ent - sum([group_size / num_samples * grout_ent for (group_size,grout_ent) in entries])
end

function calcGainRatio(datasets,class,ent = calcEnt(datasets))
    num_samples = size(datasets,1)
    groups = groupby(datasets, class)
    entries = [(size(subsets,1),calcEnt(subsets)) for subsets in groups]
    gain =  ent - sum([group_size / num_samples * grout_ent for (group_size,grout_ent) in entries])
    # $IV = - ∑^V_{v=1} |D^v| / |D| log2(|D^v| / |D|)$
    IV = - sum([group_size / num_samples * log2(group_size / num_samples) for (group_size,grout_ent) in entries])
    # $Gain(D,a) = Gain(D,a) / IV$
    return gain / IV
end

function maxLabel(datasets)
    return by(datasets, :好瓜, df -> DataFrame(N = size(df, 1)))
end

function createTree(datasets, classes = names(datasets))
    groups = groupby(datasets,classes[end])
    if length(groups) == 1          # 取出分类标签（好瓜/坏瓜），如果只有一种，不需要继续分割
        return groups[end][end][end]
    end
    if length(classes) == 1         # 没有别的类别可以用来分割，取概率最大的分类标签
        max_group = groups[1]
        for group in groups[2:end-1]
            if length(group) > length(max_group)
                max_group = group
            end
        end
        return max_group[end][end]
    end

    best_class = classes[1]         # 取出最好的分类类别，即信息增益最大的类别
    best_gain = calcGain(datasets,best_class)
    for class in classes[1:end-1]
        curr_gain = calcGain(datasets,class)
        if curr_gain > best_gain
            best_class = class
            best_gain = curr_gain
        end
    end

    new_classes = copy(classes)     # 递归调用，下一个分支需要用到的分类标签
    filter!(x->x!=best_class, new_classes)   # 由于julia传引用，需要拷贝一份
    best_class_label = Set(datasets[best_class])
    tree = Dict(best_class => Dict())

    # 遍历该类别标准下所有子分类，递归求解
    for group in groupby(datasets,best_class)
        tree[best_class][group[best_class][1]] = createTree(group,new_classes)
    end

    return tree
end

datasets = CSV.read("Work/ML/2.0.csv");
tree = createTree(datasets)

print(tree)

describe(datasets)
by(datasets, :好瓜, df -> DataFrame(N = size(df, 1)))

stream = open("Work/ML/tree.data","w")
serialize(stream,tree)
close(stream)

stream = open("Work/ML/tree.data","r")
read_tree = deserialize(stream)
close(stream)
