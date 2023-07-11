export module rapid.SongPir;

import rapid.Packet;
import rapid.PacketQueue;
import rapid.SeqIdMarker;
import rapid.VirtualPipeline;
import rapid.RoundRobinQueue;
import std;

export
class BasePir {

    virtual int find_nxt_schedule() { return 0; }
    virtual Packet schedule() { return Packet{}; }
    virtual void enqueue(Packet&& pkt) {}

public:
    virtual Packet next(Packet&& incoming_pkt, Packet&& resubmitted_pkt) {
        return incoming_pkt;
    };
    virtual bool is_full() const { return false; }
    virtual bool is_full(Packet pkt) const { return false; }
    virtual void write_cam(Packet&& pkt) {}
    virtual void db_info() {}
    virtual void print_info() {}

    Packet call_next(Packet&& incoming_pkt, Packet&& resubmitted_pkt) {
        return next(std::move(incoming_pkt), std::move(resubmitted_pkt));
    }

    bool call_is_full() const {
        return is_full();
    }
    bool call_is_full(Packet pkt) const { return is_full(pkt); }
    void call_write_cam(Packet&& pkt) { write_cam(std::move(pkt)); }
    void call_db_info() { db_info(); }
    void call_print_info() { print_info(); }
};

export template <size_t N, size_t K = 2>
class SongPir : public BasePir {
    PacketQueue<N> m_incoming_buffer;
    SeqIdMarker<K> m_seq_id_marker;
    //std::bitset<K> m_dirty_cam;
    std::set<std::uint32_t> m_dirty_cam;
    //std::bitset<K> m_schedule_cam;
    std::set<std::uint32_t> m_schedule_cam;
    //std::array<PacketQueue<N>, K> m_dirty_buffer;
    std::map<std::uint32_t, PacketQueue<N>> m_dirty_buffer;
    int m_buffer_size{ 0 };
    RoundRobinQueue<K> m_schedule_queue;


    int find_nxt_schedule() override {
        auto size = m_schedule_queue.size();
        for (int i = 0; i < size; i++) {
            auto x = m_schedule_queue.dequeue();
            //if (x != 0) {
            //    return x;
            //} 
            if (m_dirty_buffer.find(x) == m_dirty_buffer.end()) {
                m_schedule_cam.erase(x);
                m_dirty_cam.erase(x);
                std::cout << "should not happen" << std::endl;
            }
            else if (m_dirty_buffer[x].is_empty()) {
                m_dirty_buffer.erase(x);
                m_dirty_cam.erase(x);
                m_schedule_cam.erase(x);
            }
            else {
                return x;
            }
        }
        return 0;
    }

    Packet schedule() override {
        //auto key { m_schedule_queue.dequeue() };
        auto key{ find_nxt_schedule() };
        if (key == 0) {
            return Packet{};
        }
        dirty_buffer_snd++;
        auto pkt{ m_dirty_buffer[key].dequeue() };
        --m_buffer_size;

        //schedule_fout << "cycle: " << cnt_cycle << " schedule flow: " << key << " pkt key: " << pkt.m_key;
        if (m_dirty_buffer[key].is_empty()) {
            m_dirty_cam.erase(key);
            m_schedule_cam.erase(key);
            schedule_fout << " empty" << std::endl;
            // std::cout << "empty" << std::endl;
        }
        else {
            m_schedule_queue.enqueue(key);
            schedule_fout << " not empty" << std::endl;
            //std::cout << "not empty" << std::endl;
        }
        
        return pkt;
        //return Packet{};
    }

    void enqueue(Packet&& pkt) override {
        if (m_buffer_size < N) {
            ++m_buffer_size;
            dirty_buffer_rcv++;
            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
            if (m_dirty_buffer.find(pkt.m_key) == m_dirty_buffer.end()) {
                m_dirty_buffer.insert(std::make_pair(pkt.m_key, PacketQueue<N> {}));
            }
            m_dirty_buffer[pkt.m_key].enqueue(std::move(pkt));
        }
        else {
            ++m_drop_packet_count;
            std::cout << "shouldn't be here" << std::endl;
        }
    }

    std::ofstream ib_fout;
    std::ofstream db_fout;
    size_t cnt_cycle{ 0 };
    std::ofstream schedule_fout;
    std::ofstream wb_fout;
    std::ofstream bp_fout;
    std::ofstream wbb_fout;

