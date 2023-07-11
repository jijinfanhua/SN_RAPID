export module rapid.SeqIdMarker;

import rapid.Packet;
import rapid.Device;
import std;

export template <size_t K = 2>
class SeqIdMarker : public Device {
    //std::array<std::byte, K> m_seq_id {};
    std::map<std::uint32_t, std::byte> m_seq_id {};
public:
    SeqIdMarker() = default;

    std::byte get_seq_id(std::uint32_t key){
        if (m_seq_id.find(key) == m_seq_id.end()) {
            return std::byte(0);
        }
        else {
            return m_seq_id[key];
        }
        
    }

    Packet next(Packet&& pkt) override {
        if (pkt.is_empty() || pkt.get_seq_id() != std::byte(0)) {
            return pkt;
        }
        else {
            if (m_seq_id.find(pkt.m_key) != m_seq_id.end()) { // 如果存在
                m_seq_id[pkt.m_key] = next_seq_id(m_seq_id[pkt.m_key]);
            }
            else {
                m_seq_id[pkt.m_key] = std::byte(1); // 如果不存在，则将其id设置为1
            }
            
            pkt.set_seq_id(m_seq_id[pkt.m_key]); // 将这个数据包设置成最新的id
            return pkt;
        }
    }
};