import os

for packet_num in range(10000, 100001, 10000):
    for flow_num in range(200, 2001, 200):
        cycle_num = 100000
        for zipfa in [round(i * 0.01, 2) for i in range(101, 111)] + [1.2]:
            for burst in [i * 0.5 for i in range(1, 7)]:
                trace_name = f"output_{packet_num}_{flow_num}_{cycle_num}_{zipfa}_{burst}.txt"
                os.system(f"RAPID-SIM.exe 1 {trace_name}")
                os.system(f"python get_average_queue_length_syn.py {trace_name}")

# res = open("new_res.txt", 'w')
#
# with open("result.txt", 'r') as f:
#     while True:
#         line = f.readline()
#         if not line:
#             break
#         arr = line.split(',')
#
#         pkt_num = arr[0].split('_')[1]
#         flow_num = arr[0].split('_')[2]
#         writeback_ratio = arr[0].split('_')[-1]
#
#         res.write(f'{arr[0]}, {pkt_num}, {flow_num}, {writeback_ratio}, {arr[1].strip()}, {arr[2].strip()}, {arr[3].strip()}, {arr[4].strip()}\n')
#
# res.close()