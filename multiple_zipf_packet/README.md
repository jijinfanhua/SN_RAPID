1. 固定周期数为100000;
2. 通过控制pkt_num来控制吞吐：
3. 吞吐：10%, 20%, 30%, 40%, 50%, 60%, 70%, 80%, 90%, 100%
    10个
4. zipfa：from 1.01 to 1.1,step = 0.01 and 1.2
    11个
5. flow_num：from 200 to 2000,step = 200
    10个
6. 分布系数:from 0.5 to 3.0,step = 0.5
    6个
7. 多少个大流?from 1  to 10

# 接口定义
> script_overall

接受参数:
packet_num
zipfa,
flow_num,
分布系数,
多少个大流
多少时钟周期

调用script
> script

> zipf_generator
  
接受
吞吐,
zipfa,
flow_num,
多少个大流
多少时钟周期

> 拼接方法

把所有zipf_generator生成的包拼接在一起


> packet_generate

接受
输入的包,
分布系数

输出
满足要求的output

完全复用arrival_average