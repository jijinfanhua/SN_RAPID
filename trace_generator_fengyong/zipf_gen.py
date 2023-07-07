import json

from const import const_list, Consts

import numpy as np
import os


class ZipfGenerator:
    def __init__(self, const=Consts()):
        self.const = const

    def gen_zipf(self):
        zipf_a = self.const.zipf_a
        # zipf_a = 2
        zipf_size = self.const.zipf_size
        # zipf_size = 1000
        max_flow_id = self.const.max_flow_id
        # max_flow_id = 10000
        zipf_file_path = 'zipf_results/zipf_' + str(zipf_a) + '_' + str(zipf_size) + '.json'
        if os.path.exists(path=zipf_file_path):
            print("same zipf distribution has already been generated")
            return
        else:
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
            return


generator_list = []

if __name__ == "__main__":
    cnt = 0
    for const in const_list:
        zipf_generator = ZipfGenerator(const)
        print("zipf {cnt}, a={zipf_a},size={zipf_size}"
              .format(cnt=cnt,
                      zipf_a=zipf_generator.const.zipf_a,
                      zipf_size=zipf_generator.const.zipf_size))
        generator_list.append(zipf_generator)
        cnt += 1

    for zipf_generator in generator_list:
        zipf_generator.gen_zipf()
