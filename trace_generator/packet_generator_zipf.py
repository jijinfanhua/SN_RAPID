import numpy as np
from tqdm import trange


# input format:
#   1. number of packets
#   2. number of flows
#   3. zipf distribution parameter (s)
#   4-4+number of flows:  flow id

# output format:
# arrival time, flow id

def generate_packets_zipf(input_file, output_file):
    print("Generating packets...")
    _input = open(input_file, "r")
    _output = open(output_file, "w")
    num_packets = int(_input.readline())
    num_flows = int(_input.readline())
    zipf_parameter = float(_input.readline())
    flow_id = []
    for i in range(num_flows):
        flow_id.append(int(_input.readline()))
    _input.close()
    # generate packets
    zipf_randoms = np.random.zipf(zipf_parameter, num_packets)
    flow_id = [flow_id[i % num_flows] for i in zipf_randoms]
    for i in trange(num_packets):
        _output.write(str(i) + " " + str(flow_id[i]) + "\n")
    _output.close()
    print("Done!")


if __name__ == "__main__":
    input_file = "input.txt"
    output_file = "output4.txt"
    generate_packets_zipf(input_file, output_file)
