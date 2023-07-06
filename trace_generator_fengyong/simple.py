from tqdm import tqdm
import collections


# flow_dict = defaultdict()
# counter = 1

# packets = []

# cnt = 0

# with open("./trace/switch.txt", 'r') as f:
#     while True:
#         line = f.readline()
#         if not line:
#             break
#         if len(line) <= 1:
#             continue
#         data = eval(line)
#         flow = data[1]

#         if flow not in flow_dict:
#             flow_dict[flow] = counter
#             counter += 1
#         packets.append(flow_dict[flow])

#         cnt += 1
#         if cnt % 1000000 == 0:
#             print(cnt)

# print(len(packets))

# # with open("./trace/switch.txt", 'r') as f:
# #     with tqdm(desc="Processing file", unit=" lines") as pbar:
# #         while True:
# #             line = f.readline()
# #             if not line:
# #                 break
# #             if len(line) <= 1:
# #                 continue
# #             data = eval(line)
# #             flow = data[1]

# #             if flow not in flow_dict:
# #                 flow_dict[flow] = counter
# #                 counter += 1
# #             packets.append(flow_dict[flow])

# #             pbar.update(10000)

# with open('syn_trace.txt', 'w') as g:
#     for pkt in tqdm(packets, desc="Writing to file"):
#         g.write(str(pkt) + '\n')


# flow_dict = defaultdict()
# cnt = 0
# with open('syn_trace.txt', 'r') as h:
#     while True:
#         line = h.readline()
#         if not line:
#             break
#         flow_id = int(line.strip())
#         if flow_id not in flow_dict:
#             flow_dict[flow_id] = 1
#         else:
#             flow_dict[flow_id] += 1
#         cnt = cnt + 1
#         if cnt % 1000000 == 0:
#             print(cnt)

# print(cnt)

# sorted_d = dict(sorted(flow_dict.items(), key=lambda item: item[1], reverse=True))

# print(len(sorted_d))

# with open('syn_trace_info.txt', 'w') as k:
#     for key, value in tqdm(sorted_d.items(), desc="Writing to file"):
#         k.write(str(key) + ' ' + str(value) + '\n')


num_files = 8
num_lines_per_file = 50000000

# # 读取大文件并拆分成小文件
# with open('./trace/syn_trace.txt', 'r') as bigfile:
#     for i in range(num_files):
#         print(f"writing trace file {i}")
#         with open(f'./trace/syn_trace_{i}.txt', 'w') as smallfile:
#             for _ in range(num_lines_per_file):
#                 line = bigfile.readline()
#                 if not line:  # 如果文件已经读完，就结束循环
#                     break
#                 smallfile.write(line)

# 分析每个小文件中的数字出现次数，并按次数降序排列，写入新文件
for i in range(num_files):
    print(f"analyzing trace file {i}")
    counter = collections.Counter()
    with open(f'./trace/syn_trace_{i}.txt', 'r') as smallfile:
        for line in smallfile:
            number = int(line.strip())  # 假设每行只有一个数字
            counter[number] += 1
    print(len(counter))
    with open(f'./trace/syn_trace_info_{i}.txt', 'w') as countfile:
        for number, count in counter.most_common():
            countfile.write(f'{number} {count}\n')
        