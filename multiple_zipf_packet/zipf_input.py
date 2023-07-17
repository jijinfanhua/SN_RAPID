import os

import numpy as np
import sys
# import matplotlib.pyplot as plt

big_flow_num = int(sys.argv[5])
num_packets = np.floor(int(sys.argv[1]) / big_flow_num)
num_flows = int(sys.argv[2])
time = np.floor(int(sys.argv[3]) / big_flow_num)
zipfa = float(sys.argv[4])

# 程序的目的是为了生成大流和小流,可以先生成num_flow个满足zipf分布的数
# 然后把这些数归一化等,乘到总包数上面去
# 得到的数字是包的数量满足zipf分布,有flow_num个
# zipf分布
for i in range(big_flow_num):
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
    with open(file_name, "a") as f:
        # f.write(str(num_packets) + '\n')
        # f.write(str(num_flows) + '\n')
        # f.write(str(time) + '\n')

        for j in range(num_flows):
            num_packets_in_flow = normalized_zipf_distribution[j]
            f.write(str(j) + " " + str(num_packets_in_flow) + "\n")

content_to_add = str(int(num_packets)) + "\n" + str(num_flows) + "\n" + str(int(time)) + "\n"
file_name = ("./input_{}_{}_{}_{}.txt".format(num_packets, num_flows, time, zipfa))

# with open(file_name,'r') as file:
#     original_content = file.read()
# content_to_add +=  original_content
#
# file.close()
# with open(file_name, 'w') as file:
#     # 将拼接后的内容写入文件
#     file.write(content_to_add)
# # 关闭文件
# file.close()

midfile = "./midfile.txt"
# 所有相同流求和
def combine_rows(input_file, output_file):
    result_dict = {}
    for line in input_file:
        key, value = line.split()
        key = int(key)
        value = int(value)
        if key in result_dict:
            result_dict[key] += value
        else:
            result_dict[key] = value
    for key, value in result_dict.items():
        output_file.write(f"{key} {value}\n")
with open(file_name, 'r') as input_file, open(midfile, 'w') as output_file:
    combine_rows(input_file, output_file)

midfile2 = "./midfile2.txt"

def change_values(input_file, output_file):
    for line in input_file:
        elements = line.split()
        # 如果这一行的第二个元素是'10'，就将其更改为'1'
        if elements[1] == str(big_flow_num):
            elements[1] = '1'
        # 将更改后的行写入新文件
        output_file.write(' '.join(elements) + '\n')

with open(midfile, 'r') as input_file, open(midfile2, 'w') as output_file:
    change_values(input_file, output_file)

with open(midfile2,'r') as input_file,open(file_name,'w') as output_file:
    for line in input_file:
        output_file.write(line)

with open(file_name,'r') as file:
    original_content = file.read()
content_to_add +=  original_content

file.close()
with open(file_name, 'w') as file:
    # 将拼接后的内容写入文件
    file.write(content_to_add)
# 关闭文件
file.close()

os.remove(midfile)
os.remove(midfile2)