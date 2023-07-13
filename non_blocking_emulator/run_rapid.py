import os

for packet_num in range(10000, 100001, 10000):
    for flow_num in range(100, 2001, 100):
        cycle_num = 100000
        for zipfa in [round(i*0.01, 2) for i in range(101, 121)]:
            trace_name = f"syn_{packet_num}_{flow_num}_{cycle_num}_{zipfa}.txt"
            os.system(f"RAPID-SIM.exe 1 {trace_name}")
            os.system(f"python get_average_queue_length.py {trace_name}")