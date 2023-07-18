import math
from collections import deque

def get_packets(trace_name):
    packets = []

    with open('../Trace/' + trace_name + '.txt', 'r') as f:  # todo: 修改
        while True:
            line = f.readline()
            if not line:
                break
            arr = line.split()
            packets.append([int(arr[1]), 0, 0, 0, int(arr[0])])
    return packets

# 1. 每个包的等待时间（从到达到服务）或者滞留时间（从到达到处理完）
# 2. 每个包入队时刻的队长（不包括正在服务的包）pkt[1]
# 3. 每个包入队时刻队列中与这个包属于同一个的流包的数量
def blocking_scheme_find_next(trace_name, N=4, PIPE_LEN=88, packets=None):
    # 使用列表推导式创建 N 个队列
    maxlen = math.ceil(128 / N)
    queues = [deque(maxlen=maxlen) for _ in range(N)]
    g_clock = 0  # 全局时钟复位
    dirty_cam = {}  # 建立空dirty cam
    pkt_num = len(packets)
    pkt_idx = 0  # 从0开始取pkt
    queue_pointer = 0  # 轮询指针

    print(f"find_next: {trace_name}, packet num: {pkt_num}")

    pkt_drop_num = 0
    drop_flag = 0

    queuing_latency = []
    packet_queue_lengths = []
    clock_queue_lengths = []

    # packet_f = open('./record1_1.txt', 'w')

    while pkt_num > 0 or any(q for q in queues if q):  # 任何一个queue不空就需要走下去

        if pkt_num > 0:
            if packets[pkt_idx][4] == g_clock:
                cur = packets[pkt_idx][0] % N
                if len(queues[cur]) < maxlen:
                    pkt = packets[pkt_idx]

                    queue_len = 0
                    for i in range(0, N):
                        queue_len += len(queues[i])

                    pkt[1] = queue_len # 当前队列长度
                    pkt[2] = g_clock # 入队时钟
                    flow_pkt_num = 0
                    queues[cur].append(pkt)
                else:
                    drop_flag = 1
                    pkt_drop_num += 1
                pkt_idx += 1
                pkt_num -= 1
            else:
                pass

        queue_len = 0
        if pkt_num > 0:
            for i in range(0, N):
                queue_len += len(queues[i])
        clock_queue_lengths.append(queue_len)

        # 如果dirty cam中有这个流，则阻塞，不能调度，指针下移；
        # 不轮询

        for i in range(0, N):
            queue_pointer = (queue_pointer + 1) % N
            if len(queues[queue_pointer]) > 0:
                head = queues[queue_pointer][0]
                if head[0] in dirty_cam:
                    break
                else:
                    latency_queuing = g_clock - head[2] + PIPE_LEN

                    queuing_latency.append(latency_queuing)
                    packet_queue_lengths.append(head[1])

                    queues[queue_pointer].popleft()
                    dirty_cam[head[0]] = g_clock
                    # queue_pointer = (queue_pointer + 1) % N
                    break
            else:
                pass

        # 更新dirty cam中够时间的流，删除
        dirty_cam = {key: value for key, value in dirty_cam.items() if ((g_clock - value) != PIPE_LEN)}

        g_clock += 1

    # packet_f.close()

    avg_latency = sum(queuing_latency) / len(queuing_latency)
    avg_packet_queue_length = sum(packet_queue_lengths) / len(packet_queue_lengths)
    avg_clock_queue_length = sum(clock_queue_lengths) / len(clock_queue_lengths)

    print(f"packet drop: {pkt_drop_num}")
    record_f = open("./blocking_next_result_burst_multi_big.txt", 'a')
    record_f.write(f'{trace_name} {PIPE_LEN}, {N}, {drop_flag}, {pkt_drop_num/len(packets)}, {avg_latency}, {avg_packet_queue_length}, {avg_clock_queue_length}\n')
    record_f.close()


