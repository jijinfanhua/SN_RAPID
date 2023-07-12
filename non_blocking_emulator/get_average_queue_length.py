import os
import sys

# 首先获得数据包所在的时钟，然后读每个时钟的队列长度

pkt_clk = []

# 该文件需要参数化

trace_name = sys.argv[1] #"output3.txt"

with open("./trace_syn/" + trace_name, 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break
        arr = line.split()
        clk, idx = int(arr[0]), int(arr[1])
        pkt_clk.append(clk)

queue_clk_dict = {}

with open("./trace_syn/" + "db_size_i_27_1q.txt", 'r') as g:
    while True:
        line = g.readline()
        if not line:
            break
        arr = line.split()
        clk, l = int(arr[1]), int(arr[3])
        queue_clk_dict[clk] = l

# 1. 求数据包进入队列时的队列长度
queue_sum = 0
for clk in pkt_clk:
    queue_sum += queue_clk_dict[clk]
print("packet average queue length: {}".format(queue_sum / len(pkt_clk)))

# 2. 求任意时刻队列长度，需要读取end_clock
last_clk = 0
with open("./trace_syn/" + "end_clock.txt", 'r') as h:
    last_clk = int(h.readline().strip())

# 使用列表推导式选择所有小于 last_clk 的键对应的值
values = [v for k, v in queue_clk_dict.items() if k < last_clk]
# 计算均值
mean_value = sum(values) / len(values) if values else None

print("average queue length: {}".format(mean_value))

filename_without_extension, extension = os.path.splitext(trace_name)

with open("./trace_syn/" + filename_without_extension + '_queue_info.txt', 'w') as t:
    t.write("packet average queue length: {}\n".format(queue_sum / len(pkt_clk)))
    t.write("average queue length: {}\n".format(mean_value))

