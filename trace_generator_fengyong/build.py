from constant import const

import numpy as np
import sys
import os
import json


def get_zif():
    print(const.zipf_a, const.zipf_size)
    zipf_file_path = './zipf_results/zipf_' + str(const.zipf_a) + '_' + str(const.zipf_size) + '.json'
    if os.path.exists(path=zipf_file_path):
        f = open(zipf_file_path, 'r')
        return np.array(json.load(f))
    else:
        print("file does not exist")


def get_pkt_size():
    # x1 中含有N1个包 x2中含有N2个包 x4中含有N3个包，N1 + N2 = pkt_num_per_second，将三者合在一起， 并将包长限制在64到1500之内
    x1 = np.random.normal(const.mu1, const.sigma1, const.N1)
    x2 = np.random.normal(const.mu2, const.sigma2, const.N2)
    x3 = np.random.normal(const.mu3, const.sigma3, const.N3)
    _pkt_size_list = np.ceil(np.concatenate([x1, x2, x3]))
    np.random.shuffle(_pkt_size_list)
    for i in range(len(_pkt_size_list)):
        if _pkt_size_list[i] < 64:
            _pkt_size_list[i] = 64
        elif _pkt_size_list[i] > 1500:
            _pkt_size_list[i] = 1500

    return _pkt_size_list


def get_random_ip():
    _ip = np.random.randint(0, 256, 4, dtype=np.int64)
    while _ip[0] == 0 or _ip[0] > 223 or _ip[0] == 127 or _ip[0] == 128 and _ip[1] == 1:
        _ip = np.random.randint(0, 256, 4, dtype=np.int64)
    return (_ip[0] << 24) + (_ip[1] << 16) + (_ip[2] << 8) + _ip[3]


def get_random_port():
    return np.random.randint(0, 65535)


def get_random_protocol():
    return np.random.randint(0,255)


def get_random_tuple5():
    return (int(sip_buffer[np.random.randint(const.sip_num)]) << 65) + (
            int(dip_buffer[np.random.randint(const.dip_num)]) << 33) + (
                   int(sport_buffer[np.random.randint(const.sport_num)]) << 17) + (
                   int(dport_buffer[np.random.randint(const.dport_num)]) << 1) + int(np.random.randint(2))


def set_buffer():
    for i in range(const.sip_num):
        sip_buffer.append(get_random_ip())
    for i in range(const.dip_num):
        dip_buffer.append(get_random_ip())
    for i in range(const.sport_num):
        sport_buffer.append(get_random_port())
    for i in range(const.dport_num):
        dport_buffer.append(get_random_port())


def set_time(_tuple5, _num, _pkt_size_list, _pkt_pre_num, _result, _begin_time_now):
    global per_size_min_bytes
    global begin_time_now
    global pkts_build
    if const.time_num - _begin_time_now > _num * const.min_send_space:
        _total_time = int(np.random.randint(_num * const.min_send_space,
                                            min(_num * const.max_send_space, const.time_num - _begin_time_now)))
    elif const.time_num - _begin_time_now < _num / 2:
        return True
    else:
        _total_time = const.time_num - _begin_time_now

    _time_space = np.floor(np.random.exponential(scale=_total_time / _num, size=_num))
    time = _begin_time_now
    _pkt_num = _pkt_pre_num
    for _i in _time_space:
        results[time].append((_tuple5, int(_pkt_size_list[_pkt_num])))
        per_size[time] += int(_pkt_size_list[_pkt_num])
        _pkt_num += 1
        time += int(_i)
        pkts_build += 1
        if time >= const.time_num:
            # print(_num - _pkt_num + _pkt_pre_num)
            # if _num - _pkt_num + _pkt_pre_num > 1000:
            #     per_size_min_bytes += const.per_size_min_bytes
            #     begin_time_now = 0
            return


if __name__ == '__main__':
    # print(sys.argv[1])
    zip_list = get_zif()
    pkt_size_list = get_pkt_size()
    tuple5_buffer = []
    sip_buffer = []
    dip_buffer = []
    sport_buffer = []
    dport_buffer = []
    protocol_buffer = []
    set_buffer()

    #
    results = [[] for i in range(const.time_num)]
    per_size = [0 for i in range(const.time_num)]
    pkts_build = 0
    begin_time_now = 0
    #
    per_size_min_bytes = const.per_size_min_bytes * 1
    for i in range(len(zip_list)):
        if zip_list[i] <= 1:
            break
        tuple5 = get_random_tuple5()
        if set_time(tuple5, int(zip_list[i]), pkt_size_list, pkts_build, results, begin_time_now):
            i -= 1
            begin_time_now = 0
            per_size_min_bytes += const.per_size_min_bytes

        # 找到一个合适的下一个begin time时间，在接下来的stupid space时间内的每个时间片内，到达的包总字节数不超过per_size_min_bytes
        # 这样保证在对一次时间片的遍历过程中，所有包的吞吐能够均匀分配到每个时间片内
        while begin_time_now < const.time_num:
            _b = True
            for _i in range(const.stupid_space):
                if begin_time_now + _i < const.time_num and per_size[begin_time_now + _i] > per_size_min_bytes:
                    begin_time_now += _i + 1
                    _b = False
                    break
            if _b:
                break
        # 如果没找到合适的begin time，则增加per_size_min，从头开始找时间片来分配该流的包
        if begin_time_now + const.stupid_space >= const.time_num:
            per_size_min_bytes += const.per_size_min_bytes
            begin_time_now = 0

    print('1')

    # write_file_name = "port_1.txt"


    write_file_name = "./trace/port.txt"
    write_file = open(write_file_name, 'w+')

    sys.stdout = write_file
    total_size = 0
    time_now = 0
    time_remain = 0
    for i in range(const.time_num):
        pkt_per = {}
        size_len = 0
        result = results[i]
        total_size += per_size[i]

        while total_size < const.per_size_mean_bytes * (i + 1):
            tuple5 = get_random_tuple5()
            result.append((tuple5, pkt_size_list[pkts_build]))
            per_size[i] += pkt_size_list[pkts_build]
            total_size += pkt_size_list[pkts_build]
            pkts_build += 1

        for pkt_info in result:
            if pkt_info[0] not in pkt_per:
                pkt_per[pkt_info[0]] = [pkt_info[1]]
            else:
                pkt_per[pkt_info[0]].append(pkt_info[1])

        pkt_per_list = list(pkt_per.items())
        np.random.shuffle(pkt_per_list)

        for pkt_infos in pkt_per_list:
            for pkt in pkt_infos[1]:
                print(time_now, pkt_infos[0], int(pkt))
                time_space = np.random.uniform(time_remain + const.space_per_bytes * pkt)
                time_now += const.time_per_bytes * pkt + time_space
                time_remain = time_remain + const.space_per_bytes * pkt - time_space