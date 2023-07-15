import numpy as np

# poisson

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
        tmp = _input.readline().split()
        flow_id.append(int(tmp[0]))
        flow_rate.append(int(tmp[1]))
    _input.close()

    # code above is copied from other generator

    # 用流的比率确定数据包的到达时间
    total_rate = np.sum(flow_rate)
    flow_ratio = [rate / total_rate for rate in flow_rate]
    # 流的比率是流的速率 / 总的速率
    arrival_time = []
    # 生成到达时间
    while len(arrival_time) < num_packets:
        for i in range(num_flows):
            # generate Poisson arrival time for each flow
            rate = flow_rate[i]
            # print(rate)
            # 参数是流的速率,生成数据包的数量是sie,放到arrival里面去
            arrival_time.extend(np.random.poisson(rate, size=int(flow_ratio[i] * num_packets)))

    # 生成的数据包的数量不能够超过num_packets
    # 转成npy,然后排序,然后取前n_p个元素
    arrival_time = np.sort(np.array(arrival_time)[:num_packets])

    # 不超过总时间
    arrival_time = (arrival_time / max(arrival_time) * total_time).astype(int)

    # 根据流的比率给数据包分配流id,然后把到达时间和对应的流id写出去
    flow_ids_assigned = np.random.choice(flow_id, num_packets, p=flow_ratio)
    for i in range(num_packets):
        _output.write(str(arrival_time[i]) + " " + str(flow_ids_assigned[i]) + "\n")
    _output.close()
    print("Done!")


if __name__ == "__main__":
    input_file = "./input_zipf.txt"
    output_file = "./output.txt"
    generate_packets(input_file, output_file)
