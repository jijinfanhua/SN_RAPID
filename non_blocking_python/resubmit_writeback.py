# -*- coding: utf-8 -*-

import collections
import queue
import sys


class Packet:
    def __init__(self, time=0, flow_id=0, is_update=0, sequence_id=0):
        self.data = {
            'time': time, 'flow_id': flow_id, 'is_update': is_update, 'sequence_id': sequence_id
        }


# 从文件里读出数据包
def read_from_file(file_name: str) -> list:
    file_content = []
    pkt_cnt = 0
    with open("./trace/" + file_name, 'r') as f:  # 将 'filename.txt' 替换为你的文件名
        for line in f:
            elements = line.strip().split()
            dic = {'time': int(elements[0]), 'flow_id': int(elements[1]), 'is_update': int(elements[2]),
                   'sequence_id': int(elements[3])}
            index = int(elements[0])
            while len(file_content) <= index:
                file_content.append(None)

            # 在适当的位置插入字典
            file_content[index] = dic
            pkt_cnt += 1

    return pkt_cnt, file_content


# input a packet, return the action&packet output
def writer_state_machine(T: int, packet_in: Packet, dirty_table: list):
    if packet_in is None:
        return 'idle', packet_in
    time = packet_in['time']
    flow_id = packet_in['flow_id']
    is_update = packet_in['is_update']
    sequence_id = packet_in['sequence_id']

    # 回环之前修改 数据包时间戳 time
    def resubmitted():
        return 'resubmitted', packet_in

    def forward():
        return 'forward', packet_in

    def id_up(id):
        return id + 1

    D = T

    for i in dirty_table:
        if i['flow_id'] == flow_id:
            # DIRTY
            if i['dirty'] is True:
                # Time Out
                if i['timer'] <= 0:
                    dirty_table.remove(i)
                    return forward()
                # Wrong Sequence ID
                elif sequence_id != i['expect_id']:
                    i['timer'] = D
                    return resubmitted()
                # Right Sequence ID
                elif sequence_id == i['expect_id']:
                    i['timer'] = D
                    i['dirty'] = False
                    return resubmitted()

            # CLEAN
            elif i['dirty'] is False:
                # Time Out
                if i['timer'] <= 0:
                    dirty_table.remove(i)
                    return forward()
                # Wrong Sequence ID
                elif sequence_id != i['expect_id']:
                    i['timer'] = D
                    return resubmitted()
                # Right Sequence ID
                elif sequence_id == i['expect_id']:
                    # Update State
                    if is_update == 0:
                        i['expect_id'] += 1
                        return forward()
                    # Do not Update State
                    elif is_update == 1:
                        i['expect_id'] += 1
                        i['timer'] = D
                        i['dirty'] = True
                        return forward()

    # IDLE
    if is_update == 1:
        dirty_table.append({'flow_id': flow_id, 'timer': D, 'expect_id': sequence_id + 1, 'dirty': True})
        return forward()
    elif is_update == 0:
        return forward()

    # for i in range(len(dirty_table)):
    #     # hit dirty table -----> return
    #     if flow_id == dirty_table[i]['flow_id']:
    #         # stale state -----> resubmitted
    #         if time < dirty_table[i]['time'] + T:
    #             # file.write("stale state\n")
    #             return resubmitted()
    #         # wrong sequence_id -----> resubmitted
    #         elif sequence_id != dirty_table[i]['expect_id']:
    #             # file.write("wrong sequence_id ")
    #             # file.write(str(dirty_table[i]) + "\n")
    #             return resubmitted()
    #         # right sequence_id -----> forward
    #         elif sequence_id == dirty_table[i]['expect_id']:
    #             dirty_table[i]['expect_id'] = id_up(dirty_table[i]['expect_id'])
    #             # file.write("forward")
    #             return forward()
    #
    # # not hit dirty table but update state -----> update dirty table and forward
    # if is_update == 1:
    #     dirty_table.append({'flow_id': flow_id, 'timer': D, 'expect_id': packet_in['sequence_id'] + 1, 'dirty': True})
    # elif is_update == 0:
    #     pass
    # else:
    #     print('wrong is_update input!')
    # return forward()


# input two packet(resubmitted, pipeline), return the packet sent to Writer
def reader_virtual_queue(pip_packet: Packet, re_packet: Packet, queue: collections.deque, seq_id_cam: list, drop_cnt: list) -> Packet:
    # re exists
    if re_packet is not None:
        if pip_packet is not None:
            if len(queue) < queue.maxlen:
                queue.append(pip_packet)
            else:
                # file_drop.write('queuelen: ' + str(len(queue)) + ' drop packet! ' + str(pip_packet) + '\n')
                drop_cnt[0] += 1
        return re_packet

    # re does not exist
    if len(queue) != 0:
        pkt_out = queue.popleft()
        if pip_packet is not None:
            queue.append(pip_packet)
    elif len(queue) == 0:
        pkt_out = pip_packet

    if pkt_out is None:
        return pkt_out

    for i in range(len(seq_id_cam)):
        if seq_id_cam[i]['flow_id'] == pkt_out['flow_id']:
            pkt_out['sequence_id'] = seq_id_cam[i]['sequence_id']
            seq_id_cam[i]['sequence_id'] += 1
            return pkt_out

    pkt_out['sequence_id'] = 0
    cam_trance = {'flow_id': pkt_out['flow_id'], 'sequence_id': 1}
    seq_id_cam.append(cam_trance)
    return pkt_out


