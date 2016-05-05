/********
miaodx;20160504

Use RTIMULib(https://github.com/richards-tech/RTIMULib2.git) to provide access to mpu9250/6050's data.[It is the library detect which mpu]

ʹ��һ�� fifo ������ʵʱ�����ݣ������ṩ�����������(getAnglePresent)
�������ݵĲ��ԣ�mpuDevAnglePresentUpdate����ȡ����ļ�����Ȼ��ȥ����ֵ -- ͨ�� yaw �����������Ϊ�������Ἰ�����ȶ���ȡƽ��

USAGE:[��Ҫ��� RTIMULib �� pthread ��]
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



//mpuDevAngle �����ݲɼ��봦���ṩһ��ͨ�õĽӿ�
class mpuDevAngle {
public:
	mpuDevAngle() {}
	;
	~mpuDevAngle() {
		mpubuffer.clear();
	}

	int mpuDevAngleInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//ȷ����ǰλ��ʱʹ�� compareSize = 100���˶�������ʵʱ����ȡ 30 ����

	void mpuDevAngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle);

	inline RTVector3_T<RTFLOAT> CalculateAvg(list<RTVector3_T<RTFLOAT>> &callist);

	//��ǰֵ
	int mpuDevAnglePresentUpdate(bool moving = false);	//moving ָʾ�Ƿ�����������,����ʱֻȡ�Ϻ�ļ���ֵ����Ϊ��ֹ�����Կ��Ƕ�һЩֵ

	RTVector3_T<RTFLOAT> getAnglePresent();

private:
	list<RTVector3_T<RTFLOAT>> mpubuffer;
	list<RTVector3_T<RTFLOAT>> tmpbuffer;
	int bufferSize = 200;//���Ǵ洢����� 200 ��ֵ���д���
	int compareSizeStill = 100;//����ȡ����� 100 ��ֵ��ȷ����ǰλ��
	int compareSizeMoving = 30;//����ȡ����� 30 ��ֵ���ж��Ƿ񵽴�Ŀ��λ��

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

	void mpuDevReadForever();	//��ѯ�Ҹ�������

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
//class mpuDevyaw �Ĵ�����Ϊֱ�Ӵ�������ֵ������������Ե�Ч�����⣬���ԣ�����ʹ��
class mpuDevYaw{
public:
	mpuDevYaw(){};
	~mpuDevYaw(){
		mpubuffer.clear();
	}

	int mpuDevYawInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//ȷ����ǰλ��ʱʹ�� compareSize = 100���˶�������ʵʱ����ȡ 30 ����

	void mpuDevYawBufferUpdate(float newYaw);

	inline float CalculateAvg(list<float> &callist);

	//��ǰֵ
	int mpuDevYawPresentUpdate(bool moving = false);	//moving ָʾ�Ƿ�����������,����ʱֻȡ�Ϻ�ļ���ֵ����Ϊ��ֹ�����Կ��Ƕ�һЩֵ

	//�ж��Ƿ񵽴�Ŀ��λ�ã����С�� 0.1 ����������
	bool mpuDevYawReachTarget(float targetYaw, float precision = 0.1);

	float getYawPresent();

private:
	list<float> mpubuffer;
	list<float> tmpbuffer;
	int bufferSize = 200;//���Ǵ洢����� 200 ��ֵ���д���
	int compareSizeStill = 100;//����ȡ����� 100 ��ֵ��ȷ����ǰλ��
	int compareSizeMoving = 30;//����ȡ����� 30 ��ֵ���ж��Ƿ񵽴�Ŀ��λ��

	float presentYaw = 0.0;
	std::mutex accessBufferMutex;
};
#endif