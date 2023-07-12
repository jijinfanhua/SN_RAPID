import numpy as np

num_packets = 161866303
num_flows = 4543261

# zipf分布
zipf_distribution = np.random.zipf(1.29, num_flows)
print(zipf_distribution)

# 对zipf分布的值进行缩放,为了归一化能够进行
zipf_distribution = zipf_distribution / max(zipf_distribution)
# 归一化，和为包数
normalized_zipf_distribution = zipf_distribution / sum(zipf_distribution) * num_packets
# 包的总数为整数
normalized_zipf_distribution = (np.ceil(normalized_zipf_distribution).astype(int))
# 调一下综述
# normalized_zipf_distribution[-1] = num_packets - sum(normalized_zipf_distribution[:-1])

import matplotlib.pyplot as plt

# 首先，我们需要对数据进行排序，以便我们的图形能有更好的表现
sorted_distribution = np.sort(normalized_zipf_distribution)[::-1]

plt.figure(figsize=(10, 6))
plt.plot(sorted_distribution, linewidth=2)
plt.title('Zipf Distribution (a=1.29)')
plt.xlabel('Flow ID')
plt.ylabel('Number of Packets')
plt.grid(True)
plt.show()
