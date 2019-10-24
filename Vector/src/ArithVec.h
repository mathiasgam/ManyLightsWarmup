


#ifndef ARITH_VEC_H
#define ARITH_VEC_H


#include <iostream>
#include <cassert>

// Implementation dependant of compiler optimizing removing the forloops.

// Implementation optimized for small N, eg(2-4)


template<class T, class V, unsigned int N>
class API ArithVec {
protected:

	/// Actual data of the vector
	T data[N];

	/// Construct uninitialized vector
	ArithVec()
	{
	}

	/// Construct a vector where all coordinates are identical
	explicit ArithVec(T _a)
	{
		for (int i = 0; i < N; i++) {
			data[i] = _a;
		}
	}

	/// construc a vector from an array of elements
	ArithVec(const T a[N])
	{
		for (int i = 0; i < N; i++) {
			data[i] = a[i];
		}
	}

	/// Construct a 2D vector 
	ArithVec(T _a, T _b):
		data{_a, _b}
	{
		assert(N == 2);
	}

	/// Construct a 3D vector
	ArithVec(T _a, T _b, T _c):
		data{_a, _b, _c}
	{
		assert(N == 3);
	}

	/// Construct a 4D vector
	ArithVec(T _a, T _b, T _c, T _d):
		data{_a, _b, _c, _d}
	{
		assert(N == 4);
	}

public:

	constexpr unsigned int getDim() { return N; }

	/// Const index operator
	inline const T& operator [] (unsigned int i) const
	{
		assert(i < N);
		return data[i];
	}

	/// Non-const index operator
	inline T& operator [] (unsigned int i)
	{
		assert(i < N);
		return data[i];
	}

	/// Assignment addition with scalar
	inline bool operator==(V v)
	{
		for (int i = 0; i < N; i++) {
			if (data[i] != v[i]) {
				return false;
			}
		}
		return true;
	}

	/// Assignment addition with scalar
	inline V & operator+=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] += c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment subtraction with scalar
	inline V & operator-=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] -= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment multiplication with scalar
	inline V & operator*=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] *= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment division with scalar
	inline V & operator/=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] /= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment addition with vector
	inline V & operator+=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] += other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment subtraction with vector
	inline V & operator-=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] -= other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment multiplication with vector
	inline V & operator*=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] *= other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment division with vector
	inline V & operator/=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] /= other[i];
		}
		return static_cast<V&>(*this);
	}

	inline V operator-() const
	{
		V v_new;
		for (int i = 0; i < N; i++) {
			v_new[i] = -data[i];
		}
		return v_new;
	}

	// #### #### #### #### #### #### #### ####
	// #### Binary expressions

	inline V operator * (const ArithVec<T,V,N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] * other[i];
		}
		return res;
	}

	inline V operator / (const ArithVec<T, V, N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] / other[i];
		}
		return res;
	}
	inline V operator - (const ArithVec<T, V, N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] - other[i];
		}
		return res;
	}
	inline V operator + (const ArithVec<T, V, N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] + other[i];
		}
		return res;
	}

	inline V operator * (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] * c;
		}
		return res;
	}

	inline V operator / (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] / c;
		}
		return res;
	}
	inline V operator - (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] - c;
		}
		return res;
	}
	inline V operator + (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] + c;
		}
		return res;
	}

	/// Sum of all elements
	inline T element_sum() const {
		float t = 0;
		for (int i = 0; i < N; i++) {
			t += data[i];
		}
		return t;
	}

}; // end of member functions

template <class T, class V, unsigned int N>
inline V operator*(const T& c, const ArithVec<T, V, N>& v) {
	V res;
	for (int i = 0; i < N; i++) {
		res[i] = c * v[i];
	}
	return res;
}

template <class T, class V, unsigned int N>
inline V operator/(const T& c, const ArithVec<T, V, N>& v) {
	V res;
	for (int i = 0; i < N; i++) {
		res[i] = c / v[i];
	}
	return res;
}

template <class T, class V, unsigned int N>
inline V operator+(const T& c, const ArithVec<T, V, N>& v) {
	V res;
	for (int i = 0; i < N; i++) {
		res[i] = c + v[i];
	}
	return res;
}

template <class T, class V, unsigned int N>
inline V operator-(const T& c, const ArithVec<T, V, N>& v) {
	V res;
	for (int i = 0; i < N; i++) {
		res[i] = c - v[i];
	}
	return res;
}

template <class T, class V, unsigned int N>
inline T dot(const ArithVec<T, V, N>& v0, const ArithVec<T, V, N>& v1) {
	return (v0*v1).element_sum();
}


template <class T, class V, unsigned int N>
inline T sqr_length(const ArithVec<T, V, N>& v) {
	return (v*v).element_sum();
}


template<class T, class V, unsigned int N>
inline V min(const ArithVec<T, V, N>& v0, const ArithVec<T, V, N>& v1) {
	V v;
	for (int i = 0; i < N; i++) {
		v[i] = v0[i] < v1[i] ? v0[i] : v1[i];
	}
	return v;
}

template<class T, class V, unsigned int N>
inline V max(const ArithVec<T, V, N>& v0, const ArithVec<T, V, N>& v1) {
	V v;
	for (int i = 0; i < N; i++) {
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

template <class T, class V, unsigned int N>
inline std::ostream& operator<<(std::ostream&os, const ArithVec<T, V, N>& v)
{
	os << "[ ";
	for (unsigned int i = 0; i < N; i++) os << v[i] << " ";
	os << "]";
	return os;
}


#endif // !ARITH_VEC_H
