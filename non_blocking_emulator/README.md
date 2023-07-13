# RAPID-SIM
 A simulator for RAPID

1. 请将生成的trace命名为"syn_[pkt_num]\_[flow_num]\_[cycle_num]_[zipfa].txt"
2. 将工程生成的"RAPID-SIM.exe"放置到non_blocking_emulator根目录下；
3. 命令：RAPID-SIM.exe 1 [trace_name]
4. 运行完成后，运行"get_average_queue_length.py [trace_name]"

需求
1. 固定周期数为100000;
2. 通过控制pkt_num来控制吞吐：
3. 吞吐：10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%, 90%, 100%
4. zipfa：1.01 : 0.01: 1.20
5. flow_num：100 : 100 :  2000

需求2
1. Baidu Trace: 合成14、28、42、56、70、84、98吞吐的trace
2. CAIDA Trace：合成9、18、27、36、45、54、63、72、81、90、99吞吐的trace
3. 跑两个use case：heavy hitter、SYN Flood