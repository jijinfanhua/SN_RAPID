export module rapid.SongReadWritePeer;

import rapid.Packet;
import rapid.Device;
import rapid.SongPir;
import rapid.SongPiw;
import rapid.VirtualPipeline;
import rapid.SongPipeline;
import rapid.PacketQueue;
import rapid.StateTable;
import std;
#include "assert.h"
const size_t FRONT_CYCLES { 5 };
//export const size_t PROC_CYCLES { 15 };
//export const size_t BACKBUS_CYCLES { 2 };
//export consteval size_t PIPELINE_LENGTH(size_t IID, size_t OID)
//{
//    return (OID - IID + 1) * PROC_CYCLES;
//}
//export consteval size_t BACKBUS_LENGTH(size_t IID, size_t OID)
//{
//    return 2 + (OID - IID) * BACKBUS_CYCLES;
//}
//export consteval size_t CLOCK_MAX(size_t IID, size_t OID)
//{
//    return PIPELINE_LENGTH(IID, OID) + BACKBUS_LENGTH(IID, OID);
//}

export template <size_t N, size_t RID, size_t WID, size_t K = 2>
class SongReadWritePeer : public Device {
    constexpr const static size_t m_pipeline_length { PIPELINE_LENGTH(RID, WID) - FRONT_CYCLES };
    constexpr const static size_t m_backbus_length { BACKBUS_LENGTH(RID, WID) };
    constexpr const static size_t m_clock_max { CLOCK_MAX_2(RID, WID) - FRONT_CYCLES + 1 };

    VirtualPipeline<FRONT_CYCLES> m_front;
    VirtualPipeline<m_pipeline_length> m_pipeline;
    VirtualPipeline<m_backbus_length> m_backbus;
    VirtualPipeline<m_backbus_length> m_wb_bus;
    //SongPir<N, K> m_pir;
    //SongPir_MQ<N, K> m_pir;
    //SongPir_RR<N, K> m_pir;
    BasePir* m_pir;
    //NatStateTable m_state_table;
    //DDosStateTable m_state_table;
    //BFDStateTable m_state_table;
    //TopKStateTable<30> m_state_table;
    StateTable* m_state_table;
    //BasePir m_pir;

    //SongPiw<std::byte(1 << RID), m_clock_max, K> *m_piw;
    SongPiw<std::byte(1 << RID), m_clock_max, K>* m_piw;
    Packet m_temp_pkt;
    Packet m_wb_pkt;
    std::ofstream w_r_peer_out;
    std::ofstream wbb_out;
    
    size_t cnt_cycle{ 0 };

    Packet m_front_out_pkt;


public:
    SongReadWritePeer() {
        w_r_peer_out.open("./wr_info.txt", std::ios_base::out);
        wbb_out.open("./wbb_info.txt", std::ios_base::out);
        //if (PIR_TYPE == 0) {
        //    m_pir = SongPir<N, K>();
        //}
        //else if (PIR_TYPE == 1) {
        //    m_pir = SongPir_MQ<N, K>();
        //}
        //else if (PIR_TYPE == 2) {
        //    m_pir = SongPir_RR<N, K>();
        //}
        //std::cout << "pipeline length " << PIPELINE_LENGTH(RID, WID) << " back bus length " << BACKBUS_LENGTH(RID, WID) << "clock max " << CLOCK_MAX_2(RID, WID) << std::endl;
    };

    SongReadWritePeer(size_t type, std::string sub_dir) {
        std::cout << "m_clock_max: " << m_clock_max << std::endl;
        w_r_peer_out.open(sub_dir + "wr_info.txt", std::ios_base::out);
        wbb_out.open(sub_dir + "wbb_info.txt", std::ios_base::out);
        if (type >= 18) {
            std::cout << " wrong type " << std::endl;
        }

        m_piw = new SongPiw<std::byte(1 << RID), m_clock_max, K>(sub_dir, type);

        uint32_t state_table_type = type / 7;
        uint32_t pir_type = type % 7;
        switch (pir_type) {
        case 0:
            m_pir = new SongPir<N, K>(type, sub_dir);
            break;
        case 1:
            m_pir = new SongPir_MQ<N, K, 2>(type, sub_dir);
            break;
        case 2:
            m_pir = new SongPir_RR<N, K>(type, sub_dir);
            break;
        case 3:
            m_pir = new SongPir_SQ<N, K>(type, sub_dir);
            break;
        case 4:
            m_pir = new SongPir_MQ<N, K, 4>(type, sub_dir);
            break;
        case 5:
            m_pir = new SongPir_MQ<N, K, 8>(type, sub_dir);
            break;
        case 6:
            m_pir = new SongPir_Direct<N, K, 1>(type, sub_dir);
            break;
        default:
            m_pir = new SongPir<N, K>();
            break;
        }
        /*
        *
        *   i / 4 = 0: i = 0, 1, 2, 3: NAT
        *   i / 4 = 1: i = 4, 5, 6, 7: DDoS
            i / 4 = 2: i = 8, 9, 10, 11: big flow detector
            i / 4 = 3: i = 12, 13, 14, 15: TopK
        */
        switch (state_table_type) {
        case 0:
            m_state_table = new NatStateTable();
            break;
        case 1:
            m_state_table = new DDosStateTable();
            break;
        case 2:
            m_state_table = new BFDStateTable();
            break;
        case 3:
            std::cout << "TopK used!" << std::endl;
            m_state_table = new TopKStateTable<300>();
            break;
        default:
            m_state_table = new SimpleTable();
            break;
        }
    }

