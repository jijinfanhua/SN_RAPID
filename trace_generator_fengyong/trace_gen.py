from const import const_list, Consts
import numpy as np
import sys
import os
import json
from test import test_throughput


def get_random_ip():
    _ip = np.random.randint(0, 256, 4, dtype=np.int64)
    while _ip[0] == 0 or _ip[0] > 223 or _ip[0] == 127 or _ip[0] == 128 and _ip[1] == 1:
        _ip = np.random.randint(0, 256, 4, dtype=np.int64)
    return (_ip[0] << 24) + (_ip[1] << 16) + (_ip[2] << 8) + _ip[3]


def get_random_port():
    return np.random.randint(0, 65535)


def get_random_protocol():
    return np.random.randint(0, 255)


class TraceGenerator:
    def __init__(self, const=Consts()):
        self.const = const

        self.sip_buffer = [get_random_ip() for i in range(self.const.sip_num)]
        self.dip_buffer = [get_random_ip() for i in range(self.const.dip_num)]
        self.sport_buffer = [get_random_port() for i in range(self.const.sport_num)]
        self.dport_buffer = [get_random_port() for i in range(self.const.dport_num)]
        self.protocol_buffer = [get_random_protocol() for i in range(self.const.proto_num)]

        self.cnt_pkt_built = 0
        # 每个流的包数
        self.cnt_pkt_of_each_flow = self.get_zif()
        # 到达的每个包的长度
        self.pkt_size_list = self.get_pkt_size()

        # 每个时间片的包
        self.pkt_per_time_slice = [[] for i in range(self.const.time_slice_num)]
        # 每个时间片的总字节数
        self.total_bytes_per_time_slice = [0 for i in range(self.const.time_slice_num)]

    def get_zif(self):
        # read zipf from a generated file
        zipf_a = self.const.zipf_a
        zipf_size = self.const.zipf_size
        # zipf_a = 2
        # zipf_size = 1000
        zipf_file_path = 'zipf_results/zipf_' + str(zipf_a) + '_' + str(zipf_size) + '.json'
        if os.path.exists(path=zipf_file_path):
            f = open(zipf_file_path, 'r')
            return np.array(json.load(f))
        else:
            print("file does not exist, generate new zipf")
            zipf_a = self.const.zipf_a
            zipf_size = self.const.zipf_size
            max_flow_id = self.const.max_flow_id
            zipf_file_path = 'zipf_results/zipf_' + str(zipf_a) + '_' + str(zipf_size) + '.json'
            with open(zipf_file_path, 'w') as f:
                print("generator zipf distribution, a={zipf_a}, size={zipf_size}"
                      .format(zipf_a=zipf_a, zipf_size=zipf_size))
                # the sequence of the arriving flows
                flow_sequence = np.random.zipf(zipf_a, zipf_size)
                # 统计每个流出现的次数 id为1的流最多 有些流的id可能会很大，但是出现次数很少，因此只保留max_flow_id之前的流
                count_each_flow_num = np.bincount(flow_sequence[flow_sequence < max_flow_id])
                count_each_flow_num[::-1].sort()
                json.dump(count_each_flow_num.tolist(), f)
                print("zipf distribution generated")
                return count_each_flow_num

    def get_pkt_size(self):
        # x1 中含有N1个包 x2中含有N2个包 x4中含有N3个包，N1 + N2 = pkt_num_per_second，将三者合在一起， 并将包长限制在64到1500之内
        x1 = np.random.normal(self.const.mu1, self.const.sigma1, self.const.N1)
        if self.const.N2 < 0 or self.const.N2 > self.const.pkt_num_per_second:
            raise ValueError("参数配置有误，fraction2不在0到1之间")
        x2 = np.random.normal(self.const.mu2, self.const.sigma2, self.const.N2)

        x3 = np.random.normal(self.const.mu3, self.const.sigma3, self.const.N3)
        _pkt_size_list = np.ceil(np.concatenate([x1, x2, x3]))
        np.random.shuffle(_pkt_size_list)
        for i in range(len(_pkt_size_list)):
            if _pkt_size_list[i] < 64:
                _pkt_size_list[i] = 64
            elif _pkt_size_list[i] > 1500:
                _pkt_size_list[i] = 1500
        return _pkt_size_list

    def get_random_tuple5(self):
        return (int(self.sip_buffer[np.random.randint(self.const.sip_num)]) << 72) + (
                int(self.dip_buffer[np.random.randint(self.const.dip_num)]) << 40) + (
                int(self.sport_buffer[np.random.randint(self.const.sport_num)]) << 24) + (
                int(self.dport_buffer[np.random.randint(self.const.dport_num)]) << 8) + (
            int(self.protocol_buffer[np.random.randint(self.const.proto_num)]))

    # 为某流的包分配时间片, flow_pkt_num: 该流的数据包个数， begin time now: 分配的起始时间
    def set_time(self, tuple5, flow_pkt_num, flow_begin_time_now):
        time_slice_remain = self.const.time_slice_num - flow_begin_time_now
        # 该流分配的总时间片范围
        if time_slice_remain > flow_pkt_num * self.const.min_send_space:

            flow_total_time = int(np.random.randint(flow_pkt_num * self.const.min_send_space,
                                                    min(flow_pkt_num * self.const.max_send_space, time_slice_remain)))
        elif time_slice_remain < flow_pkt_num / 2:  # 剩余时间片不够，从头开始分配
            return True
        else:
            flow_total_time = time_slice_remain

        # 指数分布的时间间隔
        time_space_list = np.floor(np.random.exponential(scale=flow_total_time / flow_pkt_num, size=flow_pkt_num))
        time = flow_begin_time_now
        for time_space in time_space_list:
            pkt_len = int(self.pkt_size_list[self.cnt_pkt_built])
            self.pkt_per_time_slice[time].append((tuple5, pkt_len))
            self.total_bytes_per_time_slice[time] += pkt_len
            time += int(time_space)
            self.cnt_pkt_built += 1
            if time >= self.const.time_slice_num:
                return

    # 将该流的包分配到每个时间片内
    def distribute_packet(self):
        begin_time_now = 0
        # 每个时间片分配字节数的阈值
        time_slice_bytes_threshold = self.const.min_threshold_byte_size_of_each_slice

        for i in range(len(self.cnt_pkt_of_each_flow)):
            if self.cnt_pkt_of_each_flow[i] <= 1:
                break
            tuple5 = self.get_random_tuple5()
            if self.set_time(tuple5, int(self.cnt_pkt_of_each_flow[i]), begin_time_now):
                i -= 1
                begin_time_now = 0
                time_slice_bytes_threshold += self.const.min_threshold_byte_size_of_each_slice
            # 找到一个合适的下一个begin time时间，来进行数据包的分配
            # 在接下来的stupid space时间内的每个时间片内，每个时间片都没有分配多于threshold的数据量
            # 这样保证在对一次时间片的遍历过程中，所有包的吞吐能够均匀分配到每个时间片内
            while begin_time_now < self.const.time_slice_num:
                find_next_begin_time = True
                for j in range(self.const.stupid_space):
                    next_time = begin_time_now + j
                    if next_time < self.const.time_slice_num and \
                            self.total_bytes_per_time_slice[next_time] > time_slice_bytes_threshold:
                        begin_time_now = next_time + 1
                        find_next_begin_time = False
                        break
                if find_next_begin_time:
                    break
            # 如果没找到合适的begin time，则增加per_size_min，从头开始找时间片来分配该流的包
            if begin_time_now + self.const.stupid_space >= self.const.time_slice_num:
                time_slice_bytes_threshold += self.const.min_threshold_byte_size_of_each_slice
                begin_time_now = 0

    # 将每个时间片的包分配一个确定的时间，写入文件
    def trace_write(self, file_path):
        total_size = 0
        time_now = 0
        time_remain = 0
        with open(file_path, 'w') as f:
            print("write file")
            for i in range(const.time_slice_num):
                total_size += self.total_bytes_per_time_slice[i]

                # 分配一个小流的包
                while total_size < self.const.bytes_per_time_slice * (i + 1):
                    # self.cnt_pkt_built < len(self.pkt_size_list):
                    tuple5 = self.get_random_tuple5()
                    self.pkt_per_time_slice[i].append((tuple5, self.pkt_size_list[self.cnt_pkt_built]))
                    self.total_bytes_per_time_slice[i] += self.pkt_size_list[self.cnt_pkt_built]
                    total_size += self.pkt_size_list[self.cnt_pkt_built]
                    self.cnt_pkt_built += 1

                for pkt_info in self.pkt_per_time_slice[i]:
                    tuple5 = pkt_info[0]
                    pkt_len = int(pkt_info[1])
                    f.write(str(time_now) + ' ' + str(tuple5) + ' ' + str(pkt_len) + '\n')
                    time_space = np.random.uniform(time_remain + self.const.space_per_bytes * pkt_len)
                    time_now = time_now + self.const.time_per_bytes * pkt_len + time_space
                    time_remain = time_remain + self.const.space_per_bytes * pkt_len - time_space

    def gen_trace(self, filepath):
        # write trace to file
        self.distribute_packet()
        self.trace_write(filepath)
        print("trace generated")


port_num = 16

if __name__ == "__main__":
    sip_buffer = []
    dip_buffer = []
    sport_buffer = []
    dport_buffer = []
    protocol_buffer = []
    cnt_port = 0
    for const in const_list:
        if cnt_port >= port_num:
            break
        filename = "./trace/port_" + str(cnt_port) + ".txt"
        output_filename = "./trace_info/port_" + str(cnt_port) + ".txt"
        trace_generator = TraceGenerator(const)
        print(trace_generator.const.__dict__)
        trace_generator.gen_trace(filename)
        test_throughput(filename, trace_generator.const.throughput, output_filename)
        cnt_port += 1