    size_t incoming_buffer_rcv{ 0 };
    size_t incoming_buffer_snd{ 0 };
    size_t incoming_buffer_to_dirty_buffer{ 0 };
    size_t incoming_buffer_to_pipeline{ 0 };
    size_t dirty_buffer_rcv{ 0 };
    size_t dirty_buffer_snd{ 0 };

public:
    int m_drop_packet_count { 0 };
    //SongPir() = default;
    SongPir() {
        ib_fout.open("ib_size.txt", std::ios_base::out);
        db_fout.open("db_size.txt", std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        
        wbb_fout.open("./packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list" << std::endl;
    }

    SongPir(size_t type, size_t period) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + ".txt";
        ib_fout.open("ib_size" + suffix, std::ios_base::out);
        db_fout.open("db_size" + suffix, std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info" + suffix, std::ios_base::out);
        bp_fout.open("bp_info" + suffix, std::ios_base::out);

        wbb_fout.open("./packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list" << std::endl;
    }

    SongPir(size_t type, size_t period, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + ".txt";
        ib_fout.open(sub_dir + "ib_size" + suffix, std::ios_base::out);
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);

        wbb_fout.open(sub_dir + "packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list" << std::endl;
    }

    SongPir(size_t type, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + ".txt";
        ib_fout.open(sub_dir + "ib_size" + suffix, std::ios_base::out);
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);

        wbb_fout.open(sub_dir + "packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list" << std::endl;
    }


    bool is_full() const override {
        return m_incoming_buffer.is_full();
    }

    bool is_full(Packet pkt) const override {
        return m_incoming_buffer.is_full();
    }

    void write_cam(Packet&& pkt) override {
        cnt_cycle++;
        int expect_id = (int)m_seq_id_marker.get_seq_id(pkt.m_key);
        //wb_fout << "cycle: " << cnt_cycle << " flow id: " << pkt.m_key  << " is_write_back: " << !pkt.is_empty() << " seq id: " << (int)pkt.get_seq_id() << " expect id: " << expect_id << std::endl;
        if (!pkt.is_empty()) {
            if (pkt.get_seq_id() != m_seq_id_marker.get_seq_id(pkt.m_key)) {
                //std::cout << "cycle: " << cnt_cycle << " error" << std::endl;
                m_dirty_cam.insert(pkt.m_key);
                m_schedule_cam.erase(pkt.m_key);
                m_schedule_queue.remove(pkt.m_key);
            }
        }
    }

    // resubmit > schedule > incoming
    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {
        ib_fout << "cycle: " << cnt_cycle << " incoming_buffer_size: " << m_incoming_buffer.size() << std::endl;
        db_fout << "cycle: " << cnt_cycle << " dirty_buffer_size: " << m_buffer_size << " schedule_queue_size: " << m_schedule_queue.size() << std::endl;
        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;
        if (!incoming_pkt.is_empty()) {
            incoming_buffer_rcv++;
            m_incoming_buffer.enqueue(std::move(incoming_pkt)); 
        }
        bool incoming_packet_valid { false };
        if (const auto& pkt { m_incoming_buffer.front() }; !pkt.is_empty()) {
            if (m_dirty_cam.find(pkt.m_key) != m_dirty_cam.end()) {
                if (m_buffer_size < N) {
                    incoming_buffer_to_dirty_buffer++;
                    enqueue(m_incoming_buffer.dequeue());
                }
            } else {
                incoming_packet_valid = true;    
            }
        }
        if (!resubmit_pkt.is_empty()) {
            if (resubmit_pkt.get_seq_id() ==  m_seq_id_marker.get_seq_id(resubmit_pkt.m_key)) {
                // std::cout << "seq get" << std::endl;
                if (m_schedule_cam.find(resubmit_pkt.m_key) == m_schedule_cam.end()) {
                    if (!(m_dirty_buffer.find(resubmit_pkt.m_key) == m_dirty_buffer.end() || m_dirty_buffer[resubmit_pkt.m_key].is_empty())) {
                        m_schedule_queue.enqueue(resubmit_pkt.m_key);
                        m_schedule_cam.insert(resubmit_pkt.m_key);
                    }
                    else {
                        m_dirty_cam.erase(resubmit_pkt.m_key);
                        m_dirty_buffer.erase(resubmit_pkt.m_key);
                    }
                }
            }
            return resubmit_pkt;
        } else {
            auto scheduled_pkt { schedule() };
            if (!scheduled_pkt.is_empty()) {
                return m_seq_id_marker.next(std::move(scheduled_pkt));
            } else if (incoming_packet_valid) {
                incoming_buffer_to_pipeline++;
                return m_seq_id_marker.next(m_incoming_buffer.dequeue());
            }
        }
        return Packet {};
    }

    void print_info() override {
        std::cout << "incoming_buffer rcv: " << incoming_buffer_rcv << " to dirty buffer: " << incoming_buffer_to_dirty_buffer << " to pipeline: " << incoming_buffer_to_pipeline  << std::endl;
        std::cout << "dirty_buffer rcv " << dirty_buffer_rcv << " snd: " << dirty_buffer_snd << std::endl;
    }
};

export template <size_t N, size_t K = 2, size_t Q_NUM = 1>
class SongPir_SQ : public BasePir {
    // PacketQueue<N> m_incoming_buffer;
    SeqIdMarker<K> m_seq_id_marker;
    //std::bitset<K> m_dirty_cam;
    //std::bitset<K> m_schedule_cam;
    std::set<std::uint32_t> m_dirty_cam;
    std::set<std::uint32_t> m_schedule_cam;
    size_t m_size_per_queue{ 2 * N / Q_NUM };
    std::array<PacketQueue< 2 * N / Q_NUM>, Q_NUM> m_multi_queue;
    size_t m_schedule_pos{ 0 };
    std::map<std::uint32_t, size_t> m_cnt_buffered_pkt;
    //std::array<size_t, K> m_cnt_buffered_pkt;


    int find_nxt_schedule() override {
        int pos = -1;
        for (int i = 0; i < Q_NUM; i++) {
            if (!m_multi_queue.at(m_schedule_pos).is_empty()) {
                auto& pkt = m_multi_queue.at(m_schedule_pos).front();
                if (!(m_dirty_cam.find(pkt.m_key) != m_dirty_cam.end() && m_schedule_cam.find(pkt.m_key) == m_schedule_cam.end())) {
                    //if ( !(m_dirty_cam.test(pkt.m_key) && !m_schedule_cam.test(pkt.m_key))) { // 队头包可以被调
                    pos = m_schedule_pos;
                }
                break;
            }
            m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
        }
        m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
        //auto size = m_schedule_queue.size();
        //for (int i = 0; i < size; i++) {
        //    auto x = m_schedule_queue.dequeue();
        //    //if (x != 0) {
        //    //    return x;
        //    //} 
        //    if (m_dirty_buffer.at(x).is_empty()) {
        //        m_dirty_cam.reset(x);
        //        m_schedule_cam.reset(x);
        //    }
        //    else {
        //        return x;
        //    }
        //}
        return pos;
    }

    Packet schedule() override {
        //auto key { m_schedule_queue.dequeue() };
        auto key{ find_nxt_schedule() };
        if (key == -1) { //所有队列都不能被调度（或者为空）
            return Packet{};
        }
        multi_queue_snd++;
        auto pkt{ m_multi_queue.at(key).dequeue() };
        m_cnt_buffered_pkt[pkt.m_key]--;

        //schedule_fout << "cycle: " << cnt_cycle << " schedule queue: " << key << " flow id: " << pkt.m_key << std::endl;;
        if (m_cnt_buffered_pkt[pkt.m_key] == 0) {
            m_dirty_cam.erase(pkt.m_key);
            m_schedule_cam.erase(pkt.m_key);
            m_cnt_buffered_pkt.erase(pkt.m_key);
            // schedule_fout << " empty" << std::endl;
            // std::cout << "empty" << std::endl;
        }

        return pkt;
    }

    void enqueue(Packet&& pkt) override {
        if (!m_multi_queue.at(pkt.m_key % Q_NUM).is_full()) {
            multi_queue_rcv++;
            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
            m_multi_queue.at(pkt.m_key % Q_NUM).enqueue(std::move(pkt));
            if (m_cnt_buffered_pkt.find(pkt.m_key) != m_cnt_buffered_pkt.end()) {
                m_cnt_buffered_pkt[pkt.m_key]++;
            }
            else {
                m_cnt_buffered_pkt[pkt.m_key] = 1;
            }
        }
        else {
            ++m_drop_packet_count;
            std::cout << "shouldn't be here" << std::endl;
        }
    }

    std::ofstream db_fout;
    size_t cnt_cycle{ 0 };
    std::ofstream schedule_fout;
    std::ofstream wb_fout;
    std::ofstream bp_fout;

    size_t multi_queue_rcv{ 0 };
    size_t multi_queue_snd{ 0 };

public:
    int m_drop_packet_count { 0 };
    //SongPir() = default;
    SongPir_SQ() {
        db_fout.open("db_size.txt", std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    SongPir_SQ(size_t type, size_t period) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + ".txt";
        db_fout.open("db_size" + suffix, std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    SongPir_SQ(size_t type, size_t period, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + ".txt";
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info.txt", std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info.txt", std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    SongPir_SQ(size_t type, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + ".txt";
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info.txt", std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info.txt", std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }


    bool is_full() const override {
        std::cout << "should not use it to tell if is full" << std::endl;
        return false;
    }

    bool is_full(Packet pkt) const override {
        return !pkt.is_empty() && m_multi_queue.at(pkt.m_key % Q_NUM).is_full();
    }

    void write_cam(Packet&& pkt) override {
        cnt_cycle++;
        int expect_id = (int)m_seq_id_marker.get_seq_id(pkt.m_key);
        //wb_fout << "cycle: " << cnt_cycle << " flow id: " << pkt.m_key  << " is_write_back: " << !pkt.is_empty() << " seq id: " << (int)pkt.get_seq_id() << " expect id: " << expect_id << std::endl;
        if (!pkt.is_empty()) {
            if (pkt.get_seq_id() != m_seq_id_marker.get_seq_id(pkt.m_key)) {
                //std::cout << "cycle: " << cnt_cycle << " error" << std::endl;
                m_dirty_cam.insert(pkt.m_key);
                m_schedule_cam.erase(pkt.m_key);
                //m_schedule_queue.remove(pkt.m_key);
            }
        }
    }

    // resubmit > schedule > incoming
    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {
        db_info_fout();
        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;

        if (!incoming_pkt.is_empty()) {
            //进多队列调度（不管是不是dirty流）
            if (!m_multi_queue.at(incoming_pkt.m_key % Q_NUM).is_full()) { //这里应该提前判断
                enqueue(std::move(incoming_pkt));
            }
            else {
                std::cout << "cycle: " << cnt_cycle << " m_multi_queue_" << incoming_pkt.m_key % Q_NUM << " is full" << std::endl;
            }
        }
        if (!resubmit_pkt.is_empty()) {
            if (resubmit_pkt.get_seq_id() == m_seq_id_marker.get_seq_id(resubmit_pkt.m_key)) {
                // std::cout << "seq get" << std::endl;
                if (m_schedule_cam.find(resubmit_pkt.m_key) == m_schedule_cam.end()) {
                    if (m_cnt_buffered_pkt.find(resubmit_pkt.m_key) != m_cnt_buffered_pkt.end() && m_cnt_buffered_pkt[resubmit_pkt.m_key] != 0) {
                        m_schedule_cam.insert(resubmit_pkt.m_key);
                    }
                    else {
                        m_dirty_cam.erase(resubmit_pkt.m_key);
                    }
                }
            }
            return resubmit_pkt;
        }
        else {
            auto scheduled_pkt{ schedule() }; //schedule也需要修改
            if (!scheduled_pkt.is_empty()) {
                return m_seq_id_marker.next(std::move(scheduled_pkt));
            }
        }
        return Packet{};
    }

    void print_info() override {
        std::cout << "multi_queue rcv " << multi_queue_rcv << " snd: " << multi_queue_snd << std::endl;
    }

    void db_info_fout() {
        db_fout << "cycle: " << cnt_cycle;
        for (int i = 0; i < Q_NUM; i++) {
            db_fout << " queue_" << i << ": " << m_multi_queue.at(i).size();
        }
        db_fout << std::endl;
    }
};

export template <size_t N, size_t K = 2, size_t Q_NUM = 1>
class SongPir_Direct : public BasePir {
    // PacketQueue<N> m_incoming_buffer;
    SeqIdMarker<K> m_seq_id_marker;
    size_t m_size_per_queue{ 2 * N / Q_NUM };
    std::array<PacketQueue< 2 * N / Q_NUM>, Q_NUM> m_multi_queue;
    size_t m_schedule_pos{ 0 };
    //std::map<std::uint32_t, size_t> m_cnt_buffered_pkt;
    //std::array<size_t, K> m_cnt_buffered_pkt;


    int find_nxt_schedule() override {
        int pos = -1;
        for (int i = 0; i < Q_NUM; i++) {
            if (!m_multi_queue.at(m_schedule_pos).is_empty()) {
                pos = m_schedule_pos;
                break;
            }
            m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
        }
        m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;

        return pos;
    }

    Packet schedule() override {
        auto key{ find_nxt_schedule() };
        if (key == -1) { //所有队列都不能被调度（或者为空）
            return Packet{};
        }
        multi_queue_snd++;
        auto pkt{ m_multi_queue.at(key).dequeue() };

        return pkt;
    }

    void enqueue(Packet&& pkt) override {
        if (!m_multi_queue.at(pkt.m_key % Q_NUM).is_full()) {
            multi_queue_rcv++;
            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
            m_multi_queue.at(pkt.m_key % Q_NUM).enqueue(std::move(pkt));
            /*if (m_cnt_buffered_pkt.find(pkt.m_key) != m_cnt_buffered_pkt.end()) {
                m_cnt_buffered_pkt[pkt.m_key]++;
            }
            else {
                m_cnt_buffered_pkt[pkt.m_key] = 1;
            }*/
        }
        else {
            ++m_drop_packet_count;
            std::cout << "shouldn't be here" << std::endl;
        }
    }

    std::ofstream db_fout;
    size_t cnt_cycle{ 0 };
    std::ofstream schedule_fout;
    std::ofstream wb_fout;
    std::ofstream bp_fout;

    size_t multi_queue_rcv{ 0 };
    size_t multi_queue_snd{ 0 };

public:
    int m_drop_packet_count { 0 };
    //SongPir() = default;
    SongPir_Direct() {
        db_fout.open("db_size.txt", std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        std::cout << "direct single queue" << std::endl;
    }

    SongPir_Direct(size_t type, size_t period) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_" + std::to_string(Q_NUM) + "q.txt";
        db_fout.open("db_size" + suffix, std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info" + suffix, std::ios_base::out);
        bp_fout.open("bp_info" + suffix, std::ios_base::out);
        std::cout << "direct single queue" << std::endl;
    }

    SongPir_Direct(size_t type, size_t period, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_" + std::to_string(Q_NUM) + "q.txt";
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);
        std::cout << "direct single queue" << std::endl;
    }

    SongPir_Direct(size_t type, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_" + std::to_string(Q_NUM) + "q.txt";
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);
        std::cout << "direct single queue" << std::endl;
    }



    bool is_full() const override {
        std::cout << "should not use it to tell if is full" << std::endl;
        return false;
    }

    bool is_full(Packet pkt) const override {
        return !pkt.is_empty() && m_multi_queue.at(pkt.m_key % Q_NUM).is_full();
    }

    void write_cam(Packet&& pkt) override {
        cnt_cycle++;
    }

    // resubmit > incoming
    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {
        db_info_fout();
        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;

        if (!incoming_pkt.is_empty()) {
            //进多队列调度（不管是不是dirty流）
            if (!m_multi_queue.at(incoming_pkt.m_key % Q_NUM).is_full()) { //这里应该提前判断
                enqueue(std::move(incoming_pkt));
            }
            else {
                std::cout << "cycle: " << cnt_cycle << " m_multi_queue_" << incoming_pkt.m_key % Q_NUM << " is full" << std::endl;
            }
        }
        if (!resubmit_pkt.is_empty()) { // 优先调度resubmit packet
            schedule_fout << "cycle: " << cnt_cycle << " schedule resubmit pkt: " << resubmit_pkt.m_key 
                << " seq id: " << (int)resubmit_pkt.get_seq_id() << " queue size: " << m_multi_queue.at(0).size() << std::endl;
            return resubmit_pkt;
        }
        else {
            auto scheduled_pkt{ schedule() }; //schedule也需要修改
            if (!scheduled_pkt.is_empty()) {
                schedule_fout << "cycle: " << cnt_cycle << " schedule pipeline pkt: " << scheduled_pkt.m_key
                    << " seq id: " << (int)scheduled_pkt.get_seq_id() << " queue size: " << m_multi_queue.at(0).size() << std::endl;
                return m_seq_id_marker.next(std::move(scheduled_pkt));
            }
        }
        schedule_fout << "cycle: " << cnt_cycle << " no packet scheduled! " << " queue size: " << m_multi_queue.at(0).size() << std::endl;
        return Packet{};
    }

    void print_info() override {
        std::cout << "multi_queue rcv " << multi_queue_rcv << " snd: " << multi_queue_snd << std::endl;
    }

    void db_info_fout() {
        db_fout << "cycle: " << cnt_cycle;
        for (int i = 0; i < Q_NUM; i++) {
            db_fout << " queue_" << i << ": " << m_multi_queue.at(i).size();
        }
        db_fout << std::endl;
    }
};

export template <size_t N, size_t K = 2, size_t Q_NUM = 8>
class SongPir_MQ : public BasePir{
    // PacketQueue<N> m_incoming_buffer;
    SeqIdMarker<K> m_seq_id_marker;
    //std::bitset<K> m_dirty_cam;
    //std::bitset<K> m_schedule_cam;
    std::set<std::uint32_t> m_dirty_cam;
    std::set<std::uint32_t> m_schedule_cam;
    size_t m_size_per_queue{ 2 * N / Q_NUM };
    std::array<PacketQueue< 2 * N / Q_NUM>, Q_NUM> m_multi_queue;
    size_t m_schedule_pos{ 0 };
    std::map<std::uint32_t, size_t> m_cnt_buffered_pkt;
    //std::array<size_t, K> m_cnt_buffered_pkt;


    int find_nxt_schedule() override {
        int pos = -1;
        for (int i = 0; i < Q_NUM; i++) {
            if (!m_multi_queue.at(m_schedule_pos).is_empty()) {
                auto& pkt = m_multi_queue.at(m_schedule_pos).front();
                if (!(m_dirty_cam.find(pkt.m_key) != m_dirty_cam.end() && m_schedule_cam.find(pkt.m_key) == m_schedule_cam.end())) {
                    //if ( !(m_dirty_cam.test(pkt.m_key) && !m_schedule_cam.test(pkt.m_key))) { // 队头包可以被调
                    pos = m_schedule_pos;
                }
                break;
            }
            m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
        }        
        m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
        return pos;
    }

    Packet schedule() override {
        //auto key { m_schedule_queue.dequeue() };
        auto key{ find_nxt_schedule() };
        if (key == -1) { //所有队列都不能被调度（或者为空）
            return Packet{};
        }
        multi_queue_snd++;
        auto pkt{ m_multi_queue.at(key).dequeue() };
        m_cnt_buffered_pkt[pkt.m_key]--;

        //schedule_fout << "cycle: " << cnt_cycle << " schedule queue: " << key << " flow id: " << pkt.m_key << std::endl;;
        if (m_cnt_buffered_pkt[pkt.m_key] == 0) {
            m_dirty_cam.erase(pkt.m_key);
            m_schedule_cam.erase(pkt.m_key);
            m_cnt_buffered_pkt.erase(pkt.m_key);
            // schedule_fout << " empty" << std::endl;
            // std::cout << "empty" << std::endl;
        }

        return pkt;
    }

    void enqueue(Packet&& pkt) override {
        if (!m_multi_queue.at(pkt.m_key % Q_NUM).is_full()) {
            multi_queue_rcv++;
            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
            m_multi_queue.at(pkt.m_key % Q_NUM).enqueue(std::move(pkt));
            if (m_cnt_buffered_pkt.find(pkt.m_key) != m_cnt_buffered_pkt.end()) {
                m_cnt_buffered_pkt[pkt.m_key]++;
            }
            else {
                m_cnt_buffered_pkt[pkt.m_key] = 1;
            }
        }
        else {
            ++m_drop_packet_count;
            std::cout << "shouldn't be here" << std::endl;
        }
    }

    std::ofstream db_fout;
    size_t cnt_cycle{ 0 };
    std::ofstream schedule_fout;
    std::ofstream wb_fout;
    std::ofstream bp_fout;

    size_t multi_queue_rcv{ 0 };
    size_t multi_queue_snd{ 0 };

public:
    int m_drop_packet_count { 0 };
    //SongPir() = default;
    SongPir_MQ() {
        db_fout.open("db_size.txt", std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    SongPir_MQ(size_t type, size_t period) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_" + std::to_string(Q_NUM) + "q.txt";
        db_fout.open("db_size" + suffix, std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info" + suffix, std::ios_base::out);
        bp_fout.open("bp_info" + suffix, std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    SongPir_MQ(size_t type, size_t period, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_" + std::to_string(Q_NUM) + "q.txt";
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    SongPir_MQ(size_t type, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_" + std::to_string(Q_NUM) + "q.txt";
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);
        std::cout << "multi queue" << std::endl;
    }

    

    bool is_full() const override {
        std::cout << "should not use it to tell if is full" << std::endl;
        return false;
    }

    bool is_full(Packet pkt) const override {
        return !pkt.is_empty() && m_multi_queue.at(pkt.m_key % Q_NUM).is_full();
    }

    void write_cam(Packet&& pkt) override {
        cnt_cycle++;
        int expect_id = (int)m_seq_id_marker.get_seq_id(pkt.m_key);
        //wb_fout << "cycle: " << cnt_cycle << " flow id: " << pkt.m_key  << " is_write_back: " << !pkt.is_empty() << " seq id: " << (int)pkt.get_seq_id() << " expect id: " << expect_id << std::endl;
        if (!pkt.is_empty()) {
            if (pkt.get_seq_id() != m_seq_id_marker.get_seq_id(pkt.m_key)) {
                //std::cout << "cycle: " << cnt_cycle << " error" << std::endl;
                m_dirty_cam.insert(pkt.m_key);
                m_schedule_cam.erase(pkt.m_key);
                //m_schedule_queue.remove(pkt.m_key);
            }
        }
    }

    // resubmit > schedule > incoming
    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {
        db_info_fout();
        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;
        
        if (!incoming_pkt.is_empty()) {
            //进多队列调度（不管是不是dirty流）
            if (!m_multi_queue.at(incoming_pkt.m_key % Q_NUM).is_full()) { //这里应该提前判断
                enqueue(std::move(incoming_pkt));
            }
            else {
                std::cout << "cycle: " << cnt_cycle << " m_multi_queue_" << incoming_pkt.m_key % Q_NUM << " is full" << std::endl;
            }
        }
        if (!resubmit_pkt.is_empty()) {
            if (resubmit_pkt.get_seq_id() == m_seq_id_marker.get_seq_id(resubmit_pkt.m_key)) {
                // std::cout << "seq get" << std::endl;
                if (m_schedule_cam.find(resubmit_pkt.m_key) == m_schedule_cam.end()) {
                    if (m_cnt_buffered_pkt.find(resubmit_pkt.m_key) != m_cnt_buffered_pkt.end() && m_cnt_buffered_pkt[resubmit_pkt.m_key] != 0) {
                        m_schedule_cam.insert(resubmit_pkt.m_key);
                    }
                    else {
                        m_dirty_cam.erase(resubmit_pkt.m_key);
                    }
                }
            }
            return resubmit_pkt;
        }
        else {
            auto scheduled_pkt{ schedule() }; //schedule也需要修改
            if (!scheduled_pkt.is_empty()) {
                return m_seq_id_marker.next(std::move(scheduled_pkt));
            }
        }
        return Packet{};
    }

    void print_info() override {
        std::cout << "multi_queue rcv " << multi_queue_rcv << " snd: " << multi_queue_snd << std::endl;
    }

    void db_info_fout() {
        db_fout << "cycle: " << cnt_cycle;
        for (int i = 0; i < Q_NUM; i++) {
            db_fout << " queue_" << i << ": " << m_multi_queue.at(i).size();
        }
        db_fout << std::endl;
    }
};

//export template <size_t N, size_t K = 2, size_t Q_NUM = 4>
//class SongPir_MQ_4 : public BasePir {
//    // PacketQueue<N> m_incoming_buffer;
//    SeqIdMarker<K> m_seq_id_marker;
//    //std::bitset<K> m_dirty_cam;
//    //std::bitset<K> m_schedule_cam;
//    std::set<std::uint32_t> m_dirty_cam;
//    std::set<std::uint32_t> m_schedule_cam;
//    size_t m_size_per_queue{ 2 * N / Q_NUM };
//    std::array<PacketQueue< 2 * N / Q_NUM>, Q_NUM> m_multi_queue;
//    size_t m_schedule_pos{ 0 };
//    std::map<std::uint32_t, size_t> m_cnt_buffered_pkt;
//    //std::array<size_t, K> m_cnt_buffered_pkt;
//
//
//    int find_nxt_schedule() override {
//        int pos = -1;
//        for (int i = 0; i < Q_NUM; i++) {
//            if (!m_multi_queue.at(m_schedule_pos).is_empty()) {
//                auto& pkt = m_multi_queue.at(m_schedule_pos).front();
//                if (!(m_dirty_cam.find(pkt.m_key) != m_dirty_cam.end() && m_schedule_cam.find(pkt.m_key) == m_schedule_cam.end())) {
//                    //if ( !(m_dirty_cam.test(pkt.m_key) && !m_schedule_cam.test(pkt.m_key))) { // 队头包可以被调
//                    pos = m_schedule_pos;
//                }
//                break;
//            }
//            m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
//        }
//        m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
//        //auto size = m_schedule_queue.size();
//        //for (int i = 0; i < size; i++) {
//        //    auto x = m_schedule_queue.dequeue();
//        //    //if (x != 0) {
//        //    //    return x;
//        //    //} 
//        //    if (m_dirty_buffer.at(x).is_empty()) {
//        //        m_dirty_cam.reset(x);
//        //        m_schedule_cam.reset(x);
//        //    }
//        //    else {
//        //        return x;
//        //    }
//        //}
//        return pos;
//    }
//
//    Packet schedule() override {
//        //auto key { m_schedule_queue.dequeue() };
//        auto key{ find_nxt_schedule() };
//        if (key == -1) { //所有队列都不能被调度（或者为空）
//            return Packet{};
//        }
//        multi_queue_snd++;
//        auto pkt{ m_multi_queue.at(key).dequeue() };
//        m_cnt_buffered_pkt[pkt.m_key]--;
//
//        //schedule_fout << "cycle: " << cnt_cycle << " schedule queue: " << key << " flow id: " << pkt.m_key << std::endl;;
//        if (m_cnt_buffered_pkt[pkt.m_key] == 0) {
//            m_dirty_cam.erase(pkt.m_key);
//            m_schedule_cam.erase(pkt.m_key);
//            m_cnt_buffered_pkt.erase(pkt.m_key);
//            // schedule_fout << " empty" << std::endl;
//            // std::cout << "empty" << std::endl;
//        }
//
//        return pkt;
//    }
//
//    void enqueue(Packet&& pkt) override {
//        if (!m_multi_queue.at(pkt.m_key % Q_NUM).is_full()) {
//            multi_queue_rcv++;
//            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
//            m_multi_queue.at(pkt.m_key % Q_NUM).enqueue(std::move(pkt));
//            if (m_cnt_buffered_pkt.find(pkt.m_key) != m_cnt_buffered_pkt.end()) {
//                m_cnt_buffered_pkt[pkt.m_key]++;
//            }
//            else {
//                m_cnt_buffered_pkt[pkt.m_key] = 1;
//            }
//        }
//        else {
//            ++m_drop_packet_count;
//            std::cout << "shouldn't be here" << std::endl;
//        }
//    }
//
//    std::ofstream db_fout;
//    size_t cnt_cycle{ 0 };
//    std::ofstream schedule_fout;
//    std::ofstream wb_fout;
//    std::ofstream bp_fout;
//
//    size_t multi_queue_rcv{ 0 };
//    size_t multi_queue_snd{ 0 };
//
//public:
//    int m_drop_packet_count { 0 };
//    //SongPir() = default;
//    SongPir_MQ_4() {
//        db_fout.open("db_size.txt", std::ios_base::out);
//        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
//        wb_fout.open("wb_info.txt", std::ios_base::out);
//        bp_fout.open("bp_info.txt", std::ios_base::out);
//        std::cout << "multi queue" << std::endl;
//    }
//
//    SongPir_MQ_4(size_t type, size_t period) {
//        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_4q.txt";
//        db_fout.open("db_size" + suffix, std::ios_base::out);
//        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
//        wb_fout.open("wb_info" + suffix, std::ios_base::out);
//        bp_fout.open("bp_info" + suffix, std::ios_base::out);
//        std::cout << "multi queue" << std::endl;
//    }
//
//    SongPir_MQ_4(size_t type, size_t period, std::string sub_dir) {
//        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_4q.txt";
//        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
//        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
//        wb_fout.open(sub_dir + "wb_info" + suffix, std::ios_base::out);
//        bp_fout.open(sub_dir + "bp_info" + suffix, std::ios_base::out);
//        std::cout << "multi queue" << std::endl;
//    }
//
//
//
//    bool is_full() const override {
//        std::cout << "should not use it to tell if is full" << std::endl;
//        return false;
//    }
//
//    bool is_full(Packet pkt) const override {
//        return !pkt.is_empty() && m_multi_queue.at(pkt.m_key % Q_NUM).is_full();
//    }
//
//    void write_cam(Packet&& pkt) override {
//        cnt_cycle++;
//        int expect_id = (int)m_seq_id_marker.get_seq_id(pkt.m_key);
//        //wb_fout << "cycle: " << cnt_cycle << " flow id: " << pkt.m_key  << " is_write_back: " << !pkt.is_empty() << " seq id: " << (int)pkt.get_seq_id() << " expect id: " << expect_id << std::endl;
//        if (!pkt.is_empty()) {
//            if (pkt.get_seq_id() != m_seq_id_marker.get_seq_id(pkt.m_key)) {
//                //std::cout << "cycle: " << cnt_cycle << " error" << std::endl;
//                m_dirty_cam.insert(pkt.m_key);
//                m_schedule_cam.erase(pkt.m_key);
//                //m_schedule_queue.remove(pkt.m_key);
//            }
//        }
//    }
//
//    // resubmit > schedule > incoming
//    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {
//        db_info_fout();
//        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;
//
//        if (!incoming_pkt.is_empty()) {
//            //进多队列调度（不管是不是dirty流）
//            if (!m_multi_queue.at(incoming_pkt.m_key % Q_NUM).is_full()) { //这里应该提前判断
//                enqueue(std::move(incoming_pkt));
//            }
//            else {
//                std::cout << "cycle: " << cnt_cycle << " m_multi_queue_" << incoming_pkt.m_key % Q_NUM << " is full" << std::endl;
//            }
//        }
//        if (!resubmit_pkt.is_empty()) {
//            if (resubmit_pkt.get_seq_id() == m_seq_id_marker.get_seq_id(resubmit_pkt.m_key)) {
//                // std::cout << "seq get" << std::endl;
//                if (m_schedule_cam.find(resubmit_pkt.m_key) == m_schedule_cam.end()) {
//                    if (m_cnt_buffered_pkt.find(resubmit_pkt.m_key) != m_cnt_buffered_pkt.end() && m_cnt_buffered_pkt[resubmit_pkt.m_key] != 0) {
//                        m_schedule_cam.insert(resubmit_pkt.m_key);
//                    }
//                    else {
//                        m_dirty_cam.erase(resubmit_pkt.m_key);
//                    }
//                }
//            }
//            return resubmit_pkt;
//        }
//        else {
//            auto scheduled_pkt{ schedule() }; //schedule也需要修改
//            if (!scheduled_pkt.is_empty()) {
//                return m_seq_id_marker.next(std::move(scheduled_pkt));
//            }
//        }
//        return Packet{};
//    }
//
//    void print_info() override {
//        std::cout << "multi_queue rcv " << multi_queue_rcv << " snd: " << multi_queue_snd << std::endl;
//    }
//
//    void db_info_fout() {
//        db_fout << "cycle: " << cnt_cycle;
//        for (int i = 0; i < Q_NUM; i++) {
//            db_fout << " queue_" << i << ": " << m_multi_queue.at(i).size();
//        }
//        db_fout << std::endl;
//    }
//};
//
//export template <size_t N, size_t K = 2, size_t Q_NUM = 2>
//class SongPir_MQ_2 : public BasePir {
//    // PacketQueue<N> m_incoming_buffer;
//    SeqIdMarker<K> m_seq_id_marker;
//    //std::bitset<K> m_dirty_cam;
//    //std::bitset<K> m_schedule_cam;
//    std::set<std::uint32_t> m_dirty_cam;
//    std::set<std::uint32_t> m_schedule_cam;
//    size_t m_size_per_queue{ 2 * N / Q_NUM };
//    std::array<PacketQueue< 2 * N / Q_NUM>, Q_NUM> m_multi_queue;
//    size_t m_schedule_pos{ 0 };
//    std::map<std::uint32_t, size_t> m_cnt_buffered_pkt;
//    //std::array<size_t, K> m_cnt_buffered_pkt;
//
//
//    int find_nxt_schedule() override {
//        int pos = -1;
//        for (int i = 0; i < Q_NUM; i++) {
//            if (!m_multi_queue.at(m_schedule_pos).is_empty()) {
//                auto& pkt = m_multi_queue.at(m_schedule_pos).front();
//                if (!(m_dirty_cam.find(pkt.m_key) != m_dirty_cam.end() && m_schedule_cam.find(pkt.m_key) == m_schedule_cam.end())) {
//                    //if ( !(m_dirty_cam.test(pkt.m_key) && !m_schedule_cam.test(pkt.m_key))) { // 队头包可以被调
//                    pos = m_schedule_pos;
//                }
//                break;
//            }
//            m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
//        }
//        m_schedule_pos = (m_schedule_pos + 1) % Q_NUM;
//        //auto size = m_schedule_queue.size();
//        //for (int i = 0; i < size; i++) {
//        //    auto x = m_schedule_queue.dequeue();
//        //    //if (x != 0) {
//        //    //    return x;
//        //    //} 
//        //    if (m_dirty_buffer.at(x).is_empty()) {
//        //        m_dirty_cam.reset(x);
//        //        m_schedule_cam.reset(x);
//        //    }
//        //    else {
//        //        return x;
//        //    }
//        //}
//        return pos;
//    }
//
//    Packet schedule() override {
//        //auto key { m_schedule_queue.dequeue() };
//        auto key{ find_nxt_schedule() };
//        if (key == -1) { //所有队列都不能被调度（或者为空）
//            return Packet{};
//        }
//        multi_queue_snd++;
//        auto pkt{ m_multi_queue.at(key).dequeue() };
//        m_cnt_buffered_pkt[pkt.m_key]--;
//
//        //schedule_fout << "cycle: " << cnt_cycle << " schedule queue: " << key << " flow id: " << pkt.m_key << std::endl;;
//        if (m_cnt_buffered_pkt[pkt.m_key] == 0) {
//            m_dirty_cam.erase(pkt.m_key);
//            m_schedule_cam.erase(pkt.m_key);
//            m_cnt_buffered_pkt.erase(pkt.m_key);
//            // schedule_fout << " empty" << std::endl;
//            // std::cout << "empty" << std::endl;
//        }
//
//        return pkt;
//    }
//
//    void enqueue(Packet&& pkt) override {
//        if (!m_multi_queue.at(pkt.m_key % Q_NUM).is_full()) {
//            multi_queue_rcv++;
//            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
//            m_multi_queue.at(pkt.m_key % Q_NUM).enqueue(std::move(pkt));
//            if (m_cnt_buffered_pkt.find(pkt.m_key) != m_cnt_buffered_pkt.end()) {
//                m_cnt_buffered_pkt[pkt.m_key]++;
//            }
//            else {
//                m_cnt_buffered_pkt[pkt.m_key] = 1;
//            }
//        }
//        else {
//            ++m_drop_packet_count;
//            std::cout << "shouldn't be here" << std::endl;
//        }
//    }
//
//    std::ofstream db_fout;
//    size_t cnt_cycle{ 0 };
//    std::ofstream schedule_fout;
//    std::ofstream wb_fout;
//    std::ofstream bp_fout;
//
//    size_t multi_queue_rcv{ 0 };
//    size_t multi_queue_snd{ 0 };
//
//public:
//    int m_drop_packet_count { 0 };
//    //SongPir() = default;
//    SongPir_MQ_2() {
//        db_fout.open("db_size.txt", std::ios_base::out);
//        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
//        wb_fout.open("wb_info.txt", std::ios_base::out);
//        bp_fout.open("bp_info.txt", std::ios_base::out);
//        std::cout << "multi queue" << std::endl;
//    }
//
//    SongPir_MQ_2(size_t type, size_t period) {
//        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + "_2q.txt";
//        db_fout.open("db_size" + suffix, std::ios_base::out);
//        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
//        wb_fout.open("wb_info" + suffix, std::ios_base::out);
//        bp_fout.open("bp_info" + suffix, std::ios_base::out);
//        std::cout << "multi queue" << std::endl;
//    }
//
//
//
//    bool is_full() const override {
//        std::cout << "should not use it to tell if is full" << std::endl;
//        return false;
//    }
//
//    bool is_full(Packet pkt) const override {
//        return !pkt.is_empty() && m_multi_queue.at(pkt.m_key % Q_NUM).is_full();
//    }
//
//    void write_cam(Packet&& pkt) override {
//        cnt_cycle++;
//        int expect_id = (int)m_seq_id_marker.get_seq_id(pkt.m_key);
//        //wb_fout << "cycle: " << cnt_cycle << " flow id: " << pkt.m_key  << " is_write_back: " << !pkt.is_empty() << " seq id: " << (int)pkt.get_seq_id() << " expect id: " << expect_id << std::endl;
//        if (!pkt.is_empty()) {
//            if (pkt.get_seq_id() != m_seq_id_marker.get_seq_id(pkt.m_key)) {
//                //std::cout << "cycle: " << cnt_cycle << " error" << std::endl;
//                m_dirty_cam.insert(pkt.m_key);
//                m_schedule_cam.erase(pkt.m_key);
//                //m_schedule_queue.remove(pkt.m_key);
//            }
//        }
//    }
//
//    // resubmit > schedule > incoming
//    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {
//        db_info_fout();
//        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;
//
//        if (!incoming_pkt.is_empty()) {
//            //进多队列调度（不管是不是dirty流）
//            if (!m_multi_queue.at(incoming_pkt.m_key % Q_NUM).is_full()) { //这里应该提前判断
//                enqueue(std::move(incoming_pkt));
//            }
//            else {
//                std::cout << "cycle: " << cnt_cycle << " m_multi_queue_" << incoming_pkt.m_key % Q_NUM << " is full" << std::endl;
//            }
//        }
//        if (!resubmit_pkt.is_empty()) {
//            if (resubmit_pkt.get_seq_id() == m_seq_id_marker.get_seq_id(resubmit_pkt.m_key)) {
//                // std::cout << "seq get" << std::endl;
//                if (m_schedule_cam.find(resubmit_pkt.m_key) == m_schedule_cam.end()) {
//                    if (m_cnt_buffered_pkt.find(resubmit_pkt.m_key) != m_cnt_buffered_pkt.end() && m_cnt_buffered_pkt[resubmit_pkt.m_key] != 0) {
//                        m_schedule_cam.insert(resubmit_pkt.m_key);
//                    }
//                    else {
//                        m_dirty_cam.erase(resubmit_pkt.m_key);
//                    }
//                }
//            }
//            return resubmit_pkt;
//        }
//        else {
//            auto scheduled_pkt{ schedule() }; //schedule也需要修改
//            if (!scheduled_pkt.is_empty()) {
//                return m_seq_id_marker.next(std::move(scheduled_pkt));
//            }
//        }
//        return Packet{};
//    }
//
//    void print_info() override {
//        std::cout << "multi_queue rcv " << multi_queue_rcv << " snd: " << multi_queue_snd << std::endl;
//    }
//
//    void db_info_fout() {
//        db_fout << "cycle: " << cnt_cycle;
//        for (int i = 0; i < Q_NUM; i++) {
//            db_fout << " queue_" << i << ": " << m_multi_queue.at(i).size();
//        }
//        db_fout << std::endl;
//    }
//};

export template <size_t N, size_t K = 2>
class SongPir_RR : public BasePir {
    PacketQueue<N> m_incoming_buffer;
    SeqIdMarker<K> m_seq_id_marker;
    //std::bitset<K> m_dirty_cam;
    std::set<std::uint32_t> m_dirty_cam;
    //std::bitset<K> m_schedule_cam;
    std::set<std::uint32_t> m_schedule_cam;
    //std::array<PacketQueue<N>, K> m_dirty_buffer;
    std::map<std::uint32_t, PacketQueue<N>> m_dirty_buffer;
    int m_buffer_size{ 0 };
    RoundRobinQueue<K> m_schedule_queue;
    std::bitset<1> m_incoming_or_dirty; // 0: schedule incoming, 1: schedule dirty

    int find_nxt_schedule() override {
        auto size = m_schedule_queue.size();
        for (int i = 0; i < size; i++) {
            auto x = m_schedule_queue.dequeue();
            //if (x != 0) {
            //    return x;
            //} 
            if (m_dirty_buffer.find(x) == m_dirty_buffer.end()) {
                m_schedule_cam.erase(x);
                m_dirty_cam.erase(x);
                std::cout << "should not happen" << std::endl;
            }
            else if (m_dirty_buffer[x].is_empty()) {
                m_dirty_buffer.erase(x);
                m_dirty_cam.erase(x);
                m_schedule_cam.erase(x);
            }
            else {
                return x;
            }
        }
        return 0;
    }

    Packet schedule() override {
        //auto key { m_schedule_queue.dequeue() };
        auto key{ find_nxt_schedule() };
        if (key == 0) {
            return Packet{};
        }
        dirty_buffer_snd++;
        auto pkt{ m_dirty_buffer[key].dequeue() };
        --m_buffer_size;

        //schedule_fout << "cycle: " << cnt_cycle << " schedule flow: " << key << " pkt key: " << pkt.m_key;
        if (m_dirty_buffer[key].is_empty()) {
            m_dirty_cam.erase(key);
            m_schedule_cam.erase(key);
            //schedule_fout << " empty" << std::endl;
            // std::cout << "empty" << std::endl;
        }
        else {
            m_schedule_queue.enqueue(key);
            //schedule_fout << " not empty" << std::endl;
            //std::cout << "not empty" << std::endl;
        }

        return pkt;
        //return Packet{};
    }

    void enqueue(Packet&& pkt) override {
        if (m_buffer_size < N) {
            ++m_buffer_size;
            dirty_buffer_rcv++;
            // std::cout << "cycle: " << cnt_cycle << " enqueue dirty buffer " << pkt.m_key << std::endl;
            if (m_dirty_buffer.find(pkt.m_key) == m_dirty_buffer.end()) {
                m_dirty_buffer.insert(std::make_pair(pkt.m_key, PacketQueue<N> {}));
            }
            m_dirty_buffer[pkt.m_key].enqueue(std::move(pkt));
        }
        else {
            ++m_drop_packet_count;
            std::cout << "shouldn't be here" << std::endl;
        }
    }

    std::ofstream ib_fout;
    std::ofstream db_fout;
    size_t cnt_cycle{ 0 };
    std::ofstream schedule_fout;
    std::ofstream wb_fout;
    std::ofstream bp_fout;
    std::ofstream wbb_fout;

    size_t incoming_buffer_rcv{ 0 };
    size_t incoming_buffer_snd{ 0 };
    size_t incoming_buffer_to_dirty_buffer{ 0 };
    size_t incoming_buffer_to_pipeline{ 0 };
    size_t dirty_buffer_rcv{ 0 };
    size_t dirty_buffer_snd{ 0 };

public:
    int m_drop_packet_count { 0 };
    //SongPir() = default;
    SongPir_RR() {
        ib_fout.open("ib_size.txt", std::ios_base::out);
        db_fout.open("db_size.txt", std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        wbb_fout.open("./packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list round robin" << std::endl;
    }

    SongPir_RR(size_t type, size_t period) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + ".txt";
        ib_fout.open("ib_size" + suffix, std::ios_base::out);
        db_fout.open("db_size" + suffix, std::ios_base::out);
        schedule_fout.open("schedule_pkt.txt", std::ios_base::out);
        wb_fout.open("wb_info.txt", std::ios_base::out);
        bp_fout.open("bp_info.txt", std::ios_base::out);
        wbb_fout.open("./packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list round robin" << std::endl;
    }

    SongPir_RR(size_t type, size_t period, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + "_j_" + std::to_string(period) + ".txt";
        ib_fout.open(sub_dir + "ib_size" + suffix, std::ios_base::out);
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info.txt", std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info.txt", std::ios_base::out);
        wbb_fout.open(sub_dir + "packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list round robin" << std::endl;
    }

    SongPir_RR(size_t type, std::string sub_dir) {
        std::string suffix = "_i_" + std::to_string(type) + ".txt";
        ib_fout.open(sub_dir + "ib_size" + suffix, std::ios_base::out);
        db_fout.open(sub_dir + "db_size" + suffix, std::ios_base::out);
        schedule_fout.open(sub_dir + "schedule_pkt.txt", std::ios_base::out);
        wb_fout.open(sub_dir + "wb_info.txt", std::ios_base::out);
        bp_fout.open(sub_dir + "bp_info.txt", std::ios_base::out);
        wbb_fout.open(sub_dir + "packet_seq_2.txt", std::ios_base::out);
        std::cout << "per flow list round robin" << std::endl;
    }


    bool is_full() const override {
        return m_incoming_buffer.is_full();
    }

    bool is_full(Packet pkt) const override {
        return m_incoming_buffer.is_full();
    }

    void write_cam(Packet&& pkt) override {
        cnt_cycle++;
        int expect_id = (int)m_seq_id_marker.get_seq_id(pkt.m_key);
        //wb_fout << "cycle: " << cnt_cycle << " flow id: " << pkt.m_key  << " is_write_back: " << !pkt.is_empty() << " seq id: " << (int)pkt.get_seq_id() << " expect id: " << expect_id << std::endl;
        if (!pkt.is_empty()) {
            if (pkt.get_seq_id() != m_seq_id_marker.get_seq_id(pkt.m_key)) {
                //std::cout << "cycle: " << cnt_cycle << " error" << std::endl;
                m_dirty_cam.insert(pkt.m_key);
                m_schedule_cam.erase(pkt.m_key);
                m_schedule_queue.remove(pkt.m_key);
            }
        }
    }

    // resubmit > schedule > incoming
    Packet next(Packet&& incoming_pkt, Packet&& resubmit_pkt) override {

        ib_fout << "cycle: " << cnt_cycle << " incoming_buffer_size: " << m_incoming_buffer.size() << std::endl;
        db_fout << "cycle: " << cnt_cycle << " dirty_buffer_size: " << m_buffer_size << " schedule_queue_size: " << m_schedule_queue.size() << std::endl;
        //bp_fout << "cycle: " << cnt_cycle << " bp_pkt_flow: " << resubmit_pkt.m_key << " seq id: " << (int)resubmit_pkt.get_seq_id() << std::endl;
        if (!incoming_pkt.is_empty()) {
            incoming_buffer_rcv++;
            m_incoming_buffer.enqueue(std::move(incoming_pkt));
        }
        bool incoming_packet_valid{ false };
        if (const auto& pkt{ m_incoming_buffer.front() }; !pkt.is_empty()) {
            if (m_dirty_cam.find(pkt.m_key) != m_dirty_cam.end()) {
                if (m_buffer_size < N) {
                    incoming_buffer_to_dirty_buffer++;
                    enqueue(m_incoming_buffer.dequeue());
                }
            }
            else {
                incoming_packet_valid = true;
            }
        }
        if (!resubmit_pkt.is_empty()) {
            if (resubmit_pkt.get_seq_id() == m_seq_id_marker.get_seq_id(resubmit_pkt.m_key)) {
                // std::cout << "seq get" << std::endl;
                if (m_schedule_cam.find(resubmit_pkt.m_key) == m_schedule_cam.end()) {
                    if (!(m_dirty_buffer.find(resubmit_pkt.m_key) == m_dirty_buffer.end() || m_dirty_buffer[resubmit_pkt.m_key].is_empty())) {
                        m_schedule_queue.enqueue(resubmit_pkt.m_key);
                        m_schedule_cam.insert(resubmit_pkt.m_key);
                    }
                    else {
                        m_dirty_cam.erase(resubmit_pkt.m_key);
                        m_dirty_buffer.erase(resubmit_pkt.m_key);
                    }
                }
            }
            return resubmit_pkt;
        }
        else {
            if (!m_incoming_or_dirty.test(0)) {
                if (incoming_packet_valid) {
                    incoming_buffer_to_pipeline++;
                    m_incoming_or_dirty.flip(0);
                    return m_seq_id_marker.next(m_incoming_buffer.dequeue());
                }
                else {
                    auto scheduled_pkt{ schedule() };
                    if (!scheduled_pkt.is_empty()) {
                        return m_seq_id_marker.next(std::move(scheduled_pkt));
                    }
                }
            }
            else {
                auto scheduled_pkt{ schedule() };
                if (!scheduled_pkt.is_empty()) {
                    m_incoming_or_dirty.flip(0);
                    return m_seq_id_marker.next(std::move(scheduled_pkt));
                }
                else if (incoming_packet_valid) {
                    incoming_buffer_to_pipeline++;
                    return m_seq_id_marker.next(m_incoming_buffer.dequeue());
                }
            }
        }

        return Packet{};
    }

    void print_info() override {
        //std::cout << "incoming_buffer rcv: " << incoming_buffer_rcv << " to dirty buffer: " << incoming_buffer_to_dirty_buffer << " to pipeline: " << incoming_buffer_to_pipeline  << std::endl;
        //std::cout << "dirty_buffer rcv " << dirty_buffer_rcv << " snd: " << dirty_buffer_snd << std::endl;
    }
};