    SongReadWritePeer(size_t type, size_t period, std::string sub_dir) {
        std::cout << "m_clock_max: " << m_clock_max << std::endl;
        w_r_peer_out.open(sub_dir + "wr_info.txt", std::ios_base::out);
        wbb_out.open(sub_dir + "wbb_info.txt", std::ios_base::out);
        if (type >= 18) {
            std::cout << " wrong type " << std::endl;
        }

        m_piw = new SongPiw<std::byte(1 << RID), m_clock_max, K>(sub_dir, type);

        uint32_t state_table_type = type / 7;
        uint32_t pir_type = type % 7;
        switch (pir_type) {
        case 0:
            m_pir = new SongPir<N, K>(type, period, sub_dir);
            break;
        case 1:
            m_pir = new SongPir_MQ<N, K, 2>(type, period, sub_dir);
            break;
        case 2:
            m_pir = new SongPir_RR<N, K>(type, period, sub_dir);
            break;
        case 3:
            m_pir = new SongPir_SQ<N, K>(type, period, sub_dir);
            break;
        case 4:
            m_pir = new SongPir_MQ<N, K, 4>(type, period, sub_dir);
            break;
        case 5:
            m_pir = new SongPir_MQ<N, K, 8>(type, period, sub_dir);
            break;
        case 6:
            m_pir = new SongPir_Direct<N, K, 1>(type, period, sub_dir);
            break;
        default:
            m_pir = new SongPir<N, K>();
            break;
        }
        /*
        *
        *   i / 4 = 0: i = 0, 1, 2, 3: NAT
        *   i / 4 = 1: i = 4, 5, 6, 7: DDoS
            i / 4 = 2: i = 8, 9, 10, 11: big flow detector
            i / 4 = 3: i = 12, 13, 14, 15: TopK
        */
        switch (state_table_type) {
        case 0:
            m_state_table = new NatStateTable();
            break;
        case 1:
            m_state_table = new DDosStateTable();
            break;
        case 2:
            m_state_table = new BFDStateTable();
            break;
        case 3:
            std::cout << "TopK used!" << std::endl;
            m_state_table = new TopKStateTable<300>();
            break;
        default:
            m_state_table = new SimpleTable();
            break;
        }
    }

    SongReadWritePeer(size_t type) {
        w_r_peer_out.open("./wr_info.txt", std::ios_base::out);
        wbb_out.open("./wbb_info.txt", std::ios_base::out);
        if (type >= 16) {
            std::cout << " wrong type " << std::endl;
        }
        
        uint32_t state_table_type = type / 4;
        uint32_t pir_type = type % 4;
        switch (pir_type) {
        case 0:
            m_pir = new SongPir<N, K>();
            break;
        case 1:
            m_pir = new SongPir_MQ<N, K>();
            break;
        case 2:
            m_pir = new SongPir_RR<N, K>();
            break;
        case 3:
            m_pir = new SongPir_SQ<N, K>();
            break;
        default:
            m_pir = new SongPir<N, K>();
            break;
        }
        /*
        *   
        *   i / 4 = 0: i = 0, 1, 2, 3: NAT
        *   i / 4 = 1: i = 4, 5, 6, 7: DDoS
            i / 4 = 2: i = 8, 9, 10, 11: big flow detector
            i / 4 = 3: i = 12, 13, 14, 15: TopK
        */
        switch (state_table_type) {
        case 0:
            m_state_table = new NatStateTable();
            break;
        case 1:
            m_state_table = new DDosStateTable();
            break;
        case 2:
            m_state_table = new BFDStateTable();
            break;
        case 3:
            m_state_table = new TopKStateTable<30>();
            break;
        default:
            m_state_table = new SimpleTable();
            break;
        }
    }

