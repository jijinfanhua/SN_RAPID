# 这个脚本只承担了向运行脚本传输参数的任务
import subprocess
import numpy as np
from tqdm import trange

arg1_range = np.round(np.arange(0.1, 1.01, 0.1) * 100000).astype(int) #几个包
arg2_range = np.array([100, 200, 500, 1000, 1500, 2000])
# arg3 == 10 0000
arg4_range = np.round(np.arange(1.01, 1.1, 0.01), 2)    # zipf系数
arg4_range = np.append(arg4_range,1.2)                  #
# arg5_range = np.arange(1, 3.1, 1)  # poisson系数
arg6_range = np.arange(1,6)# 几个大流

for arg1 in arg1_range:
    for arg2 in arg2_range:
        for arg4 in arg4_range:
            # for arg5 in arg5_range:
            for arg6 in arg6_range:
                subprocess.call(['python', './script.py', str(arg1), str(arg2), str(100000), str(arg4),str(arg6)])
