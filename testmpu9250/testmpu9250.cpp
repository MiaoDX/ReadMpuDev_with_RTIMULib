#include <iostream>
#include "ReadMpu9250.h"
#include "myMath.h"

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


int main()
{
	mpu9250 m9250;
	m9250.mpu9250Init();
	m9250.mpu9250run();

	sleep(3);
	/*
	m9250.mpuYaw.mpu9250YawPresentUpdate(false);
	cout << m9250.mpuYaw.getYawPresent() << endl;
	*/
	m9250.mpuAngle.mpu9250AnglePresentUpdate(false);
	m9250.mpuAngle.getAnglePresent().print_out();
	
	//float oldyaw = m9250.mpuYaw.getYawPresent();
	float oldyaw = m9250.mpuAngle.getAnglePresent().z();
	float newyaw = 0.0, maxyaw = -180.0, minyaw = 180.0;

	while (true)
	{
		//cout << m9250.mpuYaw.getYawPresent() << endl;
		usleep(10000);
		
		//m9250.mpuYaw.mpu9250YawPresentUpdate(true);

		//newyaw = m9250.mpuYaw.getYawPresent();
		m9250.mpuAngle.mpu9250AnglePresentUpdate(true);
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


		//if (m9250.mpuYaw.mpu9250YawReachTarget(oldyaw + 0.1, 0.05)){
		//	//cout << "before: " << now << " ,preset: " << m9250.mpuYaw.getYawPresent() << " ,ПаІо: " << m9250.mpuYaw.getYawPresent() - now << endl;
		//	cout << "seems ok" << endl;
		//	break;
		//}
	}


	cout << "HOPE HERE" << endl;

	return 0;
}