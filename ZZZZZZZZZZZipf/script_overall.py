import subprocess
import numpy as np

arg3 = 100000
arg1_range = np.round(np.arange(0.1, 1.01, 0.01) * arg3).astype(int) #
flow_num = 1000
arg4_range = np.array([1.01,1.04,1.07,1.10,1.13])

for arg1 in arg1_range:
    for arg4 in arg4_range:
        subprocess.call(['python', './script.py', str(arg1), str(flow_num), str(arg3), str(arg4)])

