import numpy as np
from scipy.stats import zipf, norm

def generate_packets(input_file, output_file):
    print("Generating packets...")
    with open(input_file, "r") as _input:
        num_packets = int(_input.readline())
        num_flows = int(_input.readline())
        total_time = int(_input.readline())

        flow_id = []
        flow_rate = []
        for i in range(num_flows):
            tmp= _input.readline().split()
            flow_id.append(int(tmp[0]))
            flow_rate.append(int(tmp[1]))
    # generate flows' arrival time
    mean = total_time / 2
    std_dev = total_time / 4
    arrival_time = np.random.normal(loc=mean, scale=std_dev, size=num_flows)
    arrival_time = arrival_time.clip(0, total_time)
    arrival_time.sort()

    for i in range(len(arrival_time)):
        arrival_time[i] = np.floor(arrival_time[i])
        # print(arrival_time[i])

    duration = []
    for i in range(len(arrival_time)):
        duration.append(total_time - arrival_time[i])
        # print(duration[i])

    # print(f"{num_flows},{len(duration)}")
    with open(output_file, "w") as output:
        for i in range(num_flows):
            num_packets_in_flow = flow_rate[i]
            flow_lifetime = duration[i]
            packet_times = np.linspace(arrival_time[i], total_time, num_packets_in_flow)

            # generate packet arrival times within flow_lifetime following Zipf distribution
            indices = zipf.rvs(1.29, size=num_packets_in_flow) - 1
            indices = indices - indices.min()

            max_value = indices.max() if indices.max() != 0 else 0.00001
            indices = indices / max_value
            # f范围确定:0 and 1

            packet_arrival_times = packet_times[0] + indices * (packet_times[-1] - packet_times[0])
            packet_arrival_times = np.ceil(packet_arrival_times).astype(int)  # Round timestamps to nearest integer

            for packet_time in packet_arrival_times:
                output.write(f"{packet_time} {flow_id[i]}\n")




    with open(output_file, 'r') as f:
        lines = f.readlines()

    # 对每一行进行解析，然后按照第一个数字进行排序
    sorted_lines = sorted(lines, key=lambda line: int(line.split()[0]))

    # 将排序后的行写回到同一个文件中
    with open(output_file, 'w') as f:
        for line in sorted_lines:
            f.write(line)


if __name__ == "__main__":
    input_file = "../input_zipf.txt"
    output_file = "../output.txt"
    generate_packets(input_file, output_file)
