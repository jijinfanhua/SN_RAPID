export module rapid.experimental.bench;

import rapid.SongPipeline;
import rapid.ImprovedSongPipeline;
import rapid.experimental;
import std;

export template <size_t RID1, size_t WID1, size_t RID2, size_t WID2, size_t EXPLICIT_CLOCK_MAX>
class ExperimentBench {

    size_t front_buffer_size{ 0 };

public:
    ExperimentBench() = default;

    void initialize() {
        //Experiment<ImprovedSongPipeline<256, 32769, 4>, 32769> m_experiment;
    }

    void run_all(std::ostream& os, int packet_number) {
        //for (double lambda{ 0.1 }; lambda <= 0.9 + 1e-8; lambda += 0.1) {
        //for (size_t arr_period{ 10 }; arr_period >= 2; arr_period--){
            //std::array<std::stringstream, 10> m_results {};
        std::ofstream os;
        std::string pir_name[3] = { "pl", "mq", "rr" };
        std::string use_case_name[5] = { "nat", "ddos", "bfd", "topk", "wwww" };

        /*
        *   i = 0, 1, 2: NAT
        *   i = 3, 4, 5: DDoS
            i = 6, 7, 8: big flow detector
            i = 9, 10, 11: TopK
        */
        for (int i = 6; i < 9; i++) {
            std::string file_name = ".\\test_trace\\result\\" + pir_name[i % 3] + "_" + use_case_name[i / 3] + ".txt";
            os.open(file_name, std::ios_base::out);
            std::array<std::stringstream, 10> m_results {};
            for (int j = 1; j <= 10; j++) {
                if (i / 3 == 1) {
                    auto experiment = Experiment < ImprovedSongPipeline<128, 32769, 2, 2048>, TupleSrcDst, 32769 >(i);
                    std::cout << " bubble_period = " << j << std::endl;
                    m_results[j - 1] << " bubble_period = " << j << std::endl;
                    experiment.set_bubble_period(j);
                    experiment.reset();
                    experiment.run_until(100000);
                    experiment.report(std::cout);
                    experiment.report(m_results[j - 1]);
                    experiment.print_info();
                }
                else {
                    auto experiment = Experiment < ImprovedSongPipeline<128, 32769, 2, 2048>, Tuple_5, 32769 >(i);
                    std::cout << " bubble_period = " << j << std::endl;
                    m_results[j - 1] << " bubble_period = " << j << std::endl;
                    experiment.set_bubble_period(j);
                    experiment.reset();
                    experiment.run_until(100000);
                    experiment.report(std::cout);
                    experiment.report(m_results[j - 1]);
                    experiment.print_info();
                }

            }
            for (int i = 0; i < 10; i++) {
                os << m_results[i].str();
            }
            os.close();

        }
    }
};