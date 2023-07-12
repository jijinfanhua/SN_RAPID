import os

for packet_num in range(10000, 100001, 10000):
    for flow_num in range(100, 2001, 100):
        cycle_num = 100000
        for zipfa in [round(i*0.01, 2) for i in range(101, 121)]:
            trace_name = f"syn_{packet_num}_{flow_num}_{cycle_num}_{zipfa}.txt"
            os.system(f"RAPID-SIM.exe 1 {trace_name}")
            os.system(f"python get_average_queue_length.py {trace_name}")

# import os
# import subprocess
#
# def execute_and_log(command, logfile):
#     result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#     print(result.stdout.decode('utf-8'))
#     logfile.write(result.stdout.decode('utf-8').rstrip('\n'))
#     logfile.write(result.stderr.decode('utf-8').rstrip('\n'))
#
# for packet_num in range(10000, 100001, 10000):
#     for flow_num in range(100, 2001, 100):
#         cycle_num = 100000
#         for zipfa in [round(i*0.01, 2) for i in range(101, 121)]:
#             f = open('log.txt', 'a')
#             trace_name = f"syn_{packet_num}_{flow_num}_{cycle_num}_{zipfa}.txt"
#             execute_and_log(f"RAPID-SIM.exe 1 {trace_name}", f)
#             execute_and_log(f"python get_average_queue_length.py {trace_name}", f)
#             f.close()
