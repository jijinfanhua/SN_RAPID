import os
import sys
import numpy as np
from tqdm import trange

def generate_packets(input_file, output_file,distribution_rate):
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
        # print(f"{flow_id[i]} {flow_rate[i]}")
    in_file.close()

    # 用flow_rate计算这个流应当占用多少时间.
    total_flow = np.sum(flow_rate)
    flow_ratio = []
    flow_time = []
    for i in range(num_flows):
        flow_ratio.append(flow_rate[i] / total_flow)# 这个流的数据包占比
        flow_time.append(np.ceil(flow_ratio[i] * total_time))# 这个流应该的时间占比

    arrival_time = []
    time = 0
    arrival_time.append(time)# 存储在什么时候来到
    for i in range(num_flows):
        time = time + flow_time[i]
        arrival_time.append(int(time))

    for i in trange(num_flows):
        lambda_val = flow_rate[i] / flow_time[i]    # 计算每个流的平均到达率，根据流量速率和持续时间
        inter_arrival_times = np.random.poisson(distribution_rate / lambda_val, flow_rate[i])
        arrival_times = np.cumsum(inter_arrival_times) + arrival_time[i]
        arrival_times = arrival_times[arrival_times <  10 * arrival_time[i + 1]] # 这里的2,换成任意一个大于1的c其实就可以.为什么是2?因为这个会影响在最后超出十万的部分

        for packet_time in arrival_times:
            out_file.write(f"{packet_time} {flow_id[i]}\n")

    out_file.close()
    print("Done~")

def sort_file_by_first_number(input_file, output_file):
    with open(input_file, 'r') as f:
        lines = f.readlines()
    lines.sort(key=lambda x: float(x.split()[0]))
    with open(output_file, 'w') as f:
        for line in lines:
            f.write(line)

def modify_file(input_filename, output_filename):
    with open(input_filename, 'r') as in_file:
        lines = in_file.readlines()
    data = [(int(x.split()[0]), int(x.split()[1])) for x in lines]
    unique_numbers = set()
    for i in range(len(data)):
        while data[i][0] in unique_numbers:
            data[i] = (data[i][0] + 1, data[i][1])
        unique_numbers.add(data[i][0])
    with open(output_filename, 'w') as out_file:
        for item in data:
            out_file.write(f"{item[0]} {item[1]}\n")

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