import numpy as np

# poisson

def generate_packets(input_file, output_file):
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

    for i in range(num_flows):
        lambda_val = flow_rate[i] / flow_time[i]
        inter_arrival_times = np.random.poisson(1 / lambda_val, flow_rate[i])
        arrival_times = np.cumsum(inter_arrival_times) + arrival_time[i]
        arrival_times = arrival_times[arrival_times < arrival_time[i + 1]]

        for packet_time in arrival_times:
            out_file.write(f"{packet_time} {flow_id[i]}\n")

    out_file.close()
    # 在这个时间内,要求每个流的到达时间满足正太分布

    # 将时间和到达的数据包对应的流输出到output文件中

if __name__ == "__main__":
    input_file = "./input_zipf.txt"
    output_file = "./output.txt"
    generate_packets(input_file, output_file)