# emulate a virtual pipeline
def virtual_pipeline(T: int, packet_in: Packet, q: queue) -> Packet:
    packet_out = q.get()
    q.put(packet_in)

    return packet_out


def stateful_function(trace_name, writeback_ratio):
    T = 88
    virtual_pip_queue = queue.Queue(maxsize=T)
    for _ in range(T):
        p = None
        virtual_pip_queue.put(p)
    pkt_num, packets = read_from_file(trace_name + "_ratio_" + str(writeback_ratio) + '.txt')
    dirty_table = []
    seq_id_table = []
    reader_queue = collections.deque(maxlen=100000)
    re_packet = None

    re_cnt = 0
    fo_cnt = 0
    queue_sum_everytime = 0
    delay_sum = 0
    queue_sum_arrival = 0
    cycle_cnt_everytime = 0
    cycle_cnt_arrival = 0
    drop_cnt = []
    drop_cnt.append(0)
    j = 0
    update_cnt = 0

    # for j in range(2*len(packets)):
    while j < len(packets) or len(reader_queue) > 0:

        queue_sum_everytime += len(reader_queue)
        cycle_cnt_everytime += 1
        # file.write('reader_queue len: ' + str(len(reader_queue)) + '\n')
        # print(len(reader_queue))

        if j < len(packets):
            i = packets[j]
        else:
            i = None
        packet_in = i
        j += 1

        if i is not None:
            queue_sum_arrival += len(reader_queue)
            cycle_cnt_arrival += 1

        reader = reader_virtual_queue(packet_in, re_packet, reader_queue, seq_id_table, drop_cnt)
        # file.write("pipeline into reader: " + str(packet_in) + '\n')
        # file.write("reader queue: " + str(len(reader_queue)) + '\n')
        virtual_pip = virtual_pipeline(T, reader, virtual_pip_queue)
        # file.write("reader into pipeline: " + str(reader) + '\n')
        # file.write("pipeline into writer: " + str(virtual_pip) + '\n')
        writer = writer_state_machine(T, virtual_pip, dirty_table)
        # file.write("writer action: " + writer[0] + "  " + str(writer[1]) + '\n' + '\n')
        if writer[0] == 'resubmitted':
            re_packet = writer[1]
            re_cnt += 1
        elif writer[0] == 'forward':
            re_packet = None
            fo_cnt += 1
            delay_sum += j - writer[1]['time']
        elif writer[0] == 'idle':
            re_packet = None

        # file_queue_info.write('reader_queue_len: ' + str(len(reader_queue)) + '     pipeline_queue_len: ' + str(virtual_pip_queue.qsize()) + '\n')

    # if fo_cnt == pkt_num:
    #     file.write("Forward Info: all packets have been forwarded" + '\n')
    # else:
    #     file.write("Forward Info: some packets were delayed in the Pipeline" + '\n')
    #     file.write("fo_cnt: " + str(fo_cnt) + '\n')
    #     file.write("pkt_cnt: " + str(pkt_num) + '\n')

    resubmitted_ratio = re_cnt / (pkt_num + re_cnt)

    drop_ratio = drop_cnt[0] / pkt_num

    average_delay = delay_sum / pkt_num

    # file.write("resubmitted ratio: {:.3f}".format(resubmitted_ratio) + '\n')
    # file.write("average arrival delay: {0}".format(average_delay) + '\n')
    # file.write("average everytime length: {}".format(queue_sum_everytime / cycle_cnt_everytime) + '\n')
    # file.write("average arrival length: {}".format(queue_sum_arrival / cycle_cnt_arrival) + '\n')
    # file.write("drop ratio: {}".format(drop_cnt/pkt_num) + '\n')
    # file.write("update_num: " + str(update_cnt))
    print(f"{trace_name} {writeback_ratio} {resubmitted_ratio} {drop_ratio} {average_delay} {queue_sum_everytime / cycle_cnt_everytime} {queue_sum_arrival / cycle_cnt_arrival}\n")
    # file.write(f"{trace_name} {writeback_ratio} {resubmitted_ratio} {drop_ratio} {average_delay} {queue_sum_everytime / cycle_cnt_everytime} {queue_sum_arrival / cycle_cnt_arrival}\n")
    return f"{trace_name} {writeback_ratio} {resubmitted_ratio} {drop_ratio} {average_delay} {queue_sum_everytime / cycle_cnt_everytime} {queue_sum_arrival / cycle_cnt_arrival}\n"
    # for i in packets:
        # file_pkt_input.write(str(i) + '\n')



# file_drop = open("drop_info.txt", "w")
# file_pkt_input = open("pkt_input_info.txt", "w")
# file_queue_info = open("queue_info.txt", "w")
if __name__ == '__main__':
    idx = int(sys.argv[1])
    begin = 101 + idx
    end = begin + 1

    wb_idx = int(sys.argv[2])
    wbs = list(range(5, 101, 5))
    wb_group = wbs[wb_idx::4]

    print(wb_group)

    
    for zipfa in range(begin, end, 1):
        zipfa /= 100
        for writeback_ratio in wb_group:
            for flow_num in range(100, 2001, 100):
                for pkt_num in range(10000, 100001, 10000):
                    file_name = 'syn_' + str(pkt_num) + '_' + str(flow_num) + '_100000_' + str(zipfa)
                    value = stateful_function(file_name, writeback_ratio)
                    file = open(f"result_zipfa_{begin}_wb_{wb_idx}.txt", "a")
                    file.write(value)
                    file.close()

