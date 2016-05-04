/**
change RTIMULibDrive.cpp to provide a simple interface of reading the mpu9250 data
and store it to a fixed length queen,so we can make sure the move of gimbal's yaw is ok 
to stop or not.
*/

#include "RTIMULib.h"
#include <iostream>
#include <list>
#include <mutex>
#include <cmath>
#include <future>

#include "myMath.h"

using namespace std;




//mpu9250Angle 的数据采集与处理，我们暂时并不需要这个，但是提供一个通用的接口是很好的
class mpu9250Angle {
public:
	mpu9250Angle() {}
	;
	~mpu9250Angle() {
		mpubuffer.clear();
	}

	int mpu9250AngleInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可

	void mpu9250AngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle);

	inline RTVector3_T<RTFLOAT> CalculateAvg(list<RTVector3_T<RTFLOAT>> &callist);

	//当前值
	float mpu9250AnglePresentUpdate(bool moving = false);	//moving 指示是否是正在运行,运行时只取较后的几个值，若为静止，可以考虑多一些值

	//判断是否到达目标位置，误差小于 0.1 即当作到达
	//bool mpu9250AngleReachTarget(RTVector3_T<RTFLOAT> targetAngle, float precision = 0.1);

	RTVector3_T<RTFLOAT> getAnglePresent();

private:
	list<RTVector3_T<RTFLOAT>> mpubuffer;
	list<RTVector3_T<RTFLOAT>> tmpbuffer;
	int bufferSize = 200;//我们存储最近的 200 个值进行处理
	int compareSizeStill = 100;//我们取最近的 100 个值来确定当前位置
	int compareSizeMoving = 30;//我们取最近的 30 个值来判断是否到达目标位置

	RTVector3_T<RTFLOAT> presentAngle;
	std::mutex accessBufferMutex;
};


class mpu9250{
public:
	mpu9250(){
	};
	~mpu9250(){
		delete settings;
		delete imu;
	};

	int mpu9250Init();
	
	void mpu9250ReadForever();

	void mpu9250run();

private:
	int sampleCount = 0;
	int sampleRate = 0;
	uint64_t rateTimer;
	uint64_t displayTimer;
	uint64_t now;

	RTIMUSettings *settings;

	RTIMU *imu;
public:
	/*mpu9250Yaw mpuYaw;*/
	mpu9250Angle mpuAngle;
};


#if false
//class mpu9250yaw 的处理，因为直接处理三个值并不会出现明显的效能问题，所以，不再使用
class mpu9250Yaw{
public:
	mpu9250Yaw(){};
	~mpu9250Yaw(){
		mpubuffer.clear();
	}

	int mpu9250YawInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可

	void mpu9250YawBufferUpdate(float newYaw);

	inline float CalculateAvg(list<float> &callist);

	//当前值
	float mpu9250YawPresentUpdate(bool moving = false);	//moving 指示是否是正在运行,运行时只取较后的几个值，若为静止，可以考虑多一些值

	//判断是否到达目标位置，误差小于 0.1 即当作到达
	bool mpu9250YawReachTarget(float targetYaw, float precision = 0.1);

	float getYawPresent();

private:
	list<float> mpubuffer;
	list<float> tmpbuffer;
	int bufferSize = 200;//我们存储最近的 200 个值进行处理
	int compareSizeStill = 100;//我们取最近的 100 个值来确定当前位置
	int compareSizeMoving = 30;//我们取最近的 30 个值来判断是否到达目标位置

	float presentYaw = 0.0;
	std::mutex accessBufferMutex;
};
#endif