#include "ReadMpuDev.h"


//class mpuDev

int mpuDev::mpuDevInit(){
	//  Using RTIMULib here allows it to use the .ini file generated by RTIMULibDemo.
	//  Or, you can create the .ini in some other directory by using:
	//      RTIMUSettings *settings = new RTIMUSettings("<directory path>", "RTIMULib");
	//  where <directory path> is the path to where the .ini file is to be loaded/saved

	settings = new RTIMUSettings("RTIMULib");

	imu = RTIMU::createIMU(settings);

	if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
		printf("No IMU found\n");
		return -1;
	}

	//  This is an opportunity to manually override any settings before the call IMUInit

	//  set up IMU

	if (!imu->IMUInit()) { //ʧ��
		cerr << "init imu failed" << endl;
		return -2;
	}

	//  this is a convenient place to change fusion parameters

	imu->setSlerpPower(0.02);
	imu->setGyroEnable(true);
	imu->setAccelEnable(true);
	imu->setCompassEnable(true);


	//init mpuDevyaw
	/*mpuYaw.mpuDevYawInit();*/
	mpuAngle.mpuDevAngleInit();
	return 0;
}



void mpuDev::mpuDevReadForever(){
	int sampleCount = 0;
	int sampleRate = 0;
	uint64_t rateTimer;
	uint64_t displayTimer;
	uint64_t now;

	//  set up for rate timer

	rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();


	//  now just process data

	while (true) {
		//  poll at the rate recommended by the IMU

		usleep(imu->IMUGetPollInterval() * 1000);	//mpuDev ������ 4ms ȡһ�����ݣ�Ҳ���� 250 ��ÿ�룬���ǵĳ���Ҫ��ÿ���� 100 �������ǿ��Դﵽ��

		while (imu->IMURead()) {
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;

			now = RTMath::currentUSecsSinceEpoch();

			//  update 100 times per second Ҳ���Ǽ�� 10*1000 us ��һ��

			if ((now - displayTimer) > 10000) {
				//printf("Sample rate %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
				//cout << imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE << endl;
				//fflush(stdout);

				/*mpuYaw.mpuDevYawBufferUpdate(imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE);*/
				mpuAngle.mpuDevAngleBufferUpdate(RTVector3_T<RTFLOAT>(imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE, imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE, imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE));


				displayTimer = now;
			}

			//  update rate every second

			if ((now - rateTimer) > 1000000) {
				sampleRate = sampleCount;
				sampleCount = 0;
				rateTimer = now;
			}
		}
	}
}


void mpuDev::mpuDevrun(){	//������ѯ������ detach �Ա�֤�᲻ͣ���� fifo���ҿ��Լ���������������
	std::thread t2(&mpuDev::mpuDevReadForever, this);
	t2.detach();
}



//class mpuDevAngle
int mpuDevAngle::mpuDevAngleInit(int _bufferSize /*= 200*/, int _compareSizeStill /*= 100*/, int _compareSizeMoving /* = 10*/) {	//ȷ����ǰλ��ʱʹ�� compareSize = 100���˶�������ʵʱ����ȡ 30 ����
	bufferSize = _bufferSize;
	compareSizeStill = _compareSizeStill;
	compareSizeMoving = _compareSizeMoving;
	//��ʼ��
	mpubuffer.resize(bufferSize);
	return 0;
}

void mpuDevAngle::mpuDevAngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle) {
	if (accessBufferMutex.try_lock()) {	//�õ�������£��������
		mpubuffer.pop_front();
		mpubuffer.push_back(newAngle);
		accessBufferMutex.unlock();
	}
	else {
		cout << "Failed to get the lock when update buffer" << endl;
	}

}
;

inline RTVector3_T<RTFLOAT> mpuDevAngle::CalculateAvg(list<RTVector3_T<RTFLOAT>> &callist)
{
	RTVector3_T<RTFLOAT> avg(0,0,0);
	list<RTVector3_T<RTFLOAT>>::iterator it;
	for (it = callist.begin(); it != callist.end(); it++) {
		avg += *it;
		//cout << " " << *it ;
	}

	//avg /= callist.size();
	avg.setX(avg.x() / callist.size());
	avg.setY(avg.y() / callist.size());
	avg.setZ(avg.z() / callist.size());
	//cout << endl  << callist.size() << endl;
	return avg;
}

//��ǰֵ:�� yaw �������� sort(CompZ())
int mpuDevAngle::mpuDevAnglePresentUpdate(bool moving/* = false*/) {	//moving ָʾ�Ƿ�����������

	int compareSize = moving ? compareSizeMoving : compareSizeStill;

	//tmpbuffer Ϊ�жϵ�ǰλ�õ� buffer������ȡ����Ҫ��С��Ԫ��
	tmpbuffer.clear();
	accessBufferMutex.lock();
	tmpbuffer.assign(mpubuffer.begin(), mpubuffer.end());
	accessBufferMutex.unlock();
	std::list<RTVector3_T<RTFLOAT>>::iterator it1, it2;
	it1 = it2 = tmpbuffer.begin();
	advance(it2, bufferSize - compareSize);
	tmpbuffer.erase(it1, it2);

	//����ȥ����ֵ
	tmpbuffer.sort(CompZ());
	for (int i = 0; i < compareSize / 5; i++) { //ȥ��������С��  ���֮һ �����ݣ��п��ܻ����������
		tmpbuffer.pop_back();
		tmpbuffer.pop_front();
	}

	presentAngle = CalculateAvg(tmpbuffer);
	
	return 0;
}

inline RTFLOAT warp_180(RTFLOAT x){
	return x<-180?x+360:(x>180?x-360:x);
}

void invertAngle(RTVector3_T<RTFLOAT>& angle){
	RTFLOAT r, p, y;
	r = angle.x();
	p = angle.y();
	angle.setX(-warp_180(r+180));
	angle.setY(-warp_180(p));
}

RTVector3_T<RTFLOAT> mpuDevAngle::getAnglePresent(bool upside_down) {
        lock_guard<mutex> lock(accessBufferMutex);	
	if (upside_down){
		invertAngle(presentAngle);
	}
	return presentAngle;
}