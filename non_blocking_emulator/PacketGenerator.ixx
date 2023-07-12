export module rapid.PacketGenerator;

import rapid.ZipfDistribution;
import rapid.GeometryDistribution;
import rapid.WriteBackGenerator;
import rapid.Packet;
import rapid.FlowInfo;
import std;


//export template <size_t K = 2>
//class PacketGenerator {
//    ZipfDistribution<K> m_zipf; // to control the key
//    GeometryDistribution m_geo; // to control the flow
//    WriteBackGenerator m_write_back_generator;
//
//    int m_clock { 0 };
//
//public:
//    int m_tx_packet_count { 0 };
//
//    PacketGenerator() = default;
//    PacketGenerator(double lambda, double alpha = 1.01)
//        : m_zipf(alpha)
//        , m_geo(lambda)
//    {
//    }
//
//    void set_lambda(double lambda)
//    {
//        m_geo.set_lambda(lambda);
//    }
//
//    void initialize_write_back_generator(std::initializer_list<std::pair<int, double>> l)
//    {
//        m_write_back_generator.initialize(l);
//    }
//
//    Packet next()
//    {
//        if (m_clock == 0) {
//            m_clock = m_geo.next() - 1;
//            ++m_tx_packet_count;
//            return m_write_back_generator.set_write_back(Packet { m_zipf.next() });
//        } else {
//            --m_clock;
//            return Packet {};
//        }
//    }
//};

export template <size_t K = 2>
class PacketGenerator {
    TunedZipfDistribution<K, 3, 100000, 10, 3, 2, 20> m_zipf; // to control the key
    //ZipfDistribution<K> m_zipf;
    GeometryDistribution m_geo; // to control the flow
    FixedWriteBackGenerator<K> m_write_back_generator;
    //WriteBackGenerator m_write_back_generator;
    int m_clock{ 0 };
    int m_arrive_period{ 10 }; //每几个周期到达一个包
    std::ofstream fout;

public:
    int m_tx_packet_count { 0 };

    PacketGenerator() = default;
    PacketGenerator(double lambda, double alpha = 1.01)
        : m_zipf(alpha)
        , m_geo(lambda)
    {
    }

    void set_lambda(double lambda)
    {
        m_geo.set_lambda(lambda);
    }

    void set_arr_period(size_t arr_period) {
        m_arrive_period = arr_period;
    }

    void initialize_write_back_generator(size_t wb_gap)
    {
        m_write_back_generator.initialize(wb_gap);
    }

    void initialize_write_back_generator(std::initializer_list<std::pair<int, double>> l) {
        m_write_back_generator.initialize(l);
    }

    void initialize_zipf(size_t fc_num, size_t fixed_flow_num, size_t pkt_gap, size_t burst_gap, double burst_rate) {
        m_zipf.set_parameter_and_init(fc_num, fixed_flow_num, pkt_gap, burst_gap, burst_rate);
    }

    void initialize_zipf() {
        m_zipf.set_parameter_and_init();
    }

    void reset() {
        
        fout.open("./packet_seq.txt", std::ios_base::out);
        m_zipf.reset();
    }

    Packet next()
    {
        //if (m_clock == 0) {
        //    m_clock = m_geo.next() - 1;
        //    ++m_tx_packet_count;
        //    return m_write_back_generator.set_write_back(Packet{ m_zipf.next() });
        //}
        //else {
        //    --m_clock;
        //    return Packet{};
        //}

        if (m_clock == 0) {
            m_clock = m_arrive_period - 1;
            //m_clock = m_geo.next() - 1;
            ++m_tx_packet_count;
            auto flow_id = m_zipf.next();
            auto pkt = m_write_back_generator.set_write_back(Packet{ flow_id });
            fout << flow_id << " " << (int)pkt.m_write_back_bitmap<< std::endl;
            return pkt;
        }
        else {
            --m_clock;
            fout << 0 << std::endl;
            return Packet{};
        }
    }
};

