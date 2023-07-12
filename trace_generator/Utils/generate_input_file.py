# generate input file for packet generator

# input format:
#   1. number of packets
#   2. time
#   3. number of flows
#   4-4+number of flows:  flow id, arrival rate of each flow

# 1. 确定要生成的流数量、数据包个数、吞吐（确定时钟数）；
# 2. 按照流数量，使用zipf程序生成每流包数量；
# 3. 生成input file
# 预计：50%吞吐丢包率为10%-30%（变量：zipf参数、流个数、平均每流包个数）

with open("input.txt", "w") as f:
    f.write(str(int(5e3)) + '\n')
    f.write(str(int(1e4)) + '\n')
    f.write("5000\n")
    for i in range(5000):
        f.write(str(i) + " " + str(1) + "\n")
