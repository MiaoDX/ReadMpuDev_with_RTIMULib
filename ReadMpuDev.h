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

#include "myMath.h"

using namespace std;



//mpuDevAngle 的数据采集与处理，提供一个通用的接口
class mpuDevAngle {
public:
	mpuDevAngle() {}
	;
	~mpuDevAngle() {
		mpubuffer.clear();
	}

	int mpuDevAngleInit(int _bufferSize = 20);

	void mpuDevAngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle);

	inline RTVector3_T<RTFLOAT> CalculateAvg(list<RTVector3_T<RTFLOAT>> &callist);

	int mpuDevAnglePresentUpdate();

	RTVector3_T<RTFLOAT> getAnglePresent();

private:
	list<RTVector3_T<RTFLOAT>> mpubuffer;
	list<RTVector3_T<RTFLOAT>> tmpbuffer;
	int bufferSize = 20;//我们存储最近的 20 个值进行处理

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
	mpuDevAngle mpuAngle;
};