export template <size_t K = 2>
class SimplePacketGenerator {
    FixedWriteBackGenerator<K> m_write_back_generator;
    std::ofstream fout;
public:
    int m_tx_packet_count { 0 };
    int wb_gap{ 1 };
    int m_clock{ 0 };
    int m_arr_period{ 10 };
    SimplePacketGenerator() = default;
    SimplePacketGenerator(double lambda, double alpha = 1.01)
    {
    }

    void set_lambda(double lambda)
    {
    }

    void set_arr_period(size_t arr_period) {
    }

    void initialize_write_back_generator(size_t wb_gap)
    {
    }

    void initialize_write_back_generator(std::initializer_list<std::pair<int, double>> l) {
    }

    void reset() {
        fout.open("./packet_seq.txt", std::ios_base::out);
    }
    Packet next()
    {
        //if (m_clock == 0) {
        //    m_clock = m_arrive_period - 1;
        //    ++m_tx_packet_count;
        //    auto flow_id = m_zipf.next();
        //    auto pkt = m_write_back_generator.set_write_back(packet{ flow_id });
        //    //fout << flow_id << " " << (int)pkt.m_write_back_bitmap<< std::endl;
        //    return pkt;
        //}
        //else {
        //    --m_clock;
        //    //fout << 0 << std::endl;
        //    return packet{};
        //}
        //std::cout << m_arr_period << std::endl;
        if (m_clock == 0) {
            m_clock = m_arr_period - 1;
            ++m_tx_packet_count;
            //std::cout << m_write_back_generator.m_wb_gap << std::endl;
            auto pkt = m_write_back_generator.set_write_back(Packet{ 1 });
            fout<< 1 << " " << (int)pkt.m_write_back_bitmap << std::endl;
            return pkt;
        }
        else {
            m_clock--;
            fout << 0 << std::endl;
            return Packet{};
        }
    }
};


export template <typename T>
    requires std::is_base_of_v<BaseTuple, T>
class RealTracePacketGenerator {
    std::vector<Packet> packets;
    std::string file_name;
    //std::string tuple_id_map_name;
    std::uint32_t cnt_flow_num;
    std::map<T, std::uint32_t> flow_set;
    std::uint32_t m_arr_period{1};
    std::uint32_t m_bubble_period{1};
    std::uint32_t m_snd_bubble_cnt{0};
    std::uint32_t cnt_pkt_snd {0};

    std::vector<bool> packet_guider;
    std::uint32_t round_pointer{0};

    std::string parent_path;

    std::ofstream seq_out;

    std::ofstream pkt_clk;
    std::ofstream end_clk;

    std::uint32_t IF_CLK {0};

public:
    RealTracePacketGenerator() {
        std::string type;
        if (std::is_same<T, Tuple_5>::value) {
            std::cout << "tuple 5" << std::endl;
            type = "1";
        }
        else if (std::is_same<T, TupleSrcDst>::value) {
            std::cout << "tuple src dst" << std::endl;
            type = "2";
        }
        file_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\trace_" + type + ".txt";
        //tuple_id_map_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\tuple_id_map_" + type + ".txt";
        cnt_flow_num = 1;
        if (!std::filesystem::exists(std::filesystem::path(file_name))) {
            std::cout << "build new trace" << std::endl;
            build_trace();
        }
        //std::cout << "load trace" << std::endl;
        load_trace(file_name);
    }

