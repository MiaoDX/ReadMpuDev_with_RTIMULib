# ReadMpuDev_with_RTIMULib

For Raspberry.(Model 2B)

Use [RTIMULib](https://github.com/richards-tech/RTIMULib2.git) 来读取 MPU 数据，进行简单处理后（取最近的几次数据，去除最值，取平均）向外提供获取 roll,pitch,yaw 数据的接口。

由于库函数提供了很好的处理，对 mpu 的型号没有要求，但是一些板子（比如 mpu9250 还是要校准一下才好些）。

建议先看一下库的简单使用，我们的 main 方法就是在 [原有程序 RTIMULib2/Linux/RTIMULibDrive/RTIMULibDrive.cpp](https://github.com/richards-tech/RTIMULib2/blob/e541f972db2bb30c294b99d3d4bd928c61e6bdf5/Linux/RTIMULibDrive/RTIMULibDrive.cpp) 基础上进行更改的。

# Wiring
IMU Vcc <-> raspberry 3.3v(or 5.0v)
IMU GND <-> raspberry GND(Pin 6)
IMU SDA <-> raspberry SDA(Pin 3)
IMU SCL <-> raspberry SCL(Pin 5)

请通电前仔细检查连接无误

# Usage In raspberry

git clone https://github.com/MiaoDX/ReadMpuDev_with_RTIMULib.git
<<<<<<< HEAD

cd ReadMpuDev_with_RTIMULib

mkdir build

cd build

cmake ..

make ..

./testmpuDev


=======
cd ReadMpuDev_with_RTIMULib
mkdir build
cd build
cmake ..
make ..
./testmpuDev

>>>>>>> 2f240a059e8d03439e13b5a6507d9a6549a8330d
# Attention
我目前对 RTIMULib 的理解还是有限，这里只是提供一个简单的使用，并为自己的其他应用提供一个合适的数据接口。
库中有许多东西可以看一看，比如 fusion 算法等。