


#ifndef ARITH_VEC_H
#define ARITH_VEC_H


#include <iostream>
#include <cassert>
#include <intrin.h>

// Implementation dependant of compiler optimizing removing the forloops.

// Implementation optimized for small N, eg(2-4)


template<class T, class V, unsigned int K>
class API ArithVec {
protected:

	/// Actual data of the vector
	T data[K];

	/// Construct uninitialized vector
	ArithVec()
	{
	}

	/// Construct a vector where all coordinates are identical
	explicit ArithVec(T _a)
	{
		for (int i = 0; i < K; i++) {
			data[i] = _a;
		}
	}

	/// construc a vector from an array of elements
	ArithVec(const T a[K])
	{
		for (int i = 0; i < K; i++) {
			data[i] = a[i];
		}
	}

	/// Construct a 2D vector 
	ArithVec(T _a, T _b):
		data{_a, _b}
	{
		assert(K == 2);
	}

	/// Construct a 3D vector
	ArithVec(T _a, T _b, T _c):
		data{_a, _b, _c}
	{
		assert(K == 3);
	}

	/// Construct a 4D vector
	ArithVec(T _a, T _b, T _c, T _d):
		data{_a, _b, _c, _d}
	{
		assert(K == 4);
	}

public:

	constexpr unsigned int getDim() { return K; }

	/// Const index operator
	inline const T& operator [] (unsigned int i) const
	{
		assert(i < K);
		return data[i];
	}

	/// Non-const index operator
	inline T& operator [] (unsigned int i)
	{
		assert(i < K);
		return data[i];
	}

	/// Assignment addition with scalar
	inline bool operator==(V v)
	{
		for (int i = 0; i < K; i++) {
			if (data[i] != v[i]) {
				return false;
			}
		}
		return true;
	}

	/// Assignment addition with scalar
	inline V & operator+=(T c)
	{
		for (int i = 0; i < K; i++) {
			data[i] += c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment subtraction with scalar
	inline V & operator-=(T c)
	{
		for (int i = 0; i < K; i++) {
			data[i] -= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment multiplication with scalar
	inline V & operator*=(T c)
	{
		for (int i = 0; i < K; i++) {
			data[i] *= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment division with scalar
	inline V & operator/=(T c)
	{
		for (int i = 0; i < K; i++) {
			data[i] /= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment addition with vector
	inline V & operator+=(V const &other)
	{
		for (int i = 0; i < K; i++) {
			data[i] += other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment subtraction with vector
	inline V & operator-=(V const &other)
	{
		for (int i = 0; i < K; i++) {
			data[i] -= other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment multiplication with vector
	inline V & operator*=(V const &other)
	{
		for (int i = 0; i < K; i++) {
			data[i] *= other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment division with vector
	inline V & operator/=(V const &other)
	{
		for (int i = 0; i < K; i++) {
			data[i] /= other[i];
		}
		return static_cast<V&>(*this);
	}

	inline V operator-() const
	{
		V v_new;
		for (int i = 0; i < K; i++) {
			v_new[i] = -data[i];
		}
		return v_new;
	}

	// #### #### #### #### #### #### #### ####
	// #### Binary expressions

	inline V operator * (const ArithVec<T,V,K> &other) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] * other[i];
		}
		return res;
	}

	inline V operator / (const ArithVec<T, V, K> &other) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] / other[i];
		}
		return res;
	}
	inline V operator - (const ArithVec<T, V, K> &other) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] - other[i];
		}
		return res;
	}
	inline V operator + (const ArithVec<T, V, K> &other) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] + other[i];
		}
		return res;
	}

	inline V operator * (const T &c) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] * c;
		}
		return res;
	}

	inline V operator / (const T &c) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] / c;
		}
		return res;
	}
	inline V operator - (const T &c) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] - c;
		}
		return res;
	}
	inline V operator + (const T &c) const {
		V res;
		for (int i = 0; i < K; i++) {
			res[i] = data[i] + c;
		}
		return res;
	}

	/// Sum of all elements
	inline T element_sum() const {
		float t = 0;
		for (int i = 0; i < K; i++) {
			t += data[i];
		}
		return t;
	}

	/// Find the axis with the highest element
	inline unsigned int max_axis() const {
		unsigned int axis = 0;
		for (int i = 1; i < K; i++) {
			axis = data[axis] > data[i] ? axis : i;
		}
		return axis;
	}

	/// Find the axis with the lowest element
	inline unsigned int min_axis() const {
		unsigned int axis = 0;
		for (int i = 1; i < K; i++) {
			axis = data[axis] < data[i] ? axis : i;
		}
		return axis;
	}

}; // end of member functions

