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
		exit(1);
	}

	//  This is an opportunity to manually override any settings before the call IMUInit

	//  set up IMU

	imu->IMUInit();

	//  this is a convenient place to change fusion parameters

	imu->setSlerpPower(0.02);
	imu->setGyroEnable(true);
	imu->setAccelEnable(true);
	imu->setCompassEnable(true);


	//init mpuDevyaw
	/*mpuYaw.mpuDevYawInit();*/
	mpuAngle.mpuDevAngleInit();
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

		usleep(imu->IMUGetPollInterval() * 1000);	//mpuDev 建议是 4ms 取一次数据，也就是 250 次每秒，我们的程序要求每秒钟 100 个数据是可以达到的

		while (imu->IMURead()) {
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;

			now = RTMath::currentUSecsSinceEpoch();

			//  update 100 times per second 也就是间隔 10*1000 us 读一次

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


void mpuDev::mpuDevrun(){	//开启轮询，将其 detach 以保证会不停更新 fifo，且可以继续进行其他操作
	std::thread t2(&mpuDev::mpuDevReadForever, this);
	t2.detach();
}



//class mpuDevAngle
int mpuDevAngle::mpuDevAngleInit(int _bufferSize /*= 200*/, int _compareSizeStill /*= 100*/, int _compareSizeMoving /* = 10*/) {	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可
	bufferSize = _bufferSize;
	compareSizeStill = _compareSizeStill;
	compareSizeMoving = _compareSizeMoving;
	//初始化
	mpubuffer.resize(bufferSize);
}

void mpuDevAngle::mpuDevAngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle) {
	if (accessBufferMutex.try_lock()) {	//得到锁便更新，否则忽视
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

//当前值:按 yaw 进行排序， sort(CompZ())
float mpuDevAngle::mpuDevAnglePresentUpdate(bool moving/* = false*/) {	//moving 指示是否是正在运行

	int compareSize = moving ? compareSizeMoving : compareSizeStill;

	//tmpbuffer 为判断当前位置的 buffer，首先取出需要大小的元组
	tmpbuffer.clear();
	accessBufferMutex.lock();
	tmpbuffer = mpubuffer;
	accessBufferMutex.unlock();
	std::list<RTVector3_T<RTFLOAT>>::iterator it1, it2;
	it1 = it2 = tmpbuffer.begin();
	advance(it2, bufferSize - compareSize);
	tmpbuffer.erase(it1, it2);

	//排序，去除最值
	tmpbuffer.sort(CompZ());
	for (int i = 0; i < compareSize / 5; i++) { //去除最大和最小的  五分之一 个数据（有可能会出现噪声）
		tmpbuffer.pop_back();
		tmpbuffer.pop_front();
	}

	presentAngle = CalculateAvg(tmpbuffer);
}


///***** Not implement ****/
////判断是否到达目标位置，误差小于 0.1 即当作到达
//bool mpuDevAngle::mpuDevAngleReachTarget(RTVector3_T<RTFLOAT> targetAngle, float precision/* = 0.1*/) {
	////先更新当前值
	//mpuDevAnglePresentUpdate();
//
	////if (abs(presentYaw - targetYaw) < precision) {	//abs in cmath
		////cout << "we got present yaw: " << presentYaw << ",target is:" << targetYaw << " ,difference: " << presentYaw - targetYaw << endl;
		////return true;
	////}
	//return false;
//}

RTVector3_T<RTFLOAT> mpuDevAngle::getAnglePresent() {
	return presentAngle;
}





#if false
int mpuDevYaw::mpuDevYawInit(int _bufferSize /*= 200*/, int _compareSizeStill /*= 100*/, int _compareSizeMoving /* = 10*/){	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可
	bufferSize = _bufferSize;
	compareSizeStill = _compareSizeStill;
	compareSizeMoving = _compareSizeMoving;
	//初始化
	mpubuffer.resize(bufferSize);
}

void mpuDevYaw::mpuDevYawBufferUpdate(float newYaw){
	if (accessBufferMutex.try_lock()){	//得到锁便更新，否则忽视
		mpubuffer.pop_front();
		mpubuffer.push_back(newYaw);
		accessBufferMutex.unlock();
	}
	else{
		cout << "Failed to get the lock when update buffer" << endl;
	}

};

inline float mpuDevYaw::CalculateAvg(list<float> &callist)
{
	float avg = 0;
	list<float>::iterator it;
	for (it = callist.begin(); it != callist.end(); it++) {
		avg += *it;
		//cout << " " << *it ;
	}

	avg /= callist.size();
	//cout << endl  << callist.size() << endl;
	return avg;
}

//当前值
float mpuDevYaw::mpuDevYawPresentUpdate(bool moving/* = false*/){	//moving 指示是否是正在运行

	int compareSize = moving ? compareSizeMoving : compareSizeStill;

	//tmpbuffer 为判断当前位置的 buffer，首先取出需要大小的元组
	tmpbuffer.clear();
	accessBufferMutex.lock();
	tmpbuffer = mpubuffer;
	accessBufferMutex.unlock();
	std::list<float>::iterator it1, it2;
	it1 = it2 = tmpbuffer.begin();
	advance(it2, bufferSize - compareSize);
	tmpbuffer.erase(it1, it2);

	//排序，去除最值
	tmpbuffer.sort();
	for (int i = 0; i < compareSize / 5; i++){ //去除最大和最小的  五分之一 个数据（有可能会出现噪声）
		tmpbuffer.pop_back();
		tmpbuffer.pop_front();
	}

	presentYaw = CalculateAvg(tmpbuffer);
}

//判断是否到达目标位置，误差小于 0.1 即当作到达
bool mpuDevYaw::mpuDevYawReachTarget(float targetYaw, float precision/* = 0.1*/){
	//先更新当前值
	mpuDevYawPresentUpdate();

	if (abs(presentYaw - targetYaw) < precision){	//abs in cmath
		cout << "we got present yaw: " << presentYaw << ",target is:" << targetYaw << " ,difference: " << presentYaw - targetYaw << endl;
		return true;
	}
	return false;
}

float mpuDevYaw::getYawPresent(){
	return presentYaw;
}
#endif