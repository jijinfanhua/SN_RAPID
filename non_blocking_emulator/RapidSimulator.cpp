import rapid.experimental;
import rapid.experimental.bench;
import rapid.ReadWritePeer;
import rapid.SongPipeline;
import rapid.ImprovedSongPipeline;
import std;
#include "stdlib.h"
constexpr const bool TEST_MODE { true };

template <size_t RID1, size_t WID1, size_t RID2, size_t WID2, size_t EXPLICIT_CLOCK_MAX>
void run_experiment_bench(int packet_number) {
    std::cout << std::endl;
    std::cout << "===================================================" << std::endl;
    std::cout << std::format(" EXPERIMENT {} {} {} {} {}", RID1, WID1, RID2, WID2, EXPLICIT_CLOCK_MAX) << std::endl;
    std::ofstream ofs(std::format("result-{}-{}-{}-{}-{}.txt", RID1, WID1, RID2, WID2, EXPLICIT_CLOCK_MAX));
    ExperimentBench<RID1, WID1, RID2, WID2, EXPLICIT_CLOCK_MAX> bench;
    bench.initialize();
    bench.run_all(ofs, packet_number);
}

template <size_t RID1, size_t WID1, size_t RID2, size_t WID2>
void run_experiment_bench_T1(int packet_number)
{
    std::cout << "1" << std::endl;
    run_experiment_bench<RID1, WID1, RID2, WID2, CLOCK_MAX(RID1, WID1)>(packet_number);
    // song pipeline does not need to test with different explicit clock max
    //run_experiment_bench<RID1, WID1, RID2, WID2, CLOCK_MAX(RID1, WID1) + CLOCK_MAX(RID2, WID2) / 2>(packet_number);
    //run_experiment_bench<RID1, WID1, RID2, WID2, CLOCK_MAX(RID1, WID1) + CLOCK_MAX(RID2, WID2) / 2 * 2>(packet_number);
    //run_experiment_bench<RID1, WID1, RID2, WID2, CLOCK_MAX(RID1, WID1) + CLOCK_MAX(RID2, WID2) / 2 * 3>(packet_number);
    //run_experiment_bench<RID1, WID1, RID2, WID2, CLOCK_MAX(RID1, WID1) + CLOCK_MAX(RID2, WID2) / 2 * 4>(packet_number);
}

template <size_t RID1, size_t WID1, size_t RID2, size_t WID2, size_t EXPLICIT_CLOCK_MAX>
void run_song_experiment_bench(int packet_num, size_t fc_num, size_t fixed_flow_num, size_t pkt_gap, size_t burst_gap, size_t burst_ratio, size_t front_buffer_size, size_t pir_type) {
    std::cout << std::endl;
    std::cout << "===================================================" << std::endl;
    std::cout << std::format(" EXPERIMENT {} {} {} {} {} {} {}", fc_num, fixed_flow_num, pkt_gap, burst_gap, burst_ratio, front_buffer_size, pir_type) << std::endl;
    std::ofstream ofs(std::format("result_{}_{}_{}_{}_{}_{}_{}.txt", fc_num, fixed_flow_num, pkt_gap, burst_gap, burst_ratio, front_buffer_size, pir_type));
    ExperimentBench<RID1, WID1, RID2, WID2, EXPLICIT_CLOCK_MAX> bench;
    bench.initialize(fc_num, fixed_flow_num, pkt_gap, burst_gap, burst_ratio, front_buffer_size, pir_type);
    bench.run_all(ofs, packet_num);
}

