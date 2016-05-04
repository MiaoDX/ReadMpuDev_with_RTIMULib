#pragma once
/******* some calculate method and helper classes *******/

#include <cstring> //memcpy
#include <iostream>
#include <cmath>
#include <algorithm>
//#include "SBGC_lib/SBGC.h"

#define RTFLOAT double

/** copy from RTIMULib to provide a nicer interface to use roll\pitch\yaw  tuple*/
template<class RTTYPE>
class RTVector3_T
{

public:

	RTVector3_T();
	RTVector3_T(RTTYPE x, RTTYPE y, RTTYPE z);

	const RTVector3_T&  operator +=(RTVector3_T& vec);
	const RTVector3_T&  operator -=(RTVector3_T& vec);

	RTVector3_T& operator =(const RTVector3_T& vec);

	void zero();

	inline RTTYPE x() const { return m_data[0]; }
	inline RTTYPE y() const { return m_data[1]; }
	inline RTTYPE z() const { return m_data[2]; }
	inline RTTYPE data(const int i) const { return m_data[i]; }
	inline RTTYPE abs_max() const { return (std::max)({ abs(m_data[0]), abs(m_data[1]), abs(m_data[2]) }); }

	inline void setX(const RTTYPE val) { m_data[0] = val; }
	inline void setY(const RTTYPE val) { m_data[1] = val; }
	inline void setZ(const RTTYPE val) { m_data[2] = val; }
	inline void setData(const int i, RTTYPE val) { m_data[i] = val; }
	inline void fromArray(RTTYPE *val) { memcpy(m_data, val, 3 * sizeof(RTTYPE)); }
	inline void toArray(RTTYPE *val) const { memcpy(val, m_data, 3 * sizeof(RTTYPE)); }

	/**** addby miaodx to provide SBGC_ANGLE_TO_DEGREE(int2float) and  SBGC_DEGREE_TO_ANGLE(float2int) *****/
	inline void print_out();
	inline void angle_from_degree(RTVector3_T<double>& vec);
	inline void degree_from_angle(RTVector3_T<int>& vec);
	// 方式1：重载operator<用于排序时的比较(写在函数体内)  
	bool operator< (const RTVector3_T rt)
	{
		return this->y() < rt.y();
	}
	

private:
	double m_data[3];
};


struct CompZ
{
	bool operator()(const RTVector3_T<RTFLOAT>& pl, const RTVector3_T<RTFLOAT>& pr)
	{
		return pl.z() < pr.z();
	}
};

//
//  The RTVector3_T class
template<class RTTYPE>
RTVector3_T<RTTYPE>::RTVector3_T()
{
	zero();
}
template<class RTTYPE>
RTVector3_T<RTTYPE>::RTVector3_T(RTTYPE x, RTTYPE y, RTTYPE z)
{
	m_data[0] = x;
	m_data[1] = y;
	m_data[2] = z;
}
template<class RTTYPE>
RTVector3_T<RTTYPE>& RTVector3_T<RTTYPE>::operator =(const RTVector3_T& vec)
{
	if (this == &vec)
		return *this;

	m_data[0] = vec.m_data[0];
	m_data[1] = vec.m_data[1];
	m_data[2] = vec.m_data[2];

	return *this;
}

template<class RTTYPE>
const RTVector3_T<RTTYPE>& RTVector3_T<RTTYPE>::operator +=(RTVector3_T& vec)
{
	for (int i = 0; i < 3; i++)
		m_data[i] += vec.m_data[i];
	return *this;
}
template<class RTTYPE>
const RTVector3_T<RTTYPE>& RTVector3_T<RTTYPE>::operator -=(RTVector3_T& vec)
{
	for (int i = 0; i < 3; i++)
		m_data[i] -= vec.m_data[i];
	return *this;
}
template<class RTTYPE>
void RTVector3_T<RTTYPE>::zero()
{
	for (int i = 0; i < 3; i++)
		m_data[i] = 0;
}

template<class RTTYPE>
void RTVector3_T<RTTYPE>::print_out() {
	for (int i = 0; i < 3; i++)
		std::cout << " " << m_data[i];

	std::cout << std::endl;
}

//template<class RTTYPE>
//void RTVector3_T<RTTYPE>::angle_from_degree(RTVector3_T<double>& vec) {
//	for (int i = 0; i < 3; i++)
//		setData(i, SBGC_DEGREE_TO_ANGLE(vec.data(i)));
//	//vec.setData(i, SBGC_ANGLE_TO_DEGREE(data(i)));
//}
//
//template<class RTTYPE>
//void RTVector3_T<RTTYPE>::degree_from_angle(RTVector3_T<int>& vec) {
//	for (int i = 0; i < 3; i++)
//		setData(i, SBGC_ANGLE_TO_DEGREE(vec.data(i)));
//	//vec.setData(i, SBGC_DEGREE_TO_ANGLE(data(i)));
//}

