import numpy as np
from tqdm import trange

# input format:
#   1. number of packets
#   2. time
#   3. number of flows
#   4-4+number of flows:  flow id, arrival rate of each flow

# output format:
# arrival time, flow id

def generate_packets(input_file, output_file):
    print("Generating packets...")
    _input = open(input_file, "r")
    _output = open(output_file, "w")
    num_packets = int(_input.readline())
    total_time = int(_input.readline())
    num_flows = int(_input.readline())

    flow_id = []
    flow_rate = []

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
    input_file = "./input_zipf.txt"
    output_file = "./output3.txt"
    generate_packets(input_file, output_file)