int main(int argc, char const* argv[])
{
    if constexpr (TEST_MODE) {
        std::ofstream os;

        std::string pir_name[7] = { "pl", "mq2", "rr", "sq", "mq4" , "mq8", "direct_single" };
        std::string use_case_name[5] = { "nat", "ddos", "bfd", "topk", "wwww" };

        if (argc != 3) {
            std::cout << "Number of args is not 3!\n";
        }

        int syn = std::stoi(argv[1]);

        bool SYN = (syn == 1 ? true : false);

        if (SYN) {
            std::string parent_dir = "./trace_syn/";
            std::string trace_name = argv[2];
            std::string trace_path = parent_dir + trace_name;
            std::string sub_dir = parent_dir;

            // 检查目录是否存在
            if (!std::filesystem::exists(parent_dir)) {
                std::cout << "Directory " << parent_dir << " does not exist. Creating directory.\n";
                std::filesystem::create_directories(parent_dir);
            }

            // 检查文件是否存在
            if (!std::filesystem::exists(trace_path)) {
                std::cout << trace_path << " file does not exist, please add it!\n";
                return 0;
            }

            std::filesystem::path p(trace_path);

            for (int i = 27; i <= 27; i++) {
                std::string file_name = parent_dir + pir_name[i % 7] + "_" + use_case_name[i / 7] + "_" + p.stem().string() + ".txt";
                std::cout << file_name << " is used!" << std::endl;
                os.open(file_name, std::ios_base::out);
                std::stringstream m_result;
                if (i / 7 == 1) {
                    auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 1>, TupleSrcDst, 32769 >(i, trace_path, sub_dir);
                    experiment.reset();
                    experiment.run_until(100000);
                    experiment.report(std::cout);
                    experiment.report(m_result);
                    experiment.print_info();
                }
                else {
                    auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 1>, Tuple_5, 32769 >(i, trace_path, sub_dir);
                    experiment.reset();
                    experiment.run_until(100000);
                    experiment.report(std::cout);
                    experiment.report(m_result);
                    experiment.print_info();
                }

                os << m_result.str();
                os.close();
            }
        }
        else {
            std::vector<int> time_nums = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22 };
            int times = 9;
            std::string parent_dir = "./trace_real_caida/";

            for (int k = 0; k < time_nums.size(); k++) {
                int j = (k + 1) * times;
                std::string sub_dir = parent_dir + "res_topk_nic_" + std::to_string(j) + "/";

                // 检查目录是否存在
                if (!std::filesystem::exists(sub_dir)) {
                    std::cout << "Directory " << sub_dir << " does not exist. Creating directory.\n";
                    std::filesystem::create_directories(sub_dir);
                }

                std::string trace_path = sub_dir + "caida_real_build_trace_" + std::to_string(time_nums[k]) + "_NIC_400W_from_2000W.txt";

                // 检查文件是否存在
                if (!std::filesystem::exists(trace_path)) {
                    std::cout << trace_path <<  " file does not exist, please add it!\n";
                    continue;
                }

                for (int i = 27; i <= 27; i++) {
                    std::string file_name = parent_dir + pir_name[i % 7] + "_" + use_case_name[i / 7] + "_" + std::to_string(j) + ".txt";
                    std::cout << file_name << " is used!" << std::endl;
                    os.open(file_name, std::ios_base::out);
                    std::stringstream m_result;
                    if (i / 7 == 1) {
                        auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 1>, TupleSrcDst, 32769 >(i, j, trace_path, sub_dir);
                        std::cout << " bubble_period = " << j << std::endl;
                        m_result << " bubble_period = " << j << std::endl;
                        experiment.set_bubble_period(j);
                        experiment.reset();
                        experiment.run_until(100000);
                        experiment.report(std::cout);
                        experiment.report(m_result);
                        experiment.print_info();
                    }
                    else {
                        auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 1>, Tuple_5, 32769 >(i, j, trace_path, sub_dir);
                        std::cout << " bubble_period = " << j << std::endl;
                        m_result << " bubble_period = " << j << std::endl;
                        experiment.set_bubble_period(j);
                        experiment.reset();
                        experiment.run_until(100000);
                        experiment.report(std::cout);
                        experiment.report(m_result);
                        experiment.print_info();
                    }
                    os << m_result.str();
                    os.close();
                }
            }
        }
    }
    else {
        run_experiment_bench_T1<0, 2, 1, 3>(100'000);
    }
    return 0;
}

