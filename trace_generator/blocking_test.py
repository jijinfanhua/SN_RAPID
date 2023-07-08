import math
from collections import deque

packets = []

with open("packet_sequence_info.txt", 'r') as f:  #
    while True:
        line = f.readline()
        if not line:
            break
        arr = line.split()
        packets.append([int(arr[1]), 0, 0, 0, int(arr[0])])

record_f = open("whether_packet_drop.txt", 'w')


# 1. 每个包的等待时间（从到达到服务）或者滞留时间（从到达到处理完）
# 2. 每个包入队时刻的队长（不包括正在服务的包）pkt[1]
# 3. 每个包入队时刻队列中与这个包属于同一个的流包的数量
def blocking_scheme(N=4, PIPE_LEN=76):
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
    drop_flag = 0

    if_smart = 0

    packet_f = open('record1_1.txt', 'w')

    while pkt_num > 0 or any(q for q in queues if q):  # 任何一个queue不空就需要走下去

        if pkt_num > 0:
            if pkt_num % 1000000 == 0:
                print(f"pkt_num: {pkt_num}")
            if packets[pkt_idx][4] == g_clock:
                cur = packets[pkt_idx][0] % N
                if len(queues[cur]) < maxlen:
                    pkt = packets[pkt_idx]
                    pkt[1] = len(queues[cur])
                    pkt[2] = g_clock
                    flow_pkt_num = 0
                    for tmp in queues[cur]:
                        if tmp[0] == pkt[0]:
                            flow_pkt_num += 1
                    pkt[3] = flow_pkt_num
                    queues[cur].append(pkt)
                else:
                    drop_flag = 1
                    pkt_drop_num += 1
                pkt_idx += 1
                pkt_num -= 1
            else:
                pass

        # 如果dirty cam中有这个流，则阻塞，不能调度，指针下移；
        # 不轮询

        if if_smart:
            for i in range(0, N):
                queue_pointer = (queue_pointer + 1) % N
                if len(queues[queue_pointer]) > 0:
                    head = queues[queue_pointer][0]
                    if head[0] in dirty_cam:
                        pass
                    else:
                        pkt = queues[queue_pointer][0]
                        pkt[2] = g_clock - pkt[2]
                        packet_f.write(f"{pkt[0]} {pkt[1]} {pkt[2]} {pkt[3]} {g_clock}\n")
                        queues[queue_pointer].popleft()
                        dirty_cam[pkt[0]] = g_clock
                        queue_pointer = (queue_pointer + 1) % N
                        break
        else:  # here
            if len(queues[queue_pointer]) > 0:
                head = queues[queue_pointer][0]
                if head[0] in dirty_cam:
                    pass
                else:
                    pkt = queues[queue_pointer][0]
                    pkt[2] = g_clock - pkt[2]
                    # print(g_clock)
                    packet_f.write(f"{pkt[0]} {pkt[1]} {pkt[2]} {pkt[3]} {g_clock}\n")
                    queues[queue_pointer].popleft()
                    dirty_cam[pkt[0]] = g_clock
            queue_pointer = (queue_pointer + 1) % N

        # 更新dirty cam中够时间的流，删除
        dirty_cam = {key: value for key, value in dirty_cam.items() if ((g_clock - value) != PIPE_LEN)}

        g_clock += 1

    packet_f.close()

    print(pkt_drop_num)
    if drop_flag == 0:
        record_f.write('pipelen: {} N: {} drop_start: {} packet_drop_num: {}\n'.format(PIPE_LEN, N, -1, pkt_drop_num))
        return
    else:
        record_f.write('pipelen: {} N: {} drop_start: {} packet_drop_num: {}\n'.format(PIPE_LEN, N, 1, pkt_drop_num))
        return


# 使用的队列数量  *注* 所有队列加在一起大小为128，可在程序中调整128的值
QUEUE_NUM = 1
# 使用的processor数量，每个processor 19个cycle
PIPE_LEN = 4

# ** 改变上面三个变量的值，即可修改实验场景 **
blocking_scheme(QUEUE_NUM, PIPE_LEN * 19)

record_f.close()