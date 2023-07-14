import matplotlib.pyplot as plt
import numpy as np
from tqdm import trange
import sys

def generate_packet(input_file,output_file):
    print("Generatign packets...")
    input = open(input_file,'r')
    output = open(output_file,'w')
    num_packets = input.readline()
    num_flow = int(input.readline())
    time = input.readline()

    # 把有多少包,看作出现的频率处理
    flow_id = []
    flow_rate = []
    for i in range(num_flow):
        tmp = input.readline().split()
        flow_id.append(int(tmp[0]))
        flow_rate.append(int(tmp[1]))
        # print(f"{flow_id[i]} {flow_rate[i]}")
    input.close()




    print("Done!")



input_file = sys.argv[1]
output_file = sys.argv[2]

generate_packet(input_file,output_file)
