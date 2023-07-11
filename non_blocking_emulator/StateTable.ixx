export module rapid.StateTable;

import rapid.Packet;
import std;

//export int g_clock{ 0 };
export class StateTable {
public:
	StateTable() = default;

	virtual void update_state(Packet pkt) {
		return;
	}

	virtual bool require_write_back(Packet& pkt) {
		return false;
	}
	void call_update_state(Packet pkt) {
		update_state(pkt);
	}

	bool call_require_write_back(Packet& pkt) {
		return require_write_back(pkt);
	}

	Packet set_write_back(Packet&& pkt) {
		if (!pkt.is_empty()) {
			if (require_write_back(pkt)) {
				pkt.m_write_back_bitmap = std::byte(1);
			}
			else {
				pkt.m_write_back_bitmap = std::byte(0);
			}
		}
		return pkt;
	}
};


export class NatStateTable : public StateTable {
	enum NatState {
		Strange,
		Familiar
	};
	int time_end{ 500 };
	std::map<std::uint32_t, NatState> ConnectTable;
	std::map<std::uint32_t, int> Timer;
public:
	NatStateTable() {
		std::cout << "Nat" << std::endl;
	};

	void update_state(Packet pkt) override {
		if (!pkt.is_empty()) {
			ConnectTable[pkt.m_key] = Familiar;
			
		}
		return;
	}

	bool require_write_back(Packet& pkt) override {
		if (ConnectTable.find(pkt.m_key) != ConnectTable.end()) {
			switch (ConnectTable[pkt.m_key])
			{
			case Strange:
				Timer[pkt.m_key] = g_clock;
				return true;
			case Familiar:
				if (Timer.find(pkt.m_key) != Timer.end()) {
					if (g_clock - Timer[pkt.m_key] > time_end) {
						Timer[pkt.m_key] = g_clock;
						return true;
					}
					else {
						Timer[pkt.m_key] = g_clock;
						return false;
					}
				}
				else {
					std::cout << "should not be here" << std::endl;
				}
				return false;
			default:
				std::cout << "should not be here" << std::endl;
				return false;
			}
		}
		else {
			Timer[pkt.m_key] = g_clock;
			return true; // must be a strange flow
		}
	}

};

export class DDosStateTable: public StateTable {
	enum DDosState {
		LEVEL0,
		LEVEL1,
		LEVEL2,
		BAD
	};

	int time_end{ 500 };
	int num1 = { 1 };
	int num2 = { 5 };
	int num3 = { 10 };
	std::map<std::uint32_t, DDosState> StateTable;
	std::map<std::uint32_t, uint32_t> CounterTable;
	std::map<std::uint32_t, int> Timer;
public:
	DDosStateTable() {
		std::cout << "ddos" << std::endl;
	};

	void update_state(Packet pkt) override {
		if (!pkt.is_empty()) {
			if (0 <= pkt.m_cnt_syn && pkt.m_cnt_syn < num1) {
				StateTable[pkt.m_key] = LEVEL0;
			}
			else if (num1 <= pkt.m_cnt_syn && pkt.m_cnt_syn < num2) {
				StateTable[pkt.m_key] = LEVEL1;
			}
			else if (num2 <= pkt.m_cnt_syn && pkt.m_cnt_syn < num3) {
				StateTable[pkt.m_key] = LEVEL2;
			}
			else {
				StateTable[pkt.m_key] = BAD;
			}
		}
		return;
	}

	bool require_write_back(Packet& pkt) override {
		if (CounterTable.find(pkt.m_key) == CounterTable.end()) {
			CounterTable[pkt.m_key] = 0;
		}
		if (!pkt.is_resubmit) {
			if (Timer.find(pkt.m_key) == Timer.end()) {
				Timer[pkt.m_key] = g_clock;
			}
			if (g_clock - Timer[pkt.m_key] > time_end) {
				bool write_back = CounterTable[pkt.m_key] >= num1;
				CounterTable[pkt.m_key] = 0;
				pkt.m_cnt_syn = 0;
				Timer[pkt.m_key] = g_clock;
				return write_back;
			}
			else if (pkt.is_syn && pkt.is_ack) {
				CounterTable[pkt.m_key]--;
			}
			else if (pkt.is_syn) {
				CounterTable[pkt.m_key]++;
			}
			Timer[pkt.m_key] = g_clock;
		}
		pkt.m_cnt_syn = CounterTable[pkt.m_key];
		if (CounterTable[pkt.m_key] == num1 || CounterTable[pkt.m_key] == num2 || CounterTable[pkt.m_key] == num3) {
			return true;
		}
		else {
			return false;
		}
	}


};

export class BFDStateTable: public StateTable {
	enum BFDState {
		LEVEL0,
		LEVEL1,
		LEVEL2,
		BAD
	};

