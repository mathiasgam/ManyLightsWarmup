


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
	const T& operator [] (unsigned int i) const
	{
		assert(i < N);
		return data[i];
	}

	/// Non-const index operator
	T& operator [] (unsigned int i)
	{
		assert(i < N);
		return data[i];
	}

	/// Assignment addition with scalar
	bool operator==(V v)
	{
		for (int i = 0; i < N; i++) {
			if (data[i] != v[i]) {
				return false;
			}
		}
		return true;
	}

	/// Assignment addition with scalar
	V & operator+=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] += c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment subtraction with scalar
	V & operator-=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] -= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment multiplication with scalar
	V & operator*=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] *= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment division with scalar
	V & operator/=(T c)
	{
		for (int i = 0; i < N; i++) {
			data[i] /= c;
		}
		return static_cast<V&>(*this);
	}

	/// Assignment addition with vector
	V & operator+=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] += other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment subtraction with vector
	V & operator-=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] -= other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment multiplication with vector
	V & operator*=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] *= other[i];
		}
		return static_cast<V&>(*this);
	}

	/// Assignment division with vector
	V & operator/=(V const &other)
	{
		for (int i = 0; i < N; i++) {
			data[i] /= other[i];
		}
		return static_cast<V&>(*this);
	}

	V operator-() const
	{
		V v_new;
		for (int i = 0; i < N; i++) {
			v_new[i] = -data[i];
		}
		return v_new;
	}

	// #### #### #### #### #### #### #### ####
	// #### Binary expressions

	V operator * (const ArithVec<T,V,N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] * other[i];
		}
		return res;
	}

	V operator / (const ArithVec<T, V, N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] / other[i];
		}
		return res;
	}
	V operator - (const ArithVec<T, V, N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] - other[i];
		}
		return res;
	}
	V operator + (const ArithVec<T, V, N> &other) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] + other[i];
		}
		return res;
	}

	V operator * (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] * c;
		}
		return res;
	}

	V operator / (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] / c;
		}
		return res;
	}
	V operator - (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] - c;
		}
		return res;
	}
	V operator + (const T &c) const {
		V res;
		for (int i = 0; i < N; i++) {
			res[i] = data[i] + c;
		}
		return res;
	}

	/// Sum of all elements
	T element_sum() const {
		float t = 0;
		for (int i = 0; i < N; i++) {
			t += data[i];
		}
		return t;
	}


}; // end of member functions


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


template <class T, class V, unsigned int N>
inline std::ostream& operator<<(std::ostream&os, const ArithVec<T, V, N>& v)
{
	os << "[ ";
	for (unsigned int i = 0; i < N; i++) os << v[i] << " ";
	os << "]";
	return os;
}


#endif // !ARITH_VEC_H
