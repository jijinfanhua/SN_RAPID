import random


def generate_real_trace(src_file_name, writeback_ratio):
    new_f = open(src_file_name + "_ratio_" + str(writeback_ratio) + ".txt", 'w')

    d = {}
    with open(src_file_name + ".txt", 'r') as f:
        while True:
            line = f.readline()
            if not line:
                break

            flag = 0
            arr = line.split()
            clk, idx = int(arr[0]), int(arr[1])

            if idx in d:
                d[idx] += 1
                pass
            else:
                d[idx] = 0

            ra = random.randint(0, 100)
            if ra < writeback_ratio:
                flag = 1
            new_f.write(line.strip() + ' ' + str(flag) + ' ' + str(d[idx]) + '\n')

    new_f.close()

if __name__ == '__main__':
    for pkt_num in range(10000, 100001, 10000):
        for flow_num in range(100, 2001, 100):
            for zipfa in range(1.01, 1.21, 0.01):
                for writeback_ratio in range(0, 101, 5):
                    file_name = 'pkt_' + str(pkt_num) + '_' + str(flow_num) + '_100000_' + str(zipfa)
                    generate_real_trace(file_name, writeback_ratio)

