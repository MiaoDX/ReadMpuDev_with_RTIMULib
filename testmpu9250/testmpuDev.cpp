#include <iostream>
#include "ReadMpuDev.h"

using namespace std;


int main_test_RTVec_list()
{
	list<RTVector3_T<RTFLOAT>> livec;
	livec.push_back(RTVector3_T<RTFLOAT>(0, 1, 2));
	livec.push_back(RTVector3_T<RTFLOAT>(1, 0, 5));
	livec.push_back(RTVector3_T<RTFLOAT>(2, -1, 3));

	livec.sort(CompZ());

	for (auto l : livec)
		l.print_out();

	return 0;

}

/****** 何种的 mpu 并不重要，不过，如果是 9250 的话，最好预先进行校准操作 ****/
int main()
{
	mpuDev m9250;
	m9250.mpuDevInit();
	m9250.mpuDevrun();

	sleep(3);
	/*
	m9250.mpuYaw.mpuDevYawPresentUpdate(false);
	cout << m9250.mpuYaw.getYawPresent() << endl;
	*/
	m9250.mpuAngle.mpuDevAnglePresentUpdate(false);
	m9250.mpuAngle.getAnglePresent().print_out();
	
	//float oldyaw = m9250.mpuYaw.getYawPresent();
	float oldyaw = m9250.mpuAngle.getAnglePresent().z();
	float newyaw = 0.0, maxyaw = -180.0, minyaw = 180.0;

	while (true)
	{
		//cout << m9250.mpuYaw.getYawPresent() << endl;
		usleep(100000);
		
		//m9250.mpuYaw.mpuDevYawPresentUpdate(true);

		//newyaw = m9250.mpuYaw.getYawPresent();
		m9250.mpuAngle.mpuDevAnglePresentUpdate(true);
		m9250.mpuAngle.getAnglePresent().print_out();
		newyaw = m9250.mpuAngle.getAnglePresent().z();

		if (maxyaw < newyaw){
			maxyaw = newyaw;
			cout << "get new max/min: " << minyaw << " " << maxyaw << " diff: " << maxyaw - minyaw << endl;
		}
		if (minyaw > newyaw){
			minyaw = newyaw;
			cout << "get new max/min: " << minyaw << " " << maxyaw << " diff: " << maxyaw - minyaw << endl;
		}


		//if (m9250.mpuYaw.mpuDevYawReachTarget(oldyaw + 0.1, 0.05)){
		//	//cout << "before: " << now << " ,preset: " << m9250.mpuYaw.getYawPresent() << " ,相差: " << m9250.mpuYaw.getYawPresent() - now << endl;
		//	cout << "seems ok" << endl;
		//	break;
		//}
	}


	cout << "WON'T BE HERE" << endl;

	return 0;
}