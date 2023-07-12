from collections import Counter

import numpy as np


def analyze_trace_limit(trace_times, trace_throughput):
    dir_name = 'res_topk_nic_' + str(trace_throughput) + '/'
    file_name = 'baidu_build_trace_' + str(trace_times) + '_NIC_400W_from_5000W_simple_real.txt'

    traces = []
    with open('./' + dir_name + file_name, 'r') as f:
        while True:
            line = f.readline()
            if not line:
                break
            arr = line.split()
            traces.append((int(arr[0]), int(arr[1])))

    # 存储每个流的上一个时间戳、时间间隔之和、以及数据包数量
    flow_info = {}
    # List to hold the averages of all flows
    flow_averages = []

    total_packet_count = 0
    total_interval = 0

    for flow_id, timestamp in traces:
        if flow_id == -1:
            continue
        if flow_id in flow_info:
            last_timestamp, intervals, packet_count, total_flow_interval = flow_info[flow_id]
            # last_timestamp, intervals = flow_info[flow_id]
            interval = timestamp - last_timestamp
            if interval <= 300:
                packet_count += 1
                total_flow_interval += interval
                intervals.append(interval)
            else:
                # Compute the average for the previous flow and add to flow_averages
                if intervals:
                    total_packet_count += packet_count
                    total_interval += total_flow_interval
                    flow_averages.append(sum(intervals) / len(intervals))
                # Start a new flow
                intervals = []
                packet_count = 1
                total_flow_interval = 0
            flow_info[flow_id] = (timestamp, intervals, packet_count, total_flow_interval)
        else:
            flow_info[flow_id] = (timestamp, [], 1, 0)

    # Add the packet count and total interval for the last flow of each ID to the global counters
    for timestamp, intervals, packet_count, total_flow_interval in flow_info.values():
        if intervals:
            total_packet_count += packet_count
            total_interval += total_flow_interval

    # Add the averages for the last flow of each ID
    for last_timestamp, intervals, packet_count, total_flow_interval in flow_info.values():
        if intervals:
            flow_averages.append(sum(intervals) / len(intervals))

    # Calculate the final average
    final_average = sum(flow_averages) / len(flow_averages) if flow_averages else 0

    # 合并所有的间隔到一个列表中
    all_intervals = [interval for last_timestamp, intervals, packet_count, total_flow_interval in flow_info.values() for interval in intervals]

    # 将列表转换为 numpy 数组，以便使用 numpy 的函数
    all_intervals = np.array(all_intervals)

    # 计算统计值
    mean = np.mean(all_intervals)
    median = np.median(all_intervals)
    min_value = np.min(all_intervals)
    max_value = np.max(all_intervals)

    print(f"{trace_times} {trace_throughput} -->>")

    qs = []
    for i in range(1, 10):
        tmp = np.percentile(all_intervals, i * 10)
        qs.append(tmp)
        print(f"{i*10}%分位数：{tmp}")

    q1 = np.percentile(all_intervals, 25)
    q3 = np.percentile(all_intervals, 75)
    q99 = np.percentile(all_intervals, 99)

    print(f"平均数：{mean}")
    print(f"中位数：{median}")
    print(f"最小值：{min_value}")
    print(f"最大值：{max_value}")
    print(f"第一四分位数：{q1}")
    print(f"第三四分位数：{q3}")
    print(f"99%分位数：{q99}")

    # 创建一个新的列表，只包含小于99%分位数的值
    data_filtered = [x for x in all_intervals if x < qs[8]]
    # 计算这个新列表的平均值
    mean_filtered = np.mean(data_filtered)
    print(f"小于90%分位数的平均：{mean_filtered}")

    print(f"计入间隔的数据包个数：{total_packet_count}")


    # 使用 Counter 统计频率
    freq_dict = Counter(all_intervals)
    # 计算列表的长度
    total_count = len(all_intervals)
    # 对键值对进行排序
    sorted_freq = sorted(freq_dict.items())

    # 打印每个数字及其频率百分比
    for num, freq in sorted_freq:
        percentage = round((freq / total_count) * 100, 2)  # 保留两位小数
        print(f"{num}: {percentage}%")

    print("==========================================================")

