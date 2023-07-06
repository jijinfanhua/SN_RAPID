import math
from collections import deque


def get_trace(trace_name):
    packets = []
    with open(trace_name, 'r') as f:  #
        while True:  #
            line = f.readline()
            if not line:
                break
            arr = line.split()
            packets.append([int(arr[0]), 0, 0])
    return packets


def distribute_points(n, total_positions=100):
    assert 0 < n <= total_positions, "The number of packets should be between 1 and total_positions."

    positions = [False] * total_positions

    for pkt in range(n):
        pos = round(pkt * total_positions / n)
        positions[pos] = True

    for i in range(0, len(positions), 10):
        print(positions[i:i + 10])

    return positions


def blocking_scheme(N=4, PIPE_LEN=76, throughput=20, packets=None, record_f=None, ):
    # 使用列表推导式创建 N 个队列
    maxlen = math.ceil(128 / N)
    queues = [deque(maxlen=maxlen) for _ in range(N)]
    g_clock = 0  # 全局时钟复位
    dirty_cam = {}  # 建立空dirty cam
    pkt_num = len(packets)
    pkt_idx = 0  # 从0开始取pkt
    queue_pointer = 0  # 轮询指针

    print(f"packet num: {pkt_num}")

    pkt_drop_num = 0

    flag = 0

    dp = distribute_points(throughput)
    round_pointer = 0

    queuing_latency = []
    queue_lengths = []

    avg_latency = 0
    avg_queue_length = 0

    while pkt_num > 0 or any(q for q in queues if q):  # 任何一个queue不空就需要走下去

        # 每1个包加两个气泡
        if dp[round_pointer]:
            if pkt_num > 0:
                cur = packets[pkt_idx][0] % N
                if len(queues[cur]) < maxlen:
                    pkt = packets[pkt_idx]
                    pkt[1] = g_clock
                    pkt[2] = len(queues[cur])
                    queues[cur].append(pkt)
                else:
                    flag = 1
                    pkt_drop_num += 1
                pkt_idx += 1
                pkt_num -= 1
            else:
                pass
            round_pointer = (round_pointer + 1) % 100
        else:
            round_pointer = (round_pointer + 1) % 100
            pass

        # 如果dirty cam中有这个流，则阻塞，不能调度，指针下移；
        # 不轮询
        for i in range(0, N):
            if len(queues[queue_pointer]) > 0:
                head = queues[queue_pointer][0]
                if head[0] in dirty_cam:
                    pass
                else:
                    latency_queuing = g_clock - head[1] + PIPE_LEN
                    # print(latency_queuing)
                    queuing_latency.append(latency_queuing)
                    queue_lengths.append(head[2])
                    queues[queue_pointer].popleft()
                    dirty_cam[head[0]] = g_clock
                    break
                queue_pointer = (queue_pointer + 1) % N
                # break
            else:
                queue_pointer = (queue_pointer + 1) % N

        # queue_pointer = (queue_pointer + 1) % N

        # 更新dirty cam中够时间的流，删除
        dirty_cam = {key: value for key, value in dirty_cam.items() if ((g_clock - value) != PIPE_LEN)}

        g_clock += 1

    avg_latency = sum(queuing_latency) / len(queuing_latency)
    avg_queue_length = sum(queue_lengths) / len(queue_lengths)

    print(pkt_drop_num)
    record_f.write('pipelen: {} N: {} drop_start: {} packet_drop_num: {} average_latency: {} queue_length: {}\n'
                   .format(PIPE_LEN, N, flag, pkt_drop_num, avg_latency, avg_queue_length))


"""
    TRACE_FILE_NAME：需要处理的trace的路径（nic为网卡，switch为交换机）；
    RES_FILE_NAME：保存所有结果的文件路径；
    queue_nums：指定需要实验哪些队列数量；
    PIPE_LEN：指定流水线长度（处理器为单位），每个处理器为22硬件周期
"""
if __name__ == "__main__":
    TRACE_FILE_NAME = "./trace/nic/baidu_build_trace_2_NIC_400W_from_5000W_simple.txt"
    RES_FILE_NAME = "./res/nic_28_22_new_find_schedule_result.txt"

    packets = get_trace(TRACE_FILE_NAME)

    record_f = open(RES_FILE_NAME, 'w')

    queue_nums = [8]  # , 2, 4, 8, 16, 32, 64]
    for i in queue_nums:
        for j in range(4, 5):
            print('N=', i, ', j=', j, ' running!')
            blocking_scheme(N=i, PIPE_LEN=(j * 21 + j), throughput=28, packets=packets, record_f=record_f)
            print('N=', i, ', j=', j, ' end!\n----------------------')

    record_f.close()