int main1(int argc, char const* argv[])
{
    if constexpr (TEST_MODE) {
        //Experiment<SinglePeer<8, 0, 1, 2>, 2> experiment;
        //Experiment<SongPipeline<128, 32769, 4>, 32769> experiment;
        
        // experiment.set_lambda(0.9);
        
        //experiment.initialize_write_back_generator({ { 0, 0.9 }, { 1, 0.1 } });
        std::ofstream os;
        //os.open("result.txt", std::ios_base::out);
        //for (int j = 9; j >= 1; j--) {
        //for (double lambda{ 0.1 }; lambda <= 0.9 + 1e-8; lambda += 0.1) {
            //std::array<std::stringstream, 10> m_results {};
            //for (int i = 9; i >= 0; i--) {
                //std::cout << "lambda " << lambda << " ; wb_gap " << i << std::endl;
        //for (int i = 0; i < 12; i++) {
        std::string pir_name[7] = { "pl", "mq2", "rr", "sq", "mq4" , "mq8", "direct_single"};
        std::string use_case_name[5] = {"nat", "ddos", "bfd", "topk", "wwww"};

        /*
        *   i = 0, 1, 2: NAT
        *   i = 3, 4, 5: DDoS
            i = 6, 7, 8: big flow detector
            i = 9, 10, 11: TopK
        */
        // 3 * 7 + 0 是per-flow
        // 3 * 7 + 1 是multi queue 2
        // 3 * 7 + 2 是round-robin
        // 3 * 7 + 3 是single-queue
        // 3 * 7 + 4 是multi queue 4
        // 3 * 7 + 5 是multi queue 8
        // 3 * 7 + 6 是direct single queue
        std::vector<std::string> trace_paths = { "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\res_topk_nic_14\\baidu_build_trace_1_NIC_400W_from_5000W.txt",
                                                "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\res_topk_nic_42\\baidu_build_trace_3_NIC_400W_from_5000W.txt" ,
                                                "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\res_topk_nic_84\\baidu_build_trace_6_NIC_400W_from_5000W.txt" ,
                                                "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\res_topk_nic_98\\baidu_build_trace_7_NIC_400W_from_5000W.txt" };
        std::vector<int> time_nums = { 1, 3, 6, 7 }; // 倍数

        for (auto time : time_nums) {
            int j = time * 14;
            std::string trace_path = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\res_topk_nic_" + std::to_string(j) + "\\baidu_build_trace_" 
                        + std::to_string(time) +"_NIC_400W_from_5000W.txt";
            std::string sub_dir = "./res_topk_nic_" + std::to_string(j) + "/";
            for (int i = 21; i <= 27; i++) {
                std::string file_name = ".\\res_topk_nic_" + std::to_string(j) + "\\" + pir_name[i % 7] + "_" + use_case_name[i / 7] + "_" + std::to_string(j) + ".txt";
                std::cout << file_name << " is used!" << std::endl;
                os.open(file_name, std::ios_base::out);
                std::stringstream m_result;
                if (i / 7 == 1) {
                    auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 524288>, TupleSrcDst, 32769 >(i, j, trace_path, sub_dir);
                    std::cout << " bubble_period = " << j << std::endl;
                    //m_results[j-1] << " bubble_period = " << j << std::endl;
                    m_result << " bubble_period = " << j << std::endl;
                    experiment.set_bubble_period(j);
                    experiment.reset();
                    experiment.run_until(100000);
                    experiment.report(std::cout);
                    experiment.report(m_result);
                    experiment.print_info();
                }
                else {
                    auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 524288>, Tuple_5, 32769 >(i, j, trace_path, sub_dir);
                    std::cout << " bubble_period = " << j << std::endl;
                    m_result << " bubble_period = " << j << std::endl;
                    experiment.set_bubble_period(j);
                    experiment.reset();
                    experiment.run_until(100000);
                    experiment.report(std::cout);
                    experiment.report(m_result);
                    experiment.print_info();
                }

                os << m_result.str();
                os.close();
            }
        }

        //for (int i = 21; i <= 27; i++) {
        //    /*std::string file_name = ".\\test_trace\\result\\" + pir_name[i % 7] + "_" + use_case_name[i / 7] + ".txt";
        //    std::cout << file_name << " is used!" << std::endl;
        //    os.open(file_name, std::ios_base::out);*/
        //    //std::array<std::stringstream, 10> m_results {};
        //    std::string trace_path = "C:\\Users\\PC\\Desktop\\RAPID-SIM2\\res_topk_nic_28\\baidu_build_trace_2_NIC_400W_from_5000W.txt";
        //    std::string sub_dir = "./res_topk_nic_28/";
        //    for (int j = 28; j <= 28; j++) { // 14 tested
        //        std::string file_name = ".\\res_topk_nic_28\\" + pir_name[i % 7] + "_" + use_case_name[i / 7] + "_" + std::to_string(j) + ".txt";
        //        std::cout << file_name << " is used!" << std::endl;
        //        os.open(file_name, std::ios_base::out); 
        //        std::stringstream m_result;
        //        if (i / 7 == 1) {
        //            auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 524288>, TupleSrcDst, 32769 >(i, j, trace_path, sub_dir);
        //            std::cout << " bubble_period = " << j << std::endl;
        //            //m_results[j-1] << " bubble_period = " << j << std::endl;
        //            m_result << " bubble_period = " << j << std::endl;
        //            experiment.set_bubble_period(j);
        //            experiment.reset();
        //            experiment.run_until(100000);
        //            experiment.report(std::cout);
        //            experiment.report(m_result);
        //            experiment.print_info();
        //        }
        //        else {
        //            auto experiment = Experiment < ImprovedSongPipeline<64, 32769, 4, 524288>, Tuple_5, 32769 >(i, j, trace_path, sub_dir);
        //            std::cout << " bubble_period = " << j << std::endl;
        //            m_result << " bubble_period = " << j << std::endl;
        //            experiment.set_bubble_period(j);
        //            experiment.reset();
        //            experiment.run_until(100000);
        //            experiment.report(std::cout);
        //            experiment.report(m_result);
        //            experiment.print_info();
        //        }

        //        os << m_result.str();
        //        os.close();
        //    }
        //}
        
    } else {
        run_experiment_bench_T1<0, 2, 1, 3>(100'000);
    }
    return 0;
}