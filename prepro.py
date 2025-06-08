from collections import defaultdict
import math
import random

# 入力: 無向エッジリスト
input_file = "Asia.txt"
output_file = "Asia_weighted.txt"

# 1. 隣接リスト構築
friends = defaultdict(set)
edges = set()

with open(input_file) as f:
    for line in f:
        a, b = map(int, line.strip().split())
        friends[a].add(b)
        friends[b].add(a)
        # 無向なので (小さい, 大きい) に統一して一回だけ記録
        edges.add((min(a, b), max(a, b)))

# 2. 共通の友人数に基づく重み付きエッジリストを作成
with open(output_file, "w") as f_out:
    for a, b in edges:
        common = len(friends[a].intersection(friends[b]))
        weight = random.uniform(1, 10)
        #random.seed(42) 
        f_out.write(f"{a} {b} {weight:.6f}\n")