def blocking_scheme_find_schedule(trace_name, N=4, PIPE_LEN=88, packets=None):
    # 使用列表推导式创建 N 个队列
    maxlen = math.ceil(128 / N)
    queues = [deque(maxlen=maxlen) for _ in range(N)]
    g_clock = 0  # 全局时钟复位
    dirty_cam = {}  # 建立空dirty cam
    pkt_num = len(packets)
    pkt_idx = 0  # 从0开始取pkt
    queue_pointer = 0  # 轮询指针

    print(f"find_schedule: {trace_name}, packet num: {pkt_num}")

    pkt_drop_num = 0
    drop_flag = 0

    queuing_latency = []
    packet_queue_lengths = []
    clock_queue_lengths = []

    # packet_f = open('./record1_1.txt', 'w')

    while pkt_num > 0 or any(q for q in queues if q):  # 任何一个queue不空就需要走下去

        if pkt_num > 0:
            if packets[pkt_idx][4] == g_clock:
                cur = packets[pkt_idx][0] % N
                if len(queues[cur]) < maxlen:
                    pkt = packets[pkt_idx]

                    queue_len = 0
                    for i in range(0, N):
                        queue_len += len(queues[i])

                    pkt[1] = queue_len # 当前队列长度
                    pkt[2] = g_clock # 入队时钟
                    flow_pkt_num = 0
                    queues[cur].append(pkt)
                else:
                    drop_flag = 1
                    pkt_drop_num += 1
                pkt_idx += 1
                pkt_num -= 1
            else:
                pass

        queue_len = 0
        if pkt_num > 0:
            for i in range(0, N):
                queue_len += len(queues[i])
        clock_queue_lengths.append(queue_len)

        # 如果dirty cam中有这个流，则阻塞，不能调度，指针下移；
        # 不轮询

        for i in range(0, N):
            queue_pointer = (queue_pointer + 1) % N
            if len(queues[queue_pointer]) > 0:
                head = queues[queue_pointer][0]
                if head[0] in dirty_cam:
                    pass
                else:
                    latency_queuing = g_clock - head[2] + PIPE_LEN

                    queuing_latency.append(latency_queuing)
                    packet_queue_lengths.append(head[1])

                    queues[queue_pointer].popleft()
                    dirty_cam[head[0]] = g_clock
                    # queue_pointer = (queue_pointer + 1) % N
                    break
            else:
                pass

        # 更新dirty cam中够时间的流，删除
        dirty_cam = {key: value for key, value in dirty_cam.items() if ((g_clock - value) != PIPE_LEN)}

        g_clock += 1

    # packet_f.close()

    avg_latency = sum(queuing_latency) / len(queuing_latency)
    avg_packet_queue_length = sum(packet_queue_lengths) / len(packet_queue_lengths)
    avg_clock_queue_length = sum(clock_queue_lengths) / len(clock_queue_lengths)

    print(f"packet drop: {pkt_drop_num}")
    record_f = open("./blocking_schedule_result_burst_multi_big.txt", 'a')
    record_f.write(f'{trace_name} {PIPE_LEN}, {N}, {drop_flag}, {pkt_drop_num / len(packets)}, {avg_latency}, {avg_packet_queue_length}, {avg_clock_queue_length}\n')
    record_f.close()


def run_blocking(trace_name, queue_num, pipe_len):
    packets = get_packets(trace_name)
    # blocking_scheme_find_next(trace_name, queue_num, pipe_len * 22, packets)
    blocking_scheme_find_schedule(trace_name, queue_num, pipe_len * 22, packets)


for packet_num in range(10000, 100001, 10000): # todo：名字
    for flow_num in range(200, 2001, 200):
        cycle_num = 100000
        for zipfa in [round(i * 0.01, 2) for i in range(101, 111)] + [1.2]:
            for burst in [i * 0.5 for i in range(1, 7)]:
                for queue_num in [1, 2, 4, 8, 16, 32]:
                    trace_name = f"output_{packet_num}_{flow_num}_{cycle_num}_{zipfa}_{burst}"
                    run_blocking(trace_name, queue_num, 4)