export module rapid.Packet;

import std;

export int g_clock{ 0 };
export constexpr const bool OUTPUT{ false };

export class Packet {
    static int s_id;

public:
    int m_id { 0 };
    std::uint32_t m_key { 0 };

    std::uint32_t clock_time { 0 };

    std::byte m_backward_tag_bitmap { 0 };
    std::byte m_write_back_bitmap { 0 };
    int enter_timestamp{ 0 };
    bool is_syn{ false };
    bool is_ack{ false };
    bool is_resubmit{ false };
    int m_cnt_syn{ 0 };
    std::array<uint32_t, 3> m_top_k_addr {};
    Packet() = default;
    Packet(std::uint32_t key)
        : m_id(1)
        , m_key(key)
    {
    }
    Packet(std::uint32_t key, std::byte write_back_bitmap)
        : m_id(1)
        , m_key(key)
        , m_write_back_bitmap(write_back_bitmap)
    {
    }
    constexpr bool is_empty() const
    {
        return m_key == 0;
    }
    constexpr bool is_backward_packet(std::byte dest_mask) const
    {
        return m_backward_tag_bitmap >= dest_mask;
    }
    constexpr bool is_write_back_packet(std::byte peer_mask) const
    {
        return (m_write_back_bitmap & peer_mask) != std::byte { 0 };
    }
    void set_backward_tag(std::byte bitmap)
    {
        m_backward_tag_bitmap |= bitmap;
    }

    // ����backward tag bitmap���ڲ�overlapʱû���ã�
    void set_seq_id(std::byte seq_id)
    {
        m_backward_tag_bitmap = seq_id;
    }
    std::byte get_seq_id()
    {
        return m_backward_tag_bitmap;
    }

    void set_timestamp() {
        enter_timestamp = g_clock;
    }

    int get_time_offset() {
        return g_clock - enter_timestamp;
    }
};

int Packet::s_id { 0 };

export std::byte next_seq_id(std::byte seq_id) { // 1-255
    int next { static_cast<int>(seq_id) + 1 };
    next %= 256;
    if (next == 0) {
        next = 1;
    }
    std::byte result { static_cast<std::byte>(next) };
    return result;
}

export std::byte last_seq_id(std::byte seq_id) {
    int last{ static_cast<int>(seq_id) - 1 };
    last %= 256;
    if (last == 0) {
        last = 255;
    }
    std::byte result {static_cast<std::byte>(last)};
    return result;
}

export std::ostream& operator<<(std::ostream& os, const Packet& pkt)
{
    os << "Packet { id: " << pkt.m_id << ", key: " << pkt.m_key << ", backward_tag_bitmap: " << static_cast<int>(pkt.m_backward_tag_bitmap) << ", write_back_bitmap: " << static_cast<int>(pkt.m_write_back_bitmap) << " }";
    return os;
}