    SongReadWritePeer(size_t type, size_t period) {
        std::cout << "m_clock_max: "  << m_clock_max << std::endl;
        w_r_peer_out.open("./wr_info.txt", std::ios_base::out);
        wbb_out.open("./wbb_info.txt", std::ios_base::out);
        if (type >= 18) {
            std::cout << " wrong type " << std::endl;
        }

        uint32_t state_table_type = type / 7;
        uint32_t pir_type = type % 7;
        switch (pir_type) {
        case 0:
            m_pir = new SongPir<N, K>(type, period);
            break;
        case 1:
            m_pir = new SongPir_MQ<N, K, 2>(type, period);
            break;
        case 2:
            m_pir = new SongPir_RR<N, K>(type, period);
            break;
        case 3:
            m_pir = new SongPir_SQ<N, K>(type, period);
            break;
        case 4:
            m_pir = new SongPir_MQ<N, K, 4>(type, period);
            break;
        case 5:
            m_pir = new SongPir_MQ<N, K, 8>(type, period);
            break;
        case 6:
            m_pir = new SongPir_Direct<N, K, 1>(type, period);
            break;
        default:
            m_pir = new SongPir<N, K>();
            break;
        }
        /*
        *
        *   i / 4 = 0: i = 0, 1, 2, 3: NAT
        *   i / 4 = 1: i = 4, 5, 6, 7: DDoS
            i / 4 = 2: i = 8, 9, 10, 11: big flow detector
            i / 4 = 3: i = 12, 13, 14, 15: TopK
        */
        switch (state_table_type) {
        case 0:
            m_state_table = new NatStateTable();
            break;
        case 1:
            m_state_table = new DDosStateTable();
            break;
        case 2:
            m_state_table = new BFDStateTable();
            break;
        case 3:
            std::cout << "TopK used!" << std::endl;
            m_state_table = new TopKStateTable<30>();
            break;
        default:
            m_state_table = new SimpleTable();
            break;
        }
    }

    ~SongReadWritePeer() {
        if (m_pir) {
            delete m_pir;
            m_pir = nullptr;
        }
        if (m_state_table) {
            delete m_state_table;
            m_state_table = nullptr;
        }
    }
    //bool is_full() const {
    //    return m_pir.is_full();
    //}

    bool is_full() const {
        return m_pir->is_full(m_front_out_pkt);
    }

    Packet next(Packet&& pkt) {
        cnt_cycle++;
        //w_r_peer_out << "cycle: " << cnt_cycle << " r_in_pkt: " << pkt.m_key;
        //assert(0);
        
        //如果pir的 buffer满了， front_pipeline需要停顿
        bool is_stall = m_pir->call_is_full(m_front_out_pkt);
        auto front_pkt = is_stall ? Packet {} : m_front_out_pkt;
        m_front_out_pkt = is_stall ? m_front_out_pkt : m_front.next(std::move(pkt));
        
        m_state_table->call_update_state(m_wb_pkt);
        m_pir->call_write_cam(std::move(m_wb_pkt));
        

        auto pir_pkt{ m_pir->call_next(std::move(front_pkt), std::move(m_temp_pkt)) };
        auto state_pkt{ m_state_table->set_write_back(std::move(pir_pkt)) };
        //wbb_out << "cycle: " << cnt_cycle << " state flow_id: " << state_pkt.m_key << " is_write_back: " << (int)state_pkt.m_write_back_bitmap << std::endl;

        auto pipe_pkt{ m_pipeline.next(std::move(state_pkt)) };
        auto [bp_wb, pp] { m_piw->next(std::move(pipe_pkt)) };
        auto [bp, wb] { bp_wb };
        //w_r_peer_out << " w_bp_pkt: " << bp.m_key << " w_wb_pkt: " << wb.m_key << " pp_pkt: " << pp.m_key ;
        m_temp_pkt = m_backbus.next(std::move(bp));
        m_wb_pkt = m_wb_bus.next(std::move(wb));
        //w_r_peer_out << " r_bp_pkt: " << m_temp_pkt.m_key << "  r_wb_pkt: " << m_wb_pkt.m_key << std::endl;
        return std::move(pp);
    }

    void print_info() {
        m_pir->call_print_info();
    }
};