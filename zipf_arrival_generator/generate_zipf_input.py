# 程序的目的是为了生成大流和小流,可以先生成num_flow个满足zipf分布的数
# 然后把这些数归一化等,乘到总包数上面去
# 得到的数字是包的数量满足zipf分布,有flow_num个
import numpy as np
import sys
import matplotlib.pyplot as plt

num_packets = int(sys.argv[1])
num_flows = int(sys.argv[2])
time = int(sys.argv[3])
zipfa = float(sys.argv[4])

# 让流id满足zipf分布
zipf_distribution = np.random.zipf(zipfa, num_flows)
# 在下一步操作后,发现绝大多数淤积在1.0附近
zipf_distribution = zipf_distribution / max(zipf_distribution)
normalized_zipf_distribution = zipf_distribution / sum(zipf_distribution) * num_packets
normalized_zipf_distribution = np.ceil(normalized_zipf_distribution).astype(int)

filename = (f"./input_{num_packets}_{num_flows}_{time}_{zipfa}")
with open(filename,'w') as f:
    f.write(str(num_packets) + '\n')
    f.write(str(num_flows) + '\n')
    f.write(str(time) + '\n')
    f.write(str(zipfa) + '\n')

    for i in range(num_flows):
        num_packets_in_flow = normalized_zipf_distribution[i]
        f.write(str(i) + " " + str(num_packets_in_flow) + '\n')

# print(sum(zipf_distribution))
# zipf_distribution.sort()
# print(max(zipf_distribution))
# 对zipf分布的值进行缩放
# plt.plot(zipf_distribution)
# plt.show()
# plt.plot(normalized_zipf_distribution)
# plt.show()