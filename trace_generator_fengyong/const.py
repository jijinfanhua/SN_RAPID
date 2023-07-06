from config import config_list


class Consts:
    def __init__(self, throughput=100, zipf_a=1.005, max_flow_id=1000000, mean_pkt_len=300,
                 sip_num=1000, dip_num=1000, sport_num=1000, dport_num=1000, proto_num=128,
                 mu1=1200, sigma1=100, fraction1=0.1,
                 mu2=200, sigma2=30,
                 sigma3=100, fraction3=0.33
                 ):
        self.throughput = throughput  # Gbps
        self.bytes_per_second = throughput * (1 << 30) / 8
        self.time_slice_num = 1000000  # 1 us as a time slice
        self.bytes_per_time_slice = int(self.bytes_per_second / self.time_slice_num)

        self.zipf_a = zipf_a
        self.max_flow_id = max_flow_id
        self.mean_pkt_len = mean_pkt_len
        self.pkt_num_per_second = int(self.bytes_per_second / self.mean_pkt_len)  # 一秒内平均包的个数
        self.zipf_size = self.pkt_num_per_second

        # data pool size of tuple5
        self.sip_num = sip_num
        self.dip_num = dip_num
        self.sport_num = sport_num
        self.dport_num = dport_num
        self.proto_num = proto_num

        # parameters of trimodal distribution
        self.mu1 = mu1
        self.sigma1 = sigma1
        self.fraction1 = fraction1
        self.N1 = int(self.pkt_num_per_second * self.fraction1)

        self.mu2 = mu2
        self.sigma2 = sigma2

        self.fraction2 = (mean_pkt_len - fraction1 * mu1) / mu2
        # if self.fraction2 <= 0 or self.fraction2 >= 1:
        #     print(self.fraction2)
        #     raise ValueError("参数配置有误，fraction2不在0到1之间")
        self.N2 = int(self.pkt_num_per_second * self.fraction2)

        self.mu3 = self.mean_pkt_len
        self.sigma3 = sigma3
        self.fraction3 = fraction3
        self.N3 = int(self.pkt_num_per_second * self.fraction3)

        self.min_threshold_byte_size_of_each_slice = self.bytes_per_time_slice * 0.1  # 每个时间片的数据量的初始阈值

        self.min_send_space = 1
        self.max_send_space = 2

        self.stupid_space = 5

        self.time_per_bytes = 8 / 100  # 每字节的传输延时（ns）
        self.space_per_bytes = 1000000000 / self.bytes_per_second - self.time_per_bytes  # 每字节的空余时间，单位ns


port_num = 16
cnt_port = 0

const_list = []
for config in config_list:
    if cnt_port >= port_num:
        break
    const_list.append(
        Consts(
            throughput=config.throughput,
            zipf_a=config.zipf_a,
            max_flow_id=config.max_flow_id,
            mean_pkt_len=config.mean_pkt_len,
            sip_num=config.sip_num,
            dip_num=config.dip_num,
            sport_num=config.sport_num,
            dport_num=config.dport_num,
            proto_num=config.proto_num,
            mu1=config.mu1,
            sigma1=config.sigma1,
            fraction1=config.fraction1,
            mu2=config.mu2,
            sigma2=config.sigma2,
            sigma3=config.sigma3,
            fraction3=config.fraction3
        )
    )
    cnt_port += 1
