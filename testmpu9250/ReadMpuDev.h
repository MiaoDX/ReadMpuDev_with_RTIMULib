/********
miaodx;20160504

Use RTIMULib(https://github.com/richards-tech/RTIMULib2.git) to provide access to mpu9250/6050's data.[It is the library detect which mpu]

使用一个 fifo 来存贮实时的数据，向外提供处理过的数据(getAnglePresent)
更新数据的策略（mpuDevAnglePresentUpdate）：取最近的几个，然后去除最值 -- 通过 yaw 轴读数排序，因为其他两轴几乎很稳定，取平均

USAGE:[需要添加 RTIMULib 和 pthread 库]
1.init
2.mpuDevAnglePresentUpdate
3.getAnglePresent

See testmpuDev.cpp for use example.
**********/


/**
change RTIMULibDrive.cpp to provide a simple interface of reading the mpu9250/6050 data
and store it to a fixed length queen
*/

#include "RTIMULib.h"
#include <iostream>
#include <list>
#include <mutex>
#include <cmath>
#include <future>

//#include "myMath.h"
#include "..\..\myMath.h"

using namespace std;



//mpuDevAngle 的数据采集与处理，提供一个通用的接口
class mpuDevAngle {
public:
	mpuDevAngle() {}
	;
	~mpuDevAngle() {
		mpubuffer.clear();
	}

	int mpuDevAngleInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可

	void mpuDevAngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle);

	inline RTVector3_T<RTFLOAT> CalculateAvg(list<RTVector3_T<RTFLOAT>> &callist);

	//当前值
	int mpuDevAnglePresentUpdate(bool moving = false);	//moving 指示是否是正在运行,运行时只取较后的几个值，若为静止，可以考虑多一些值

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




class mpuDev{
public:
	mpuDev(){
	};
	~mpuDev(){
		delete settings;
		delete imu;
	};

	int mpuDevInit();

	void mpuDevReadForever();	//轮询且更新数据

	void mpuDevrun();

private:
	int sampleCount = 0;
	int sampleRate = 0;
	uint64_t rateTimer;
	uint64_t displayTimer;
	uint64_t now;

	RTIMUSettings *settings;

	RTIMU *imu;
public:
	/*mpuDevYaw mpuYaw;*/
	mpuDevAngle mpuAngle;
};






#if false
//class mpuDevyaw 的处理，因为直接处理三个值并不会出现明显的效能问题，所以，不再使用
class mpuDevYaw{
public:
	mpuDevYaw(){};
	~mpuDevYaw(){
		mpubuffer.clear();
	}

	int mpuDevYawInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//确定当前位置时使用 compareSize = 100，运动过程中实时计算取 30 即可

	void mpuDevYawBufferUpdate(float newYaw);

	inline float CalculateAvg(list<float> &callist);

	//当前值
	int mpuDevYawPresentUpdate(bool moving = false);	//moving 指示是否是正在运行,运行时只取较后的几个值，若为静止，可以考虑多一些值

	//判断是否到达目标位置，误差小于 0.1 即当作到达
	bool mpuDevYawReachTarget(float targetYaw, float precision = 0.1);

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