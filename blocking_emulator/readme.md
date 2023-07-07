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