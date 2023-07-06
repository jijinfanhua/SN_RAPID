import numpy

class _const:
    class ConstError(TypeError):
        pass

    class ConstCaseError(ConstError):
        pass

    def __setattr__(self, name, value):
        self.__dict__[name] = value


const = _const()

const.sip_num   = 1000
const.dip_num   = 1000
const.sport_num = 100
const.dport_num = 100
const.tuple5_num= 5000000

const.mean_pkt_len = 300 #

const.throughput = 30
const.total_size_bytes = const.throughput * (1 << 30) / 8
const.total_size_mean = int(const.total_size_bytes / const.mean_pkt_len)

const.zipf_a    = 1.0001 #
const.zipf_size = const.total_size_mean
const.flow_num  = 10000000

const.mu1, const.sigma1, const.N1 = 1200, 100, int(const.total_size_mean / 10) #
const.mu2, const.sigma2, const.N2 = 200, 30, int(const.total_size_mean * 9 / 10) #
const.mu3, const.sigma3, const.N3 = 300, 100, int(const.total_size_mean / 3) #

const.time_num = 1000000
const.size_per = 12500

const.per_size_min_bytes = int(const.total_size_bytes / const.time_num * 0.1)
const.per_size_mean_bytes = int(const.total_size_bytes / const.time_num)
const.time_per_bytes = 8 / 100
const.space_per_bytes = 1000000000 / const.total_size_bytes - const.time_per_bytes

const.min_send_space = 1
const.max_send_space = 2

const.stupid_space = 5
