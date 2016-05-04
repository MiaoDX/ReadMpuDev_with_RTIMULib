//#pragma once

//#include "myMath.h"

////
////  The RTVector3_T class
//template<class RTTYPE> 
//	RTVector3_T<RTTYPE>::RTVector3_T()
//{
//	zero();
//}
//template<class RTTYPE> 
//	RTVector3_T<RTTYPE>::RTVector3_T(RTTYPE x, RTTYPE y, RTTYPE z)
//{
//	m_data[0] = x;
//	m_data[1] = y;
//	m_data[2] = z;
//}
//template<class RTTYPE> 
//	RTVector3_T<RTTYPE>& RTVector3_T<RTTYPE>::operator =(const RTVector3_T& vec)
//{
//	if (this == &vec)
//		return *this;
//
//	m_data[0] = vec.m_data[0];
//	m_data[1] = vec.m_data[1];
//	m_data[2] = vec.m_data[2];
//
//	return *this;
//}
//
//template<class RTTYPE> 
//	const RTVector3_T<RTTYPE>& RTVector3_T<RTTYPE>::operator +=(RTVector3_T& vec)
//{
//	for (int i = 0; i < 3; i++)
//		m_data[i] += vec.m_data[i];
//	return *this;
//}
//template<class RTTYPE> 
//	const RTVector3_T<RTTYPE>& RTVector3_T<RTTYPE>::operator -=(RTVector3_T& vec)
//{
//	for (int i = 0; i < 3; i++)
//		m_data[i] -= vec.m_data[i];
//	return *this;
//}
//template<class RTTYPE> 
//	void RTVector3_T<RTTYPE>::zero()
//{
//	for (int i = 0; i < 3; i++)
//		m_data[i] = 0;
//}
//
//template<class RTTYPE> 
//	void RTVector3_T<RTTYPE>::print_out() {
//		for (int i = 0; i < 3; i++)
//			std::cout << " " << m_data[i];
//		
//		std::cout << std::endl;
//	}
//
////template<class RTTYPE> 
////void RTVector3_T<RTTYPE>::angle_from_degree(RTVector3_T<double>& vec) {
////	for (int i = 0; i < 3; i++)
////		setData(i, SBGC_DEGREE_TO_ANGLE(vec.data(i)));
////			//vec.setData(i, SBGC_ANGLE_TO_DEGREE(data(i)));
////	}
////
////template<class RTTYPE> 
////void RTVector3_T<RTTYPE>::degree_from_angle(RTVector3_T<int>& vec) {
////	for (int i = 0; i < 3; i++)
////		setData(i, SBGC_ANGLE_TO_DEGREE(vec.data(i)));
////			//vec.setData(i, SBGC_DEGREE_TO_ANGLE(data(i)));
////	}
//
//