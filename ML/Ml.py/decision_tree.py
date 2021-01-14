from math import log2
import json

import pandas
from pandas import DataFrame


def probe(func):
    name = func.__name__.strip("calc")

    def wrapper(*args, **kwargs):
        result = func(*args, **kwargs)
        if len(args) >= 2:
            print(f"${name}(D, \\text{'{'}{args[1]}{'}'}) = {result:.3f}$")
        else:
            print(name, result)
        return result

    return wrapper


def calcEnt(datasets: DataFrame, class_=None):
    total_num = len(datasets)
    label_counts = datasets.groupby(datasets.columns[-1]).size()
    # $ Ent(D) = - \sum_{p \in P} p \log_2(p) $
    ent = -sum([(num / total_num) * log2(num / total_num) for num in label_counts])
    return ent


@probe
def calcGain(datasets: DataFrame, class_, ent=None):
    if ent is None:
        ent = calcEnt(datasets)
    total_size = len(datasets)
    groups = datasets.groupby(class_).apply(lambda x: (len(x), calcEnt(x)))

    # $Gain(D,a) = Ent(D) - \sum_{v \in V} \frac{ |D^v| }{ |D| Ent(D^v) }$
    return ent - sum(
        [(group_size / total_size) * group_ent for group_size, group_ent in groups]
    )


@probe
def calcGainRatio(datasets: DataFrame, class_, ent=None):
    if ent is None:
        ent = calcEnt(datasets)
    total_size = len(datasets)
    groups = datasets.groupby(class_).apply(lambda x: (len(x), calcEnt(x)))

    # $Gain(D,a) = Ent(D) - \sum_{v \in V} \frac{ |D^v| }{ |D| Ent(D^v) }$
    gain = ent - sum(
        [group_size / total_size * group_ent for group_size, group_ent in groups]
    )

    # $IV = - \sum^V_{v=1} \frac{ |D^v| }{ |D| } log2( \frac{ |D^v| }{ |D| })$
    IV = -sum(
        [
            group_size / total_size * log2(group_size / total_size)
            for (group_size, grout_ent) in groups
        ]
    )
    # $GainRatio(D,a) = \frac{ Gain(D,a) }{ IV }$
    return gain / IV


def createTree(datasets: DataFrame, gain=calcGain, classes=None):
    if classes is None:
        classes = datasets.columns[:-1]

    groups = datasets.groupby("好瓜")

    if len(groups) == 1:  # 取出分类标签（好瓜/坏瓜），如果只有一种，不需要继续分割
        keys = list(groups.groups.keys())
        return keys[0]

    if len(classes) == 1:  # 没有别的类别可以用来分割，取概率最大的分类标签
        return groups.size().idxmax()

    class_gains = {class_: gain(datasets, class_) for class_ in classes}
    best_class, best_gains = max(class_gains.items(), key=lambda x: x[1])

    print(f"=========== Best: {best_class}:{best_gains} ===========")

    tree = {best_class: {}}
    search_classes = [key for key in classes if key != best_class]
    for sub_type, group in datasets.groupby(best_class):
        sub_tree = createTree(group, gain=gain, classes=search_classes)
        tree[best_class][sub_type] = sub_tree

    return tree


def main():
    dataset = pandas.read_csv("data/2.0.csv")

    tree1 = createTree(dataset)
    print(json.dumps(tree1, ensure_ascii=False, indent=2))
    tree2 = createTree(dataset, gain=calcGainRatio)
    print(json.dumps(tree2, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