template <class T, class V, unsigned int K>
inline V operator*(const T& c, const ArithVec<T, V, K>& v) {
	V res;
	for (int i = 0; i < K; i++) {
		res[i] = c * v[i];
	}
	return res;
}

template <class T, class V, unsigned int K>
inline V operator/(const T& c, const ArithVec<T, V, K>& v) {
	V res;
	for (int i = 0; i < K; i++) {
		res[i] = c / v[i];
	}
	return res;
}

template <class T, class V, unsigned int K>
inline V operator+(const T& c, const ArithVec<T, V, K>& v) {
	V res;
	for (int i = 0; i < K; i++) {
		res[i] = c + v[i];
	}
	return res;
}

template <class T, class V, unsigned int K>
inline V operator-(const T& c, const ArithVec<T, V, K>& v) {
	V res;
	for (int i = 0; i < K; i++) {
		res[i] = c - v[i];
	}
	return res;
}

template <class T, class V, unsigned int K>
inline T dot(const ArithVec<T, V, K>& v0, const ArithVec<T, V, K>& v1) {
	return (v0*v1).element_sum();
}


template <class T, class V, unsigned int K>
inline T sqr_length(const ArithVec<T, V, K>& v) {
	return (v*v).element_sum();
}

template <class T, class V, unsigned int K>
inline V abs(const ArithVec<T, V, K>& v) {
	V res;
	for (int i = 0; i < K; i++) {
		res[i] = v[i] < 0 ? -v[i] : v[i];
	}
	return res;
}

template<class T, class V, unsigned int K>
inline V min(const ArithVec<T, V, K>& v0, const ArithVec<T, V, K>& v1) {
	V v;
	for (int i = 0; i < K; i++) {
		v[i] = v0[i] < v1[i] ? v0[i] : v1[i];
	}
	return v;
}

template<class T, class V, unsigned int K>
inline V max(const ArithVec<T, V, K>& v0, const ArithVec<T, V, K>& v1) {
	V v;
	for (int i = 0; i < K; i++) {
		v[i] = v0[i] > v1[i] ? v0[i] : v1[i];
	}
	return v;
}

inline float min(const float& a, const float& b) {
	return a < b ? a : b;
}

inline float max(const float& a, const float& b) {
	return a > b ? a : b;
}

inline float abs(const float& c) {
	return c < 0 ? -c : c;
}

template <class T, class V, unsigned int K>
inline std::ostream& operator<<(std::ostream&os, const ArithVec<T, V, K>& v)
{
	os << "[ ";
	for (unsigned int i = 0; i < K; i++) os << v[i] << " ";
	os << "]";
	return os;
}

inline unsigned int clz(uint64_t code)
{
	return __lzcnt64(code);
}

inline unsigned int clz(uint32_t code)
{
	return __lzcnt(code);
}

inline unsigned int clz(uint16_t code)
{
	return __lzcnt16(code);
}

inline int fast_log2(int x) {
	if (x < 1)
		return -1;
	return 8 * sizeof(int) - clz(static_cast<uint32_t>(x)) - 1;
}

#endif // !ARITH_VEC_H
