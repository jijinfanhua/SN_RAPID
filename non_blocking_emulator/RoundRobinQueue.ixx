export module rapid.RoundRobinQueue;

import std;

export template <size_t K>
class RoundRobinQueue {
    std::array<std::uint32_t, K> m_schedule_queue;
    int m_front { 0 };
    int m_back { 0 };
    size_t m_size{ 0 };

public:
    RoundRobinQueue() = default;

    bool is_empty()
    {
        return m_front == m_back;
    }
    bool is_full() {
        return m_size == K;
    }

    void enqueue(std::uint32_t key)
    {
        if (is_full()) {
            std::cout << "schedule queue is full, need more schedule size" << std::endl;
            return;
        }
        m_schedule_queue[m_back] = key;
        if (++m_back == K) {
            m_back = 0;
        }
        m_size++;
    }

    std::uint32_t dequeue()
    {
        if (is_empty()) {
            return 0;
        }
        std::uint32_t key { m_schedule_queue[m_front] };
        if (++m_front == K) {
            m_front = 0;
        }
        m_size--;
        return key;
    }

    bool remove( std::uint32_t key )
    {
        if (is_empty())
            return false;
        
        auto size = m_size;
        for (int i = 0; i < size; i++) {
            auto x = dequeue();
            if (x == key) {
                //std::cout << "successfully remove" << std::endl;
                return true;
            } 
            enqueue(x);
        }
        return false;
    }


    size_t size() const {
        return m_size;
    }
};