def analyze_trace(trace_times, trace_throughput):
    dir_name = 'res_topk_nic_' + str(trace_throughput) + '/'
    file_name = 'baidu_build_trace_' + str(trace_times) + '_NIC_400W_from_5000W_simple_real.txt'

    traces = []
    with open('./' + dir_name + file_name, 'r') as f:
        while True:
            line = f.readline()
            if not line:
                break
            arr = line.split()
            traces.append((int(arr[0]), int(arr[1])))

    # 存储每个流的上一个时间戳、时间间隔之和、以及数据包数量
    flow_info = {}
    # List to hold the averages of all flows
    flow_averages = []

    total_packet_count = 0
    total_interval = 0

    # List to hold all intervals
    all_intervals = []

    for flow_id, timestamp in traces:
        if flow_id == -1:
            continue
        if flow_id in flow_info:
            last_timestamp, intervals, packet_count, total_flow_interval = flow_info[flow_id]
            interval = timestamp - last_timestamp
            packet_count += 1
            total_flow_interval += interval
            intervals.append(interval)
            flow_info[flow_id] = (timestamp, intervals, packet_count, total_flow_interval)
        else:
            flow_info[flow_id] = (timestamp, [], 1, 0)

    # Add the packet count and total interval for the last flow of each ID to the global counters
    for timestamp, intervals, packet_count, total_flow_interval in flow_info.values():
        if intervals:
            total_packet_count += packet_count
            total_interval += total_flow_interval
            all_intervals += intervals

    # Calculate the proportion of intervals less than 76
    proportion_less_than_18 = sum(i < 18 for i in all_intervals) / len(all_intervals) if all_intervals else 0
    proportion_less_than_36 = sum(i < 36 for i in all_intervals) / len(all_intervals) if all_intervals else 0
    proportion_less_than_54 = sum(i < 54 for i in all_intervals) / len(all_intervals) if all_intervals else 0
    proportion_less_than_72 = sum(i < 72 for i in all_intervals) / len(all_intervals) if all_intervals else 0

    print(f"{trace_times} {trace_throughput} -->>")
    print(f"Total packet count (included in intervals): {total_packet_count}")
    print(f"Total interval: {total_interval}")
    print(f"Proportion of intervals less than 18: {proportion_less_than_18}")
    print(f"Proportion of intervals less than 36: {proportion_less_than_36}")
    print(f"Proportion of intervals less than 52: {proportion_less_than_54}")
    print(f"Proportion of intervals less than 72: {proportion_less_than_72}")

    print("==========================================================")

for i in range(1, 8):
    analyze_trace(i, i * 14)

def generate_real_trace(trace_times, trace_throughput):
    dir_name = 'res_topk_nic_' + str(trace_throughput) + '/'
    file_name = 'baidu_build_trace_' + str(trace_times) + '_NIC_400W_from_5000W_simple'

    def distribute_points(n, total_positions=100):
        assert 0 < n <= total_positions

        positions = [False] * total_positions

        for i in range(n):
            pos = round(i * total_positions / n)
            positions[pos] = True

        return positions

    trace_list = []
    with open('./' + dir_name + file_name + '.txt', 'r') as f:
        while True:
            line = f.readline()
            if not line:
                break
            trace_list.append(int(line.split()[0]))

    g_clock = 0
    real_trace = []
    trace_cnt = 0
    round_pointer = 0
    sz = len(trace_list)

    positions = distribute_points(trace_throughput)
    for i in range(0, len(positions), 10):
        print(' '.join(map(str, positions[i:i + 10])))

    while trace_cnt < sz:
        if positions[round_pointer]:
            real_trace.append((trace_list[trace_cnt], g_clock))
            g_clock += 1
            trace_cnt += 1
            round_pointer = (round_pointer + 1) % 100
        else:
            real_trace.append((-1, g_clock))
            g_clock += 1
            round_pointer = (round_pointer + 1) % 100

    final_trace_name = './' + dir_name + file_name + '_real.txt'
    with open(final_trace_name, 'w') as g:
        for entry in real_trace:
            g.write('{} {}\n'.format(entry[0], entry[1]))

# for i in range(1, 8):
#     print('generating throughput {} Gbps trace'.format(14 * i))
#     generate_real_trace(i, 14 * i)
#     print('generating trace done')
