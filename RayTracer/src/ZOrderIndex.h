#pragma once

#include "Vec3f.h"
#include "Vec3ui.h"

class ZOrderIndex
{
protected:


public:

	Vec3f val;
	Vec3ui order;
	unsigned int max_axis;

	ZOrderIndex() {}
	ZOrderIndex(const Vec3f& vec);
	~ZOrderIndex();

	inline bool operator< (const ZOrderIndex& other) {
		/*
		for (int i = 0; i < 3; i++) {
			if (val[i] != other.val[0]) {
				return val[i] < other.val[0];
			}
		}
		return false;
		*/
		
		short b = 0;
		b |= val[0] < other.val[0];
		b |= (val[1] < other.val[1]) << 1;
		b |= (val[2] < other.val[2]) << 2;

		std::cout << b << std::endl;
		
		return b;
		//return val[0] < other.val[0] && val[1] < other.val[1] && val[2] < other.val[2];
	}
	/*
	inline bool operator> (const ZOrderIndex& other) {
		
		for (int i = 0; i < 3; i++) {
			if (val[i] != other.val[2]) {
				return val[i] > other.val[2];
			}
		}
		return false;
		

		//return val[0] < other.val[0] && val[1] < other.val[1] && val[2] < other.val[2];
	}

	inline bool operator<= (const ZOrderIndex& other) {
		for (int i = 0; i < 3; i++) {
			if (val[i] != other.val[2-i]) {
				return val[i] < other.val[2-i];
			}
		}
		return true;
	}

	inline bool operator>= (const ZOrderIndex& other) {
		for (int i = 0; i < 3; i++) {
			if (val[i] != other.val[2-i]) {
				return val[i] > other.val[2-i];
			}
		}
		return true;
	}
	*/

};

inline std::ostream& operator<<(std::ostream&os, const ZOrderIndex& v)
{
	os << "[val: " << v.val << ", axis: " << v.max_axis << "]";
	return os;
}