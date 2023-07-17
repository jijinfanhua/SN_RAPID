# 这个脚本承担的任务只是给两个接口传递参数
# 包的数量 流的数量 时钟周期总数  zipf系数 poisson强度  大流个数
import subprocess
import sys
import os
import shutil
import time
import numpy as np

def main(arg1, arg2, arg3, arg4,arg5,arg6):
    pkt_num = np.ceil(arg1 / arg6)
    time = np.ceil(arg3 / arg6)

    subprocess.check_call(['python', './zipf_input.py', str(pkt_num), str(arg2), str(time), str(arg4),str(arg6)])
        # 把拼接放到下层

    input_file_name = "./input_{}_{}_{}_{}_{}.txt".format(str(arg1), str(arg2), str(arg3), str(arg4),str(arg6))
    output_file_name = "./output_{}_{}_{}_{}_{}_{}.txt".format(str(arg1), str(arg2), str(arg3), str(arg4),str(arg5),str(arg6))

    while not os.path.isfile(input_file_name):
        time.sleep(1)
        print("sleep 1")

    subprocess.check_call(['python', './arrival_average.py', input_file_name, output_file_name,str(arg5)])

    while not os.path.isfile(output_file_name):
        time.sleep(1)
        print("sleep 2")

    if os.path.isfile(input_file_name):
        os.remove(input_file_name)

    Destination_folder = "./Trace"
    shutil.move(output_file_name,Destination_folder)

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5],sys.argv[6])
