import numpy as np
from tqdm import trange
import sys

def generate_packets(input_file, output_file):
    print("Generating packets...")
    _input = open(input_file, "r")
    _output = open(output_file, "w")
    num_packets = int(_input.readline())
    num_flows = int(_input.readline())
    total_time = int(_input.readline())
    flow_id = []
    flow_rate = []
    # 存储流的id和流的速率
    for i in range(num_flows):
        tmp= _input.readline().split()
        flow_id.append(int(tmp[0]))
        flow_rate.append(int(tmp[1]))
    _input.close()
    # generate packets
    total_rate = np.sum(flow_rate)
    flow_ratio = [rate / total_rate for rate in flow_rate]
    uniform_randoms = np.random.choice(total_time + 1, num_packets, replace=False)
    arrival_time = np.sort(uniform_randoms)
    flow_id = np.random.choice(flow_id, num_packets, p=flow_ratio)
    for i in trange(num_packets):
        _output.write(str(arrival_time[i]) + " " + str(flow_id[i]) + "\n")
    _output.close()
    print("Done!")


if __name__ == "__main__":
    # input_file = os.path.join(os.getcwd(), "input.txt")
    # output_file = os.path.join(os.getcwd(), "output.txt")
    input_file = sys.argv[1]
    # input_file = "./input_10000_1000_100000_1.01.txt"

    output_file = sys.argv[2]
    # output_file = "syn_10000_1000_100000_1.01.txt"

    generate_packets(input_file, output_file)
