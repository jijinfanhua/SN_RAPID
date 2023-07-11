export module rapid.ImprovedSongPipeline;

import rapid.Packet;
import rapid.Device;
import rapid.PacketQueue;
import rapid.SongReadWritePeer;
import std;

export template <size_t N, size_t K = 2, size_t PROC_NUM = 4, size_t FRONT_BUFFER_SIZE = 1024>
class ImprovedSongPipeline : public Device {
    PacketQueue<FRONT_BUFFER_SIZE> m_front;
    SongReadWritePeer<N, 0, PROC_NUM - 1, K> m_peer;
    std::ofstream front_buffer_info_out;
   // std::ofstream time_elapse_info;

    size_t ARCH_TYPE{ 0 };
    size_t BUBBLE_PERIOD{ 0 };
    std::string SUB_DIR;

    size_t cnt_cycle{ 0 };

    //size_t total_process_time{ 0 };

public:
    size_t front_buffer_cnt_rcv{ 0 };
    size_t front_buffer_cnt_snd{ 0 };
    size_t peer_cnt_rcv{ 0 };
    size_t peer_cnt_snd{ 0 };

    ImprovedSongPipeline() = default;
    ImprovedSongPipeline(size_t type): m_peer(type) {}
    ImprovedSongPipeline(size_t type, size_t period) : m_peer(type, period) {
        ARCH_TYPE = type;
        BUBBLE_PERIOD = period;
    }
    ImprovedSongPipeline(size_t type, size_t period, std::string sub_dir) : m_peer(type, period, sub_dir) {
        ARCH_TYPE = type;
        BUBBLE_PERIOD = period;
        SUB_DIR = sub_dir;
    }
    ImprovedSongPipeline(size_t type, std::string sub_dir) : m_peer(type, sub_dir) {
        ARCH_TYPE = type;
        SUB_DIR = sub_dir;
    }
    void initialize()
    {
        std::string front_buffer_name = SUB_DIR + "fb_size_i_" + std::to_string(ARCH_TYPE) + "_j_" + std::to_string(BUBBLE_PERIOD) + ".txt";
        front_buffer_info_out.open(front_buffer_name, std::ios_base::out);
       // time_elapse_info.open("./time_out.txt", std::ios_base::out);
        // no action
    }
    Packet next(Packet&& pkt) override
    {
        if (!pkt.is_empty()) {
            front_buffer_cnt_rcv++;
        }
        front_buffer_info_out << "cycle: " << cnt_cycle++ << " front size: " << m_front.size() << std::endl;
        if (!pkt.is_empty()) {
            //pkt.set_timestamp();
            m_front.enqueue(std::move(pkt));
            //std::cout << "enqueue" << std::endl;
        }
        
        Packet front_pkt {};
        if (!m_peer.is_full()) {
            front_pkt = m_front.dequeue();
        }
        if (!front_pkt.is_empty()) {
            front_buffer_cnt_snd++;
            peer_cnt_rcv++;
        }
        auto peer_pkt = m_peer.next(std::move(front_pkt));
        if (!peer_pkt.is_empty()) {
            //size_t process_time = peer_pkt.get_time_offset() + 2;
            //total_process_time += process_time;
            //time_elapse_info << "packet num: " << peer_cnt_snd << " elapse time: " << process_time << std::endl;
            peer_cnt_snd++;
        }
        return peer_pkt;
    }

    void print_info() {
        std::cout << "front_buffer rcv: " << front_buffer_cnt_rcv << " snd: " << front_buffer_cnt_snd << std::endl;
        std::cout << "wr_peer rcv " << peer_cnt_rcv << " snd: " << peer_cnt_snd << std::endl;
        //std::cout << "average_elapse_time: " << get_average_elapse_time() << std::endl;
        m_peer.print_info();
    }

    //size_t get_average_elapse_time() {
    //    return total_process_time / peer_cnt_snd;
    //}
};