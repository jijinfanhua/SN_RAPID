import os
import numpy as np
import sys

big_flow_num = int(sys.argv[5])
num_packets = int(sys.argv[1])
num_flows = int(sys.argv[2])
time = int(sys.argv[3])
atom_num_packets = np.floor(int(sys.argv[1]) / big_flow_num)
atom_time = np.floor(int(sys.argv[3]) / big_flow_num)

# content_to_add = str(int(num_packets) * big_flow_num)  + "\n" + str(num_flows) + "\n" + str(int(time) * big_flow_num) + "\n"




zipfa = float(sys.argv[4])

for i in range(big_flow_num):
    zipf_distribution = np.random.zipf(zipfa, num_flows)
    zipf_distribution = zipf_distribution / max(zipf_distribution)
    normalized_zipf_distribution = zipf_distribution / sum(zipf_distribution) * atom_num_packets
    normalized_zipf_distribution = (np.ceil(normalized_zipf_distribution).astype(int))

    file_name = ("./input_{}_{}_{}_{}_{}.txt".format(num_packets, num_flows, time, zipfa,big_flow_num))
    with open(file_name, "a") as f:
        for j in range(num_flows):
            num_packets_in_flow = normalized_zipf_distribution[j]
            f.write(str(j) + " " + str(num_packets_in_flow) + "\n")

content_to_add = str(int(num_packets)) + "\n" + str(num_flows) + "\n" + str(int(time)) + "\n"
file_name = ("./input_{}_{}_{}_{}_{}.txt".format(num_packets, num_flows, time, zipfa,big_flow_num))

midfile = "./midfile.txt"
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
        if elements[1] == str(big_flow_num):
            elements[1] = '1'
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