    RealTracePacketGenerator(std::string real_trace_path) {
        IF_CLK = 1;
        std::ifstream trace_in;
        trace_in.open(real_trace_path, std::ios_base::in);
        std::cout << "load trace file is: " << real_trace_path << std::endl;
        if (!trace_in.is_open()) {
            std::cout << "open trace error" << std::endl;
            return;
        }
        std::string temp;
        std::cout << "load trace" << std::endl;
        std::uint64_t cnt_pkt = 0;
        
        while (getline(trace_in, temp)) {
            if (cnt_pkt % 1000000 == 0) {
                std::cout << cnt_pkt << std::endl;
            }
            std::vector<std::string> field;
            std::size_t pos = temp.find(" ");
            while (pos != std::string::npos) {
                field.push_back(temp.substr(0, pos));
                temp = temp.substr(pos + 1);
                pos = temp.find(" ");
            }
            field.push_back(temp);
            //std::cout << saddr << " " << daddr << " " << (uint32_t)protocol << " " << sport << " " << dport << " " << is_syn << std::endl;
            uint32_t flow_id = 0;
            uint32_t clock_time = 0;
            std::from_chars(field[0].data(), field[0].data() + field[0].size(), clock_time);
            std::from_chars(field[1].data(), field[1].data() + field[1].size(), flow_id);

            auto pkt = Packet{};
            pkt.m_key = flow_id;
            pkt.clock_time = clock_time;

            std::srand(flow_id);
            pkt.m_top_k_addr = { (std::uint32_t)std::rand(), (std::uint32_t)std::rand(), (std::uint32_t)std::rand() };
            packets.push_back(pkt);

            cnt_pkt++;
        }
        trace_in.close();
        std::cout << "trace_loaded" << std::endl;

        // 从路径中获取真实的trace的名字
        std::filesystem::path fs_path(real_trace_path);
        std::string trace_name = fs_path.filename().string();
        std::string::size_type const p(trace_name.find_last_of('.'));
        std::string file_without_extension = trace_name.substr(0, p);

        parent_path = fs_path.parent_path().string();

        pkt_clk.open(parent_path + "/packet_clock.txt", std::ios_base::out);
        end_clk.open(parent_path + "/end_clock.txt", std::ios_base::out);
    }

    RealTracePacketGenerator(int period, std::string real_trace_path) {
        std::string type;
        if (std::is_same<T, Tuple_5>::value) {
            std::cout << "tuple 5" << std::endl;
            type = "1";
        }
        else if (std::is_same<T, TupleSrcDst>::value) {
            std::cout << "tuple src dst" << std::endl;
            type = "2";
        }

        packet_guider = distributePoints(period);
        for (int i = 0; i < 100; i++) {
            if (i != 0 && i % 10 == 0) {
                std::cout << std::endl;
            }
            std::cout << packet_guider[i] << " ";
        }
        std::cout << std::endl;

        // 从路径中获取真实的trace的名字
        std::filesystem::path fs_path(real_trace_path);
        std::string trace_name = fs_path.filename().string();
        std::string::size_type const p(trace_name.find_last_of('.'));
        std::string file_without_extension = trace_name.substr(0, p);

        parent_path = fs_path.parent_path().string();

        file_name = parent_path + "/" + file_without_extension + "_simple.txt";


        //file_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\trace_" + type + ".txt";
        //tuple_id_map_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\tuple_id_map_" + type + ".txt";
        cnt_flow_num = 1;
        if (!std::filesystem::exists(std::filesystem::path(file_name))) {
            std::cout << "build new trace" << std::endl;
            build_trace(real_trace_path);
        }
        //std::cout << "load trace" << std::endl;
        load_trace(file_name);
    }

    void set_bubble_period(uint32_t bubble_period) {
        m_bubble_period = bubble_period;
    }

