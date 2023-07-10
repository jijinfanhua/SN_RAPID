# 需要测试的结果：

------

## blocking模式：特定吞吐、队列数量

平均处理延迟（排队延迟+处理延迟）、平均队长（任意时刻）、丢包率

## non-blocking模式：特定吞吐、带状态应用（写回率）

平均处理延迟（排队延迟+处理延迟）、平均队长（任意时刻）、丢包率

-------

# Trace

1. Baidu Trace：实际为14.1 Gbps，可以参与网卡场景和交换机场景的实验
2. Caida Trace：实际为4.28 Gbps，仅参与网卡场景的实验
3. Synthetic Trace 1: possion 1 （流量均衡）； 吞吐为30%；网卡&交换机
4. Synthetic Trace 2: possion 2 （流量不均衡）；吞吐为30%；网卡&交换机
5. Synthetic Trace 3: possion 1 （流量均衡）；吞吐为50%；网卡&交换机

需要注意的是，合成的trace还要携带数据包长度，这样我们才知道它的吞吐

# Caida Equinix Trace吞吐 （TCP&UDP / Gbps）

1. equinix-nyc.dirA.20190117-125910.UTC.anon：4.27544
2. equinix-nyc.dirA.20190117-130000.UTC.anon：4.20907
3. equinix-nyc.dirA.20190117-130100.UTC.anon：4.16743
4. equinix-nyc.dirA.20190117-130200.UTC.anon：4.23021
5. equinix-nyc.dirA.20190117-130300.UTC.anon：4.3777
6. equinix-nyc.dirA.20190117-130400.UTC.anon：4.41852
7. equinix-nyc.dirA.20190117-130500.UTC.anon：4.24745
8. equinix-nyc.dirA.20190117-130600.UTC.anon：4.42327
9. equinix-nyc.dirA.20190117-130700.UTC.anon：4.20596
10. equinix-nyc.dirA.20190117-130800.UTC.anon：4.20138
11. equinix-nyc.dirA.20190117-130900.UTC.anon：4.5261
12. equinix-nyc.dirA.20190117-131000.UTC.anon：4.30303
13. equinix-nyc.dirA.20190117-132000.UTC.anon：4.62067
14. equinix-nyc.dirA.20190117-133000.UTC.anon：4.56082
15. equinix-nyc.dirA.20190117-134000.UTC.anon：4.50923
16. equinix-nyc.dirA.20190117-135000.UTC.anon：4.7295
17. equinix-nyc.dirA.20190117-140000.UTC.anon：4.50621

# Trace及Non-Blocking Emulator仿真说明

1. emulator接收的trace是[数据包五元组]的序列，然后会将其转化为flow_id的序列；
2. 我们会按照每个数据包的大小和时间戳来计算该流量的吞吐：Gbps；
3. 按照该吞吐计算需要的气泡，我们默认如果没有气泡则能支持到满吞吐；比如100Gbps，我们的流量为15Gbps，那么我们每100个周期就需要85个气泡，这样的话在pps上也是15%，这样合不合理？
4. 如果说设备支持的最小平均包长（满足满吞吐）等于打的包的平均包长，那么是可以的，bps==pps，但是一般来说打的包的平均包长是大于最小平均包长的，所以说这样应该是不合理的。
5. 当前我们的emulator程序就是这么处理的；但是刘书昕的程序并不是，是给出了[时钟周期数，数据包flow_id]，就应该将其向我们的emulator进行转化；比如，我们假定最大吞吐为100Gbps，能够支持的最小平均数据包长度为256bytes，打入流量的平均数据包长度为256bytes；我们就应该按照这个来计算出该设备的频率，也就是48.8MHz；
6. 也就是每秒能够处理4.88*10^7个256bytes大小的数据包；
7. 所以lsx生成的trace应该这样给我，[吞吐多少，[数据包id顺序列表]]，因为刘书昕的计算是基于pps，我们的emulator也是基于pps。

# 2023-7-10

1. baidu - 丢包率、数据包平均处理延迟、数据包平均队长

丢包率：blocking & non-blocking都有

数据包平均处理延迟：blocking和non-blocking均是只算未丢的数据包

数据包平均队长：入队时的平均队长（记录数据包入队时间，根据时间找此时平均队长）、全部的平均队长（记录最后一个数据包发完的时刻）