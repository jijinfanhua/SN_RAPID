import subprocess
import numpy as np

# 定义参数的范围
arg3 = 100000
arg1_range = np.round(np.arange(0.1, 1.01, 0.1) * arg3).astype(int)
arg2_range = np.arange(100, 2001, 100)
arg4_range = np.round(np.arange(1.01, 1.21, 0.01), 2)

# arg1_range = np.round(np.arange(0.1, 0.2, 0.1) * arg3).astype(int)
# arg2_range = np.arange(100, 102, 100)
# arg4_range = np.round(np.arange(1.01, 1.03, 0.01), 2)


# 对每个参数组合执行脚本
for arg1 in arg1_range:
    for arg2 in arg2_range:
        for arg4 in arg4_range:
            subprocess.call(['python', './run_script.py', str(arg1), str(arg2), str(arg3), str(arg4)])