    void build_trace() {
        std::string file_raw_trace_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\baidu_build_trace_1_NIC_400W_from_2000W.txt";
        //std::string file_raw_trace_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\worst_trace.txt";
        std::ifstream trace_in;
        trace_in.open(file_raw_trace_name, std::ios_base::in);
        if (!trace_in.is_open()) {
            std::cout << "open trace file error" << std::endl;
            return;
        }
        std::ofstream fout;
        fout.open(file_name, std::ios_base::out);
        if (!fout.is_open()) {
            std::cout << "open out file error" << std::endl;
            return;
        }

        /*std::ofstream map_fout;
        map_fout.open(tuple_id_map_name, std::ios_base::out);
        if (!map_fout.is_open()) {
            std::cout << "open map file error" << std::endl;
            return;
        }*/

        std::string temp;
        std::cout << "load trace" << std::endl;
        std::uint64_t cnt_pkt = 0;
        //std::cout << cnt_flow_num << std::endl;
        // timestamp saddr daddr protocol sport dport is_syn
        while (getline(trace_in, temp)) {
            if (cnt_pkt % 1000000 == 0) {
                std::cout << cnt_pkt << std::endl;
            }
            std::vector<std::string> field;
            std::size_t pos = temp.find(" ");
            while (pos != std::string::npos) {

                field.push_back(temp.substr(0, pos));
                temp = temp.substr(pos + 1);
                pos = temp.find(" ");
                //std::cout << "here" << temp << std::endl;
            }
            field.push_back(temp);
            uint64_t time = 0;
            uint32_t saddr = 0;
            uint32_t daddr = 0;
            uint8_t protocol = 0;
            uint16_t sport = 0;
            uint16_t dport = 0;
            uint32_t tot_len = 0;
            int is_syn = 0;
            int is_ack = 0;
            std::from_chars(field[0].data(), field[0].data() + field[0].size(), time);
            std::from_chars(field[1].data(), field[1].data() + field[1].size(), saddr);
            std::from_chars(field[2].data(), field[2].data() + field[2].size(), daddr);
            std::from_chars(field[3].data(), field[3].data() + field[3].size(), protocol);
            std::from_chars(field[4].data(), field[4].data() + field[4].size(), sport);
            std::from_chars(field[5].data(), field[5].data() + field[5].size(), dport);
            std::from_chars(field[6].data(), field[6].data() + field[6].size(), tot_len);
            std::from_chars(field[7].data(), field[7].data() + field[7].size(), is_syn);
            std::from_chars(field[8].data(), field[8].data() + field[8].size(), is_ack);
            //std::cout << saddr << " " << daddr << " " << (uint32_t)protocol << " " << sport << " " << dport << " " << is_syn << std::endl;
            T flow = T(saddr, daddr, protocol, sport, dport);
            //std::cout << flow.saddr << " " << flow.daddr << std::endl;
            //std::cout << (uint32_t)flow.saddr << " " << (uint32_t)flow.daddr << std::endl;
            if (flow_set.find(flow) == flow_set.end()) {

                flow_set[flow] = cnt_flow_num;
                //if (std::is_same<T, Tuple_5>::value) {
                //    map_fout << saddr << " " << daddr << " " << (uint32_t)protocol << " " << sport << " " << dport << ":" << cnt_flow_num << std::endl;
                //    //std::cout << "here" << std::endl;
                //}
                //else if (std::is_same<T, TupleSrcDst>::value) {
                //    map_fout << saddr << " " << daddr << ":" << cnt_flow_num << std::endl;
                //}

                //std::cout << cnt_flow_num << std::endl;
                cnt_flow_num++;
            }
            std::uint32_t flow_id = flow_set[flow];
            fout << (uint32_t)flow_id << " " << (uint32_t)is_syn << " " << (uint32_t)is_ack << std::endl;
            cnt_pkt++;
        }
        trace_in.close();
        fout.close();
        //map_fout.close();

    }

