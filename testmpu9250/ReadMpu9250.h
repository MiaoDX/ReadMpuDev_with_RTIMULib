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




//mpu9250Angle �����ݲɼ��봦��������ʱ������Ҫ����������ṩһ��ͨ�õĽӿ��Ǻܺõ�
class mpu9250Angle {
public:
	mpu9250Angle() {}
	;
	~mpu9250Angle() {
		mpubuffer.clear();
	}

	int mpu9250AngleInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//ȷ����ǰλ��ʱʹ�� compareSize = 100���˶�������ʵʱ����ȡ 30 ����

	void mpu9250AngleBufferUpdate(RTVector3_T<RTFLOAT> newAngle);

	inline RTVector3_T<RTFLOAT> CalculateAvg(list<RTVector3_T<RTFLOAT>> &callist);

	//��ǰֵ
	float mpu9250AnglePresentUpdate(bool moving = false);	//moving ָʾ�Ƿ�����������,����ʱֻȡ�Ϻ�ļ���ֵ����Ϊ��ֹ�����Կ��Ƕ�һЩֵ

	//�ж��Ƿ񵽴�Ŀ��λ�ã����С�� 0.1 ����������
	//bool mpu9250AngleReachTarget(RTVector3_T<RTFLOAT> targetAngle, float precision = 0.1);

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
//class mpu9250yaw �Ĵ�����Ϊֱ�Ӵ�������ֵ������������Ե�Ч�����⣬���ԣ�����ʹ��
class mpu9250Yaw{
public:
	mpu9250Yaw(){};
	~mpu9250Yaw(){
		mpubuffer.clear();
	}

	int mpu9250YawInit(int _bufferSize = 200, int _compareSizeStill = 100, int _compareSizeMoving = 10);	//ȷ����ǰλ��ʱʹ�� compareSize = 100���˶�������ʵʱ����ȡ 30 ����

	void mpu9250YawBufferUpdate(float newYaw);

	inline float CalculateAvg(list<float> &callist);

	//��ǰֵ
	float mpu9250YawPresentUpdate(bool moving = false);	//moving ָʾ�Ƿ�����������,����ʱֻȡ�Ϻ�ļ���ֵ����Ϊ��ֹ�����Կ��Ƕ�һЩֵ

	//�ж��Ƿ񵽴�Ŀ��λ�ã����С�� 0.1 ����������
	bool mpu9250YawReachTarget(float targetYaw, float precision = 0.1);

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