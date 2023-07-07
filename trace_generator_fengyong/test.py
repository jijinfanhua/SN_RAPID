import numpy as np
import matplotlib as plt

# 验证生成的trace是否满足配置要求
# 吞吐量、平均包长度


def test_throughput(path, target_throughput, output_path):
    with open(path, 'r') as f:
        lines = f.readlines()
        total_throughput = 0
        for line in lines:
            time, tuple5, pkt_len = line.split(' ')
            total_throughput = total_throughput + int(pkt_len)
        average_pkt_len = total_throughput / len(lines)
        with open(output_path, 'w') as fileout:
            fileout.write('target_throughput: {target}, pkt_num: {length}, total_throughput: {sum}, average_pkt_len: {ave_pkt_len}\n'
                          .format(target=target_throughput, length=len(lines), sum=total_throughput / (1 << 30) * 8, ave_pkt_len=average_pkt_len))