	int time_end{ 5000 };
	int num1 = { 1 };
	int num2 = { 5 };
	int num3 = { 10 };
	std::map<std::uint32_t, BFDState> StateTable;
	std::map<std::uint32_t, uint32_t> CounterTable;
	std::map<std::uint32_t, int> Timer;
public:
	BFDStateTable() {
		std::cout << "Big flow detect" << std::endl;
	};

	void update_state(Packet pkt) override {
		if (!pkt.is_empty()) {
			if (0 <= pkt.m_cnt_syn && pkt.m_cnt_syn < num1) {
				StateTable[pkt.m_key] = LEVEL0;
			}
			else if (num1 <= pkt.m_cnt_syn && pkt.m_cnt_syn < num2) {
				StateTable[pkt.m_key] = LEVEL1;
			}
			else if (num2 <= pkt.m_cnt_syn && pkt.m_cnt_syn < num3) {
				StateTable[pkt.m_key] = LEVEL2;
			}
			else {
				StateTable[pkt.m_key] = BAD;
			}
		}
		return;
	}

	bool require_write_back(Packet& pkt) override {
		if (CounterTable.find(pkt.m_key) == CounterTable.end()) {
			CounterTable[pkt.m_key] = 0;
		}
		if (!pkt.is_resubmit) {
			if (Timer.find(pkt.m_key) == Timer.end()) {
				Timer[pkt.m_key] = g_clock;
			}
			if (g_clock - Timer[pkt.m_key] > time_end) {
				bool write_back = CounterTable[pkt.m_key] >= num1;
				CounterTable[pkt.m_key] = 0;
				pkt.m_cnt_syn = 0;
				Timer[pkt.m_key] = g_clock;
				return write_back;
			}
			CounterTable[pkt.m_key]++;
			Timer[pkt.m_key] = g_clock;
		}
		pkt.m_cnt_syn = CounterTable[pkt.m_key];
		if (!pkt.is_resubmit && (CounterTable[pkt.m_key] == num1 || CounterTable[pkt.m_key] == num2 || CounterTable[pkt.m_key] == num3)) {
			return true;
		}
		else {
			return false;
		}
	}
};


export template <size_t K = 300>  class TopKStateTable : public StateTable{
	std::array<std::pair<std::uint32_t, std::uint32_t>, K> TopKTable;

public:
	TopKStateTable() {
		std::cout << "Top K" << std::endl;
	};

	void update_state(Packet pkt) override {
		if (pkt.is_empty()) {
			return;
		}
		std::uint32_t addr_1 = pkt.m_top_k_addr[0] % K;
		std::uint32_t addr_2 = pkt.m_top_k_addr[1] % K;
		while (addr_2 == addr_1) {
			addr_2 = (addr_2 + 1) % K;
		}
		std::uint32_t addr_3 = pkt.m_top_k_addr[2] % K;
		while (addr_3 == addr_2 || addr_3 == addr_1) {
			addr_3 = (addr_3 + 1) % K;
		}
		auto [flow_id_1, count_1] {TopKTable[addr_1]};
		auto [flow_id_2, count_2] {TopKTable[addr_2]};
		auto [flow_id_3, count_3] {TopKTable[addr_3]};
		if (count_1 < count_2) {
			if (count_1 < count_3) {
				TopKTable[addr_1] = std::make_pair(pkt.m_key, 1);
			}
			else {
				TopKTable[addr_3] = std::make_pair(pkt.m_key, 1);
			}
		}
		else {
			if (count_2 < count_3) {
				TopKTable[addr_2] = std::make_pair(pkt.m_key, 1);
			}
			else {
				TopKTable[addr_3] = std::make_pair(pkt.m_key, 1);
			}
		}

		return;
	}

	bool check_hit(std::uint32_t addr, std::uint32_t target_flow_id) {
		auto [flow_id, count] {TopKTable[addr]};
		if (target_flow_id == flow_id) {
			TopKTable[addr].second++;
			return true;
		}
		else {
			return false;
		}
	}

	bool require_write_back(Packet& pkt) override {
		std::uint32_t addr_1 = pkt.m_top_k_addr[0] % K;
		std::uint32_t addr_2 = pkt.m_top_k_addr[1] % K;
		while (addr_2 == addr_1) {
			addr_2 = (addr_2 + 1) % K;
		}
		std::uint32_t addr_3 = pkt.m_top_k_addr[2] % K;
		while (addr_3 == addr_2 || addr_3 == addr_1) {
			addr_3 = (addr_3 + 1) % K;
		}
		if (check_hit(addr_1, pkt.m_key)) {
			return false;
		} 
		if (check_hit(addr_2, pkt.m_key)) {
			return false;
		}
		if (check_hit(addr_3, pkt.m_key)) {
			return false;
		}
		return true;
	}


};

export class SimpleTable :public StateTable {
public:
	SimpleTable() = default;

	void update_state(Packet pkt) override {
		return;
	}

	bool require_write_back(Packet& pkt) override {
		return pkt.m_write_back_bitmap == std::byte(1);
	}
};