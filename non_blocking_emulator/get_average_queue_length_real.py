import os
import sys
import re

# 首先获得数据包所在的时钟，然后读每个时钟的队列长度

pkt_clk = []

# 该文件需要参数化

trace_type = sys.argv[1] #"caida"
trace_num = sys.argv[2] # res_topk_nic_54
trace_folder = "res_topk_nic_" + trace_num

trace_path = "./trace_real/" + trace_type + "/" + trace_folder + "/"

with open(trace_path + "packet_clock.txt", 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break
        arr = line.split()
        clk = int(arr[1])
        pkt_clk.append(clk + 1)

queue_clk_dict = {}

with open(trace_path + "db_size_i_27_j_" + trace_num + "_1q.txt", 'r') as g:
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
with open(trace_path + "end_clock.txt", 'r') as h:
    last_clk = int(h.readline().strip())

# 使用列表推导式选择所有小于 last_clk 的键对应的值
values = [v for k, v in queue_clk_dict.items() if k < last_clk]
# 计算均值
mean_value = sum(values) / len(values) if values else None

print("average queue length: {}".format(mean_value))

# 打开文件并读取内容
with open(trace_path + 'direct_single_topk_' + trace_num + '.txt', 'r') as output:
    text = output.read()

# 定义正则表达式模式
drop_pattern = r'Drop\s+%\s+=\s+(\d+\.\d+)'
time_pattern = r'average elapse time\s+=\s+(\d+)'

# 使用re.findall查找匹配的模式
drop = re.findall(drop_pattern, text)
if drop:
    drop = float(drop[0])
else:
    print("No match for Drop % found.")

elapse_time = re.findall(time_pattern, text)
if elapse_time:
    elapse_time = int(elapse_time[0])
else:
    print("No match for average elapse time found.")

print(f'Drop % = {drop}, average elapse time = {elapse_time}')

with open(trace_path + trace_type + "_" + trace_num + '_queue_info.txt', 'w') as t:
    t.write("packet average queue length: {}\n".format(queue_sum / len(pkt_clk)))
    t.write("average queue length: {}\n".format(mean_value))

with open("./result_real.txt", 'a') as res:
    res.write(f'{trace_type + "_" + trace_num}, {drop}, {elapse_time}, {queue_sum / len(pkt_clk)}, {mean_value}\n')

