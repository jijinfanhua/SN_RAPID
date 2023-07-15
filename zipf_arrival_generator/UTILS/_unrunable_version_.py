# import numpy as np
# from tqdm import trange
# from scipy.stats import zipf
#
# def generate_packets(input_file, output_file):
#     print("Generating packets...")
#     _input = open(input_file, "r")
#     _output = open(output_file, "w")
#     num_packets = int(_input.readline())
#     num_flows = int(_input.readline())
#     total_time = int(_input.readline())
#
#     flow_id = []
#     flow_rate = []
#     for i in range(num_flows):
#         tmp= _input.readline().split()
#         flow_id.append(int(tmp[0]))
#         flow_rate.append(int(tmp[1]))
#     _input.close()
#
#     # generate flows' arrival time
#     mean = total_time / 2
#     std_dev = total_time / 4
#     arrival_time = np.random.normal(loc=mean, scale=std_dev, size=num_flows)
#     arrival_time = arrival_time.clip(0, total_time)  # adjust the range within [0, total_time]
#     arrival_time.sort()
#
#     total_packets = []
#     for i in trange(num_flows):
#         duration = total_time - arrival_time[i]
#         num_segments = 1000
#         segment_duration = duration / num_segments
#         packets_in_each_segment = zipf.rvs(1.29, size=num_segments)
#         packets_in_each_segment.sort()
#         packets_in_each_segment = packets_in_each_segment[::-1]  # descending order
#
#         flow_packets = [(arrival_time[i] + segment_duration*j, flow_id[i]) for j in range(num_segments) for _ in range(packets_in_each_segment[j])]
#         total_packets.extend(flow_packets)
#     # We may generate more packets than num_packets, just keep the first num_packets
#     total_packets = sorted(total_packets)[:num_packets]
#     for packet in total_packets:
#         _output.write(str(packet[0]) + " " + str(packet[1]) + "\n")
#
#     _output.close()
#     print("Done!")
#
# if __name__ == "__main__":
#     input_file = "./input_zipf.txt"
#     output_file = "./output3_2292.txt"
#     generate_packets(input_file, output_file)

import numpy as np
from tqdm import trange
from scipy.stats import zipf

def generate_packets(input_file, output_file):
    print("Generating packets...")
    _input = open(input_file, "r")
    _output = open(output_file, "w")
    num_packets = int(_input.readline())
    num_flows = int(_input.readline())
    total_time = int(_input.readline())


    flow_id = []
    flow_rate = []
    for i in range(num_flows):
        tmp= _input.readline().split()
        flow_id.append(int(tmp[0]))
        flow_rate.append(int(tmp[1]))
    _input.close()

    # generate flows' arrival time
    mean = total_time / 2
    std_dev = total_time / 4
    arrival_time = np.random.normal(loc=mean, scale=std_dev, size=num_flows)
    arrival_time = arrival_time.clip(0, total_time)  # adjust the range within [0, total_time]
    arrival_time.sort()

    total_packets = []
    for i in range(num_flows):
        duration = total_time - arrival_time[i]
        num_segments = int(duration)

        packets_in_each_segment = zipf.rvs(1.29, size=num_segments)
        packets_in_each_segment.sort()
        packets_in_each_segment = packets_in_each_segment[::-1]  # descending order

        # Scale packet numbers to be proportional to flow_rate
        packets_in_each_segment = (packets_in_each_segment / np.sum(packets_in_each_segment)) * flow_rate[i]
        packets_in_each_segment = np.round(packets_in_each_segment).astype(int)

        segment_duration = duration / num_segments
        flow_packets = [(arrival_time[i] + segment_duration*j, flow_id[i]) for j in range(num_segments) if packets_in_each_segment[j] > 0 for _ in range(packets_in_each_segment[j])]
        total_packets.extend(flow_packets)

    total_packets.sort()

    # We may generate more packets than num_packets, just keep the first num_packets
    total_packets = total_packets[:num_packets]

    for packet in total_packets:
        _output.write(str(packet[0]) + " " + str(packet[1]) + "\n")

    _output.close()
    print("Done!")

if __name__ == "__main__":
    input_file = "./input_zipf.txt"
    output_file = "./output3_2292.txt"
    generate_packets(input_file, output_file)
#