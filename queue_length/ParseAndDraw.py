import re
import matplotlib.pyplot as plt

filename = "non_blocking_result.txt"
filename_2 = "blocking_result.txt"

# [flow_num, lamda(%), T, EW_Q, EN_Q]
blocking_res = []

# [Eh^2, Eh, T, lamda, sita, EN_Q, EW_Q]
non_blocking_res = []

with open(filename, "r") as file:
    for line in file:
        text = line.rstrip()
        pattern = re.compile(
            r"Eh\^2:\s+(\d+) Eh:\s+(\d+) m:\s+(\d+) lambda:\s+([\d.]+) sita:\s+([\d.]+) EN_q:\s+([\de.-]+) EW_q:\s+([\de.-]+)")
        matches = pattern.findall(text)
        non_blocking_res.append([[float(val) for val in match] for match in matches])

with open(filename_2, "r") as file_2:
    for line in file_2:
        text_2 = line.rstrip()
        pattern_2 = re.compile(r"n = (\d+), lamda = (\d+), m = (\d+), EW_q = ([\d.]+) EN_q = ([\d.]+)")
        matches_2 = pattern_2.findall(text_2)
        blocking_res.append([[float(val) for val in match] for match in matches_2])

# for i in range(len(non_blocking_res)):
#     print(non_blocking_res[i])

for i in range(len(blocking_res)):
    print(blocking_res[i])


# 数据
x = [1, 2, 3, 4, 5]
y = [2, 3, 5, 7, 11]

# 创建图表

# 绘制数据

# 创建一个图形和轴
fig, ax = plt.subplots()

# 在这个轴上绘图
ax.plot(x, y)

plt.show()

plt.show()  # 展示图表
