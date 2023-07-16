import os

res = open("blocking_schedule_new_result.txt", 'w')

with open("blocking_schedule_result.txt", 'r') as f:
    while True:
        line = f.readline()
        if not line:
            break
        arr = line.split(',')

        file_name = arr[0].split()[0]
        pkt_num = file_name.split('_')[1]
        flow_num = file_name.split('_')[2]
        zipfa = file_name.split('_')[-1]

        res.write(f'{arr[0]}, {pkt_num}, {flow_num}, {zipfa}, {arr[1].strip()}, {arr[2].strip()}, {arr[3].strip()}, {arr[4].strip()}, {arr[5].strip()}, {arr[6].strip()}\n')

res.close()