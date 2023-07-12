import numpy as np
import sys

# 包的数量
# 时间,确定吞吐
# 流的数量

num_packets = int(sys.argv[1])
time = int(sys.argv[2])
num_flows = int(sys.argv[3])
i = int(sys.argv[4])

# num_packets = 10000
# time = 20000
# num_flows = 1000

# zipf分布
zipf_distribution = np.random.zipf(1.2, num_flows)
# 对zipf分布的值进行缩放
zipf_distribution = zipf_distribution / max(zipf_distribution)
# 归一化，和为包数
normalized_zipf_distribution = zipf_distribution / sum(zipf_distribution) * num_packets
# 包的总数为整数
normalized_zipf_distribution = (np.ceil(normalized_zipf_distribution).astype(int))
# 调一下综述
# normalized_zipf_distribution[-1] = num_packets - sum(normalized_zipf_distribution[:-1])

file_name = ("./input_{}.txt".format(i))
with open(file_name, "w") as f:
    f.write(str(num_packets) + '\n')
    f.write(str(time) + '\n')
    f.write(str(num_flows) + '\n')

    for i in range(num_flows):
        num_packets_in_flow = normalized_zipf_distribution[i]
        f.write(str(i) + " " + str(num_packets_in_flow) + "\n")