    void build_trace(std::string real_trace_path) {
        std::string file_raw_trace_name = real_trace_path; // "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\baidu_build_trace_1_NIC_400W_from_2000W.txt";
        std::cout << "trace path is: " << file_raw_trace_name << std::endl;
        //std::string file_raw_trace_name = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\test_trace\\worst_trace.txt";
        std::ifstream trace_in;
        trace_in.open(file_raw_trace_name, std::ios_base::in);
        if (!trace_in.is_open()) {
            std::cout << "open trace file error" << std::endl;
            return;
        }
        std::ofstream fout;
        fout.open(file_name, std::ios_base::out);
        if (!fout.is_open()) {
            std::cout << "open out file error" << std::endl;
            return;
        }

        /*std::ofstream map_fout;
        map_fout.open(tuple_id_map_name, std::ios_base::out);
        if (!map_fout.is_open()) {
            std::cout << "open map file error" << std::endl;
            return;
        }*/
        
        std::string temp;
        std::cout << "load trace" << std::endl;
        std::uint64_t cnt_pkt = 0;
        //std::cout << cnt_flow_num << std::endl;
        // timestamp saddr daddr protocol sport dport is_syn
        while (getline(trace_in, temp)) {
            if (cnt_pkt % 1000000 == 0) {
                std::cout << cnt_pkt << std::endl;
            }
            std::vector<std::string> field;
            std::size_t pos = temp.find(" ");
            while (pos != std::string::npos) {

                field.push_back(temp.substr(0, pos));
                temp = temp.substr(pos + 1);
                pos = temp.find(" ");
                //std::cout << "here" << temp << std::endl;
            }
            field.push_back(temp);
            uint64_t time = 0;
            uint32_t saddr = 0;
            uint32_t daddr = 0;
            uint8_t protocol = 0;
            uint16_t sport = 0;
            uint16_t dport = 0;
            uint32_t tot_len = 0;
            int is_syn = 0;
            int is_ack = 0;
            std::from_chars(field[0].data(), field[0].data() + field[0].size(), time);
            std::from_chars(field[1].data(), field[1].data() + field[1].size(), saddr);
            std::from_chars(field[2].data(), field[2].data() + field[2].size(), daddr);
            std::from_chars(field[3].data(), field[3].data() + field[3].size(), protocol);
            std::from_chars(field[4].data(), field[4].data() + field[4].size(), sport);
            std::from_chars(field[5].data(), field[5].data() + field[5].size(), dport);
            std::from_chars(field[6].data(), field[6].data() + field[6].size(), tot_len);
            std::from_chars(field[7].data(), field[7].data() + field[7].size(), is_syn);
            std::from_chars(field[8].data(), field[8].data() + field[8].size(), is_ack);
            //std::cout << saddr << " " << daddr << " " << (uint32_t)protocol << " " << sport << " " << dport << " " << is_syn << std::endl;
            T flow = T(saddr, daddr, protocol, sport, dport);
            //std::cout << flow.saddr << " " << flow.daddr << std::endl;
            //std::cout << (uint32_t)flow.saddr << " " << (uint32_t)flow.daddr << std::endl;
            if (flow_set.find(flow) == flow_set.end()) {
                  
                flow_set[flow] = cnt_flow_num;
                //if (std::is_same<T, Tuple_5>::value) {
                //    map_fout << saddr << " " << daddr << " " << (uint32_t)protocol << " " << sport << " " << dport << ":" << cnt_flow_num << std::endl;
                //    //std::cout << "here" << std::endl;
                //}
                //else if (std::is_same<T, TupleSrcDst>::value) {
                //    map_fout << saddr << " " << daddr << ":" << cnt_flow_num << std::endl;
                //}
                
                //std::cout << cnt_flow_num << std::endl;
                cnt_flow_num++;
            }
            std::uint32_t flow_id = flow_set[flow];
            fout << (uint32_t)flow_id << " " << (uint32_t)is_syn << " " << (uint32_t)is_ack << std::endl;
            cnt_pkt++;
        }
        trace_in.close();
        fout.close();
        //map_fout.close();

    }
    
