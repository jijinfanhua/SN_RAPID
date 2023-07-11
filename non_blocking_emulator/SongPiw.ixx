export module rapid.SongPiw;

import rapid.Packet;
import std;

export template <std::byte PEER_MASK, size_t CLOCK_MAX, size_t K = 2>
class SongPiw {
    constexpr const static std::byte m_peer_mask { PEER_MASK };
    constexpr const static size_t m_clock_max { CLOCK_MAX };

    struct DirtyState {
        bool m_exist { false };
        bool m_dirty { false };
        std::byte m_seq_id {};
        int m_update_time {};
        DirtyState() = default;
    };

    //std::array<DirtyState, K> m_dirty_state {};
    std::map<std::uint32_t, DirtyState> m_dirty_state{};
    Packet m_pkt_to_write {};
    std::ofstream piw_info_out;
    size_t cnt_cycle{ 0 };

public:
    SongPiw(std::string sub_dir, size_t type) {
        piw_info_out.open(sub_dir + "\\piw_info_" + std::to_string(type) + ".txt", std::ios_base::out);
        //std::cout << "clock max " << m_clock_max << std::endl;
    };

    Packet get_pkt_to_write() const {
        return m_pkt_to_write;
    }

    std::pair< std::pair<Packet, Packet>, Packet> next(Packet&& pkt)
    {
        cnt_cycle++;
        //std::cout << m_clock_max << std::endl;
        m_pkt_to_write = Packet {};
        if (pkt.is_empty()) {
            //piw_info_out << "cycle: " << cnt_cycle << " bp_pkt: " << pkt.m_key << " wb_pkt: " << pkt.m_key << " pp_pkt: " << pkt.m_key << std::endl;
            return { {pkt, pkt}, pkt };
        } else {
            if (m_dirty_state.find(pkt.m_key) == m_dirty_state.end()) {
                m_dirty_state[pkt.m_key] = DirtyState{};
            }
            auto& state = m_dirty_state[pkt.m_key];
            if (state.m_update_time + m_clock_max < g_clock) {
                // expired
                state.m_exist = false;
            }
            if (!state.m_exist) { // ��ǰ��������
                if (pkt.is_write_back_packet(m_peer_mask)) { // ��д״̬�����ݰ�
                    if constexpr (OUTPUT) {
                        std::cout << "WRITE = " << pkt << std::endl; 
                    }
                    state.m_exist = true; //����������Ϊ�࣬�����õ��ð�����һ��id
                    state.m_dirty = true;
                    state.m_seq_id = next_seq_id(pkt.get_seq_id());
                    state.m_update_time = g_clock; // ��¼����ʱ��
                    m_pkt_to_write = pkt; // ������Ҫд�ص����ݰ�
                }
                piw_info_out << "cycle: " << cnt_cycle << " bp_pkt: " << 0 << " wb_pkt: " << m_pkt_to_write.m_key << " pp_pkt: " << pkt.m_key << std::endl;
                return { { Packet {}, std::move(m_pkt_to_write)}, std::move(pkt) };
            } else {
                if (state.m_dirty) { // ��ǰ�����ڲ��������
                    state.m_update_time = g_clock;
                    if (auto seq_id { pkt.get_seq_id() }; seq_id == state.m_seq_id) { // ��������id�����ݰ�id��ͬ������Ϊ���࣬Ҳ���Ǵ�dTableɾ��
                        state.m_dirty = false;
                    }
                } else { // ��ǰ�����ڵ���״̬����
                    if (auto seq_id { pkt.get_seq_id() }; seq_id == state.m_seq_id) {
                        state.m_seq_id = next_seq_id(state.m_seq_id);
                        if (pkt.is_write_back_packet(m_peer_mask)) { // Ҫд״̬
                            state.m_update_time = g_clock;
                            state.m_dirty = true;
                            m_pkt_to_write = pkt;
                        }
                        piw_info_out << "cycle: " << cnt_cycle << " bp_pkt: " << 0 << " wb_pkt: " << m_pkt_to_write.m_key << " pp_pkt: " << pkt.m_key << std::endl;
                        return { { Packet {}, std::move(m_pkt_to_write)}, std::move(pkt)}; // ��Ҫд��״̬�������ݰ����ᱻ�ͻ�
                    } else {
                        state.m_update_time = g_clock;
                    }
                }
                pkt.is_resubmit = true;
                piw_info_out << "cycle: " << cnt_cycle << " bp_pkt: " << pkt.m_key << " wb_pkt: " << 0 << " pp_pkt: " << 0 << std::endl;
                return { {std::move(pkt), Packet{}}, Packet {} };
            }
        }
    }
};