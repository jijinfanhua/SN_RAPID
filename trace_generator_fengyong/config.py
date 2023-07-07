import json


class ConstConfig:
    def __init__(self,
                 throughput=100,
                 zipf_a=1.005,
                 max_flow_id=1000000,
                 mean_pkt_len=300,
                 sip_num=1000,
                 dip_num=1000,
                 sport_num=1000,
                 dport_num=1000,
                 proto_num=128,
                 mu1=1200,
                 sigma1=100,
                 fraction1=None,
                 mu2=200,
                 sigma2=30,
                 sigma3=100,
                 fraction3=None
                 ):
        if fraction3 is None:
            fraction3 = [1, 3]
        if fraction1 is None:
            fraction1 = [1, 10]
        self.throughput = throughput
        self.zipf_a = zipf_a
        self.max_flow_id = max_flow_id
        self.mean_pkt_len = mean_pkt_len
        self.sip_num = sip_num
        self.dip_num = dip_num
        self.sport_num = sport_num
        self.dport_num = dport_num
        self.proto_num = proto_num
        self.mu1 = mu1
        self.sigma1 = sigma1
        self.fraction1 = fraction1[0] / fraction1[1]
        self.mu2 = mu2
        self.sigma2 = sigma2
        self.sigma3 = sigma3
        self.fraction3 = fraction3[0] / fraction3[1]


config_list = []

with open("./config.json") as config_file:
    configs = json.load(config_file)
    for config in configs['configs']:
        config.pop('//')
        config_list.append(ConstConfig(**config))
