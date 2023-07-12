export module rapid.FlowInfo;

import std;

export class BaseTuple {
public:
	std::uint32_t saddr;
	std::uint32_t daddr;
	std::uint8_t protocol;
	std::uint16_t sport;
	std::uint16_t dport;

	BaseTuple() {}
	BaseTuple(std::uint32_t _saddr, std::uint32_t _daddr, std::uint8_t _protocol, std::uint16_t _sport, std::uint16_t _dport) :
		saddr(_saddr), daddr(_daddr), protocol(_protocol), sport(_sport), dport(_dport) {
		std::cout << "init base" << std::endl;
	}
	virtual bool operator<(const BaseTuple& other) const {
		std::cout << "base compare" << std::endl;
		return false;
	}
};

export class Tuple_5: public BaseTuple {


public:
	std::uint32_t saddr;
	std::uint32_t daddr;
	std::uint8_t protocol;
	std::uint16_t sport;
	std::uint16_t dport;
	Tuple_5() {};
	Tuple_5(std::uint32_t _saddr, std::uint32_t _daddr, std::uint8_t _protocol, std::uint16_t _sport, std::uint16_t _dport) :
		saddr(_saddr), daddr(_daddr), protocol(_protocol), sport(_sport), dport(_dport) {}
	bool operator<(const Tuple_5& other) const {
		if (saddr < other.saddr) {
			return true;
		}
		else if (saddr > other.saddr) {
			return false;
		}
		else {
			if (daddr < other.daddr) {
				return true;
			}
			else if (daddr > other.daddr) {
				return false;
			}
			else {
				if (protocol < other.protocol) {
					return true;
				}
				else if (protocol > other.protocol) {
					return false;
				}
				else {
					if (sport < other.sport) {
						return true;
					}
					else if (sport > other.sport) {
						return false;
					}
					else {
						return dport < other.dport;
					}
				}
			}
		}
	}
};

export class TupleDstDport : public BaseTuple {

public:
	std::uint32_t daddr;
	std::uint16_t dport;
	TupleDstDport(std::uint32_t _daddr, std::uint16_t _dport): daddr(_daddr), dport(_dport) {}
	TupleDstDport(std::uint32_t _saddr, std::uint32_t _daddr, std::uint8_t _protocol, std::uint16_t _sport, std::uint16_t _dport) :
		daddr(_daddr), dport(_dport) {}
	TupleDstDport() {}
	bool operator<(const TupleDstDport& other) const {
		if (daddr < other.daddr) {
			return true;
		}
		else if (daddr > other.daddr) {
			return false;
		}
		else {
			return dport < other.dport;
		}
	}
};

export class TupleSrcDst: public BaseTuple {

public:
	std::uint32_t saddr;
	std::uint32_t daddr;
	TupleSrcDst(std::uint32_t _saddr, std::uint32_t _daddr): saddr(_saddr), daddr(_daddr) {}
	TupleSrcDst() {}
	TupleSrcDst(std::uint32_t _saddr, std::uint32_t _daddr, std::uint8_t _protocol, std::uint16_t _sport, std::uint16_t _dport) :
		saddr(_saddr), daddr(_daddr){
	}
	bool operator<(const TupleSrcDst& other) const {
		if (saddr < other.saddr) {
			return true;
		}
		else if (saddr > other.saddr) {
			return false;
		}
		else {
			return daddr < other.daddr;
		}
	}
	void print_info() {
		std::cout << saddr << " " << daddr << std::endl;
	}
};

export class TupleSrcDstDport: public BaseTuple {

public:
	std::uint32_t saddr;
	std::uint32_t daddr;
	std::uint16_t dport;
	TupleSrcDstDport(std::uint32_t _saddr, std::uint32_t _daddr, std::uint16_t _dport): saddr(_saddr), daddr(_daddr), dport(_dport) {}
	TupleSrcDstDport() {}
	TupleSrcDstDport(std::uint32_t _saddr, std::uint32_t _daddr, std::uint8_t _protocol, std::uint16_t _sport, std::uint16_t _dport) :
		saddr(_saddr), daddr(_daddr), dport(_dport) {}
	bool operator<(const TupleSrcDstDport& other) const {
		if (saddr < other.saddr) {
			return true;
		}
		else if (saddr > other.saddr) {
			return false;
		}
		else {
			if (daddr < other.daddr) {
				return true;
			}
			else if (daddr > other.daddr) {
				return false;
			}
			else {
				return dport < other.dport;
			}
		}
	}
};