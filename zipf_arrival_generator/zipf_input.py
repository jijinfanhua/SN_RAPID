import numpy as np
import sys
import matplotlib.pyplot as plt

num_packets = int(sys.argv[1])
num_flows = int(sys.argv[2])
time = int(sys.argv[3])
zipfa = float(sys.argv[4])

# 程序的目的是为了生成大流和小流,可以先生成num_flow个满足zipf分布的数
# 然后把这些数归一化等,乘到总包数上面去
# 得到的数字是包的数量满足zipf分布,有flow_num个
# zipf分布
zipf_distribution = np.random.zipf(zipfa, num_flows)
# 对zipf分布的值进行缩放
zipf_distribution = zipf_distribution / max(zipf_distribution)
# 归一化，和为包数
normalized_zipf_distribution = zipf_distribution / sum(zipf_distribution) * num_packets
# 包的总数为整数
normalized_zipf_distribution = (np.ceil(normalized_zipf_distribution).astype(int))
# 调一下综述
# normalized_zipf_distribution[-1] = num_packets - sum(normalized_zipf_distribution[:-1])

# normalized_zipf_distribution.sort()
# plt.plot(normalized_zipf_distribution)
# plt.show()
# #
file_name = ("./input_{}_{}_{}_{}.txt".format(num_packets, num_flows, time, zipfa))
with open(file_name, "w") as f:
    f.write(str(num_packets) + '\n')
    f.write(str(num_flows) + '\n')
    f.write(str(time) + '\n')

    for i in range(num_flows):
        num_packets_in_flow = normalized_zipf_distribution[i]
        f.write(str(i) + " " + str(num_packets_in_flow) + "\n")