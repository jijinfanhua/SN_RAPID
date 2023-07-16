import subprocess
import numpy as np

arg3 = 100000
arg1_range = np.round(np.arange(0.1, 1.01, 0.1) * arg3).astype(int) #
arg2_range = np.arange(200, 2001, 200) # flow num
arg4_range = np.round(np.arange(1.01, 1.1, 0.01), 2)
arg4_range = np.append(arg4_range,1.2)
arg5_range = np.arange(0.5, 3.5, 0.5)
# print(arg5_range)

for arg1 in arg1_range:
    for arg2 in arg2_range:
        for arg4 in arg4_range:
            for arg5 in arg5_range:
                subprocess.call(['python', './script.py', str(arg1), str(arg2), str(arg3), str(arg4),str(arg5)])

