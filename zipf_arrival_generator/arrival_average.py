import os
import sys
import numpy as np
# poisson
# def generate_packets(input_file, output_file,distribution_rate):
#     print("Generating packets...")
#     in_file = open(input_file, "r")
#     out_file = open(output_file, "w")
#     num_packets = int(in_file.readline())
#     num_flows = int(in_file.readline())
#     total_time = int(in_file.readline())
#
#     flow_id = []
#     flow_rate = []
#     for i in range(num_flows):
#         tmp = in_file.readline().split()
#         flow_id.append(int(tmp[0]))
#         flow_rate.append(int(tmp[1]))
#         # print(f"{flow_id[i]} {flow_rate[i]}")
#     in_file.close()
#
#     # 用flow_rate计算这个流应当占用多少时间.
#     total_flow = np.sum(flow_rate)
#     flow_ratio = []
#     flow_time = []
#     for i in range(num_flows):
#         flow_ratio.append(flow_rate[i] / total_flow)# 这个流的数据包占比
#         flow_time.append(np.ceil(flow_ratio[i] * total_time))# 这个流应该的时间占比
#
#     arrival_time = []
#     time = 0
#     arrival_time.append(time)# 存储在什么时候来到
#     for i in range(num_flows):
#         time = time + flow_time[i]
#         arrival_time.append(int(time))
#
#     for i in range(num_flows):
#         lambda_val = flow_rate[i] / flow_time[i]    # 计算每个流的平均到达率，根据流量速率和持续时间
#         inter_arrival_times = np.random.poisson(distribution_rate / lambda_val, flow_rate[i])
#         arrival_times = np.cumsum(inter_arrival_times) + arrival_time[i]
#         arrival_times = arrival_times[arrival_times < arrival_time[i + 1]]
#
#         for packet_time in arrival_times:
#             out_file.write(f"{packet_time} {flow_id[i]}\n")
#
#     out_file.close()

import numpy as np

def generate_packets(input_file, output_file, distribution_rate):
    print("Generating packets...")
    in_file = open(input_file, "r")
    out_file = open(output_file, "w")
    num_packets = int(in_file.readline())
    num_flows = int(in_file.readline())
    total_time = int(in_file.readline())

    flow_id = []
    flow_rate = []
    for i in range(num_flows):
        tmp = in_file.readline().split()
        flow_id.append(int(tmp[0]))
        flow_rate.append(int(tmp[1]))

    in_file.close()

    total_flow = np.sum(flow_rate)
    flow_ratio = []
    flow_time = []
    for i in range(num_flows):
        flow_ratio.append(flow_rate[i] / total_flow)
        flow_time.append(np.ceil(flow_ratio[i] * total_time))

    for i in range(num_flows):
        lambda_val = flow_rate[i] / flow_time[i]    # 计算每个流的平均到达率，根据流量速率和持续时间
        inter_arrival_times = np.random.poisson(distribution_rate / lambda_val, flow_rate[i])
        # Randomly spread arrival times across total time instead of sequential intervals
        arrival_times = np.random.choice(range(total_time), size=flow_rate[i], replace=False)
        arrival_times.sort()  # Sort to keep timeline in order
        for packet_time in arrival_times:
            out_file.write(f"{packet_time} {flow_id[i]}\n")

    out_file.close()

def sort_file_by_first_number(input_file, output_file):
    # 打开文件，读取每一行，将其保存为一个列表
    with open(input_file, 'r') as f:
        lines = f.readlines()

    # 对列表进行排序，key为每行第一个数字
    lines.sort(key=lambda x: float(x.split()[0]))

    # 将排序后的行写入输出文件
    with open(output_file, 'w') as f:
        for line in lines:
            f.write(line)


def modify_file(input_filename, output_filename):
    with open(input_filename, 'r') as in_file:
        lines = in_file.readlines()

    # Parse lines into pairs of (int, int)
    data = [(int(x.split()[0]), int(x.split()[1])) for x in lines]

    # Set to store the unique integers
    unique_numbers = set()

    for i in range(len(data)):
        while data[i][0] in unique_numbers:
            data[i] = (data[i][0] + 1, data[i][1])
        unique_numbers.add(data[i][0])

    with open(output_filename, 'w') as out_file:
        for item in data:
            out_file.write(f"{item[0]} {item[1]}\n")

# def modify_file(input_filename, output_filename):
#     with open(input_filename, 'r') as in_file:
#         lines = in_file.readlines()
#
#     data = [(int(x.split()[0]), int(x.split()[1])) for x in lines]
#
#     # Dictionary to store the unique integers and how many times we've added 1
#     unique_numbers = {}
#
#     for i in range(len(data)):
#         # If the number is not in the dictionary, add it with value 0 (meaning we haven't added 1 yet)
#         if data[i][0] not in unique_numbers:
#             unique_numbers[data[i][0]] = 0
#         else:
#             # If the number is in the dictionary, add 1 to the number and update the dictionary
#             unique_numbers[data[i][0]] += 1
#             data[i] = (data[i][0] + unique_numbers[data[i][0]], data[i][1])
#
#     with open(output_filename, 'w') as out_file:
#         for item in data:
#             out_file.write(f"{item[0]} {item[1]}\n")
#

if __name__ == "__main__":
    input_file = sys.argv[1]
    # output_file = sys.argv[2]
    midfile = "./mid.txt"
    midfile2 = './mid2.txt'
    final_file = sys.argv[2]
    distribution_rate = float(sys.argv[3])
    generate_packets(input_file, midfile,distribution_rate)
    # remove_duplicate_lines(output_file)

    sort_file_by_first_number(midfile, midfile2)

    modify_file(midfile2, final_file)
    os.remove(midfile)
    os.remove(midfile2)