    void load_trace(std::string file_name) {
        std::ifstream trace_in;
        trace_in.open(file_name, std::ios_base::in);
        std::cout << "load trace file is: " << file_name << std::endl;
        if (!trace_in.is_open()) {
            std::cout << "open trace   error" << std::endl;
            return;
        }
        std::string temp;
        std::cout << "load trace" << std::endl;
        std::uint64_t cnt_pkt = 0;
        //std::cout << cnt_flow_num << std::endl;
        // timestamp saddr daddr protocol sport dport is_syn
        while (getline(trace_in, temp)) {
            if (cnt_pkt % 1000000 == 0) {
                std::cout << cnt_pkt << std::endl;
            }
            std::vector<std::string> field;
            std::size_t pos = temp.find(" ");
            while (pos != std::string::npos) {

                field.push_back(temp.substr(0, pos));
                temp = temp.substr(pos + 1);
                pos = temp.find(" ");
                //std::cout << temp << std::endl;
            }
            field.push_back(temp);
            //std::cout << saddr << " " << daddr << " " << (uint32_t)protocol << " " << sport << " " << dport << " " << is_syn << std::endl;
            uint32_t flow_id = 0;
            uint32_t is_syn = 0;
            uint32_t is_ack = 0;
            std::from_chars(field[0].data(), field[0].data() + field[0].size(), flow_id);
            std::from_chars(field[1].data(), field[1].data() + field[1].size(), is_syn);
            std::from_chars(field[2].data(), field[2].data() + field[2].size(), is_ack);

            auto pkt = Packet{};
            pkt.m_key = flow_id;
            pkt.is_syn = is_syn;
            pkt.is_ack = is_ack;
            std::srand(flow_id);
            pkt.m_top_k_addr = { (std::uint32_t)std::rand(), (std::uint32_t)std::rand(), (std::uint32_t)std::rand() };
            packets.push_back(pkt);
           
            cnt_pkt++;
        }
        trace_in.close();
        std::cout << "trace_loaded" << std::endl;

        seq_out.open("./packet_seq.txt", std::ios_base::out);

        pkt_clk.open(parent_path + "/packet_clock.txt", std::ios_base::out);
        end_clk.open(parent_path + "/end_clock.txt", std::ios_base::out);

        //for (int i = 0; i < N; i++) {
        //    fout << m_out_pkt_seq[i] << std::endl;
        //}
        //std::ofstream fout;
        //fout.open("./seq1.txt", std::ios_base::out);
        //for (auto pkt : packets) {
        //    fout << (uint32_t)pkt.m_key << " " << (uint32_t)pkt.m_top_k_addr[0] << " " << (uint32_t)pkt.m_top_k_addr[1] << " " << (uint32_t)pkt.m_top_k_addr[2] << std::endl;
        //}
        //fout.close();
    }

    std::vector<bool> distributePoints(int n, int total_positions = 100) {
        //assert(n > 0 && n <= total_positions);

        std::vector<bool> positions(total_positions, false);

        for (int i = 0; i < n; ++i) {
            int pos = static_cast<int>(std::round(static_cast<double>(i * total_positions) / n));
            positions[pos] = true;
        }

        return positions;
    }

    Packet next() {
        auto pkt = Packet{};
        if (IF_CLK == 0) {
            if (cnt_pkt_snd == packets.size()) {
                std::cout << "all pkts sent at" << g_clock << std::endl;
                end_clk << g_clock << std::endl;
            }
            else {
                if (packet_guider[round_pointer]) {
                    pkt = packets.at(cnt_pkt_snd);
                    cnt_pkt_snd++;

                    round_pointer = (round_pointer + 1) % 100;

                    pkt_clk << pkt.m_key << " " << g_clock << std::endl;

                    return pkt;
                }
                else {
                    round_pointer = (round_pointer + 1) % 100;
                    return Packet{};
                }
            }
        }
        else {
            if (packets.at(cnt_pkt_snd).clock_time == g_clock) {
                pkt = packets.at(cnt_pkt_snd);
                pkt_clk << pkt.m_key << " " << g_clock << std::endl;
                //std::cout << g_clock << " : " << pkt.clock_time << " " <<  pkt.m_key << std::endl;
                cnt_pkt_snd++;
                if (cnt_pkt_snd == packets.size()) {
                    std::cout << "all pkts sent at " << g_clock << std::endl;
                    end_clk << g_clock << std::endl;
                    end_clk.close();
                }
            }
            else {
                return Packet{};
            }
        }
        return pkt;
    } 

    void reset() {
        cnt_pkt_snd = 0;
    }

    bool is_snd_over() {
        return cnt_pkt_snd == packets.size();
    }
};