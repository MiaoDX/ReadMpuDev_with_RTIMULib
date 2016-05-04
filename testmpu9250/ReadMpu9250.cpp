#include "ReadMpu9250.h"


int mpu9250Yaw::mpu9250YawInit(int _bufferSize /*= 200*/, int _compareSizeStill /*= 100*/, int _compareSizeMoving /* = 10*/){	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可
	bufferSize = _bufferSize;
	compareSizeStill = _compareSizeStill;
	compareSizeMoving = _compareSizeMoving;
	//初始化
	mpubuffer.resize(bufferSize);
}

void mpu9250Yaw::mpu9250YawBufferUpdate(float newYaw){
	if (accessBufferMutex.try_lock()){	//得到锁便更新，否则忽视
		mpubuffer.pop_front();
		mpubuffer.push_back(newYaw);
		accessBufferMutex.unlock();
	}
	else{
		cout << "Failed to get the lock when update buffer" << endl;
	}

};

inline float mpu9250Yaw::CalculateAvg(list<float> &callist)
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
float mpu9250Yaw::mpu9250YawPresentUpdate(bool moving/* = false*/){	//moving 指示是否是正在运行

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
bool mpu9250Yaw::mpu9250YawReachTarget(float targetYaw, float precision/* = 0.1*/){
	//先更新当前值
	mpu9250YawPresentUpdate();

	if (abs(presentYaw - targetYaw) < precision){	//abs in cmath
		cout << "we got present yaw: " << presentYaw << ",target is:" << targetYaw << " ,difference: " << presentYaw - targetYaw << endl;
		return true;
	}
	return false;
}

float mpu9250Yaw::getYawPresent(){
	return presentYaw;
}


//class mpu9250

int mpu9250::mpu9250Init(){
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


	//init mpu9250yaw
	mpuYaw.mpu9250YawInit();
}



void mpu9250::mpu9250ReadForever(){
	int sampleCount = 0;
	int sampleRate = 0;
	uint64_t rateTimer;
	uint64_t displayTimer;
	uint64_t now;

	//  set up for rate timer

	rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();



	//  now just process data

	while (1) {
		//  poll at the rate recommended by the IMU

		usleep(imu->IMUGetPollInterval() * 1000);	//mpu9250 建议是 4ms 取一次数据，也就是 250 次每秒，我们的程序要求每秒钟 100 个数据是可以达到的

		while (imu->IMURead()) {
			RTIMU_DATA imuData = imu->getIMUData();
			sampleCount++;

			now = RTMath::currentUSecsSinceEpoch();

			//  display 100 times per second 也就是间隔 10*1000 us 读一次

			if ((now - displayTimer) > 10000) {
				//printf("Sample rate %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
				//cout << imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE << endl;
				//fflush(stdout);

				mpuYaw.mpu9250YawBufferUpdate(imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE);

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


void mpu9250::mpu9250run(){
	//std::future <void> fut = std::async(mpu9250ReadForever);
	//mpu9250ReadForever();
	//fut.wait();

	//std::future<void> f4 = std::async(std::launch::async, &mpu9250::mpu9250ReadForever, this);
	//f4.wait();

	std::thread t2(&mpu9250::mpu9250ReadForever, this);
	t2.detach();

}