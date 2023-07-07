import math
from collections import deque

packets = []

with open("./trace/nic/baidu_build_trace_2_NIC_400W_from_5000W_simple.txt", 'r') as f: #
    while True: #
        line = f.readline()
        if not line:
            break
        arr = line.split()
        packets.append(int(arr[0]))

record_f = open("./res/nic_28_22_new_find_schedule_result.txt", 'a')


def distribute_points(n, total_positions=100):
    assert 0 < n <= total_positions, "The number of packets should be between 1 and total_positions."

    positions = [False] * total_positions

    for pkt in range(n):
        pos = round(pkt * total_positions / n)
        positions[pos] = True

    for i in range(0, len(positions), 10):
        print(positions[i:i + 10])

    return positions


def blocking_scheme(N=4, PIPE_LEN=76, throughput=20):
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

    while pkt_num > 0 or any(q for q in queues if q):  # 任何一个queue不空就需要走下去

        # 每1个包加两个气泡
        if dp[round_pointer]:
            if pkt_num > 0:
                cur = packets[pkt_idx] % N
                if len(queues[cur]) < maxlen:
                    queues[cur].append(packets[pkt_idx])
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
                if head in dirty_cam:
                    pass
                else:
                    queues[queue_pointer].popleft()
                    dirty_cam[head] = g_clock
                    break
                queue_pointer = (queue_pointer + 1) % N
                # break
            else:
                queue_pointer = (queue_pointer + 1) % N

        # queue_pointer = (queue_pointer + 1) % N

        # 更新dirty cam中够时间的流，删除
        dirty_cam = {key: value for key, value in dirty_cam.items() if ((g_clock - value) != PIPE_LEN)}

        g_clock += 1

    print(pkt_drop_num)
    record_f.write('pipelen: {} N: {} drop_start: {} packet_drop_num: {}\n'.format(PIPE_LEN, N, flag, pkt_drop_num))


queue_nums = [128] #[1, 2, 4, 8, 16, 32, 64]
for i in queue_nums:
    for j in range(1, 9):
        print('N=', i, ', j=', j, ' running!')
        blocking_scheme(N=i, PIPE_LEN=(j * 21 + j), throughput=28)
        print('N=', i, ', j=', j, ' end!\n----------------------')

record_f.close()
