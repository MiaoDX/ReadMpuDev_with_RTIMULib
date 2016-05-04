# ReadMpuDev_with_RTIMULib

Use [RTIMULib](https://github.com/richards-tech/RTIMULib2.git) 来读取 MPU 数据，进行简单处理后（取最近的几次数据，去除最值，取平均）向外提供获取 roll,pitch,yaw 数据的接口。

由于库函数提供了很好的处理，对 mpu 的型号没有要求，但是一些板子（比如 mpu9250 还是要校准一下才好些）。

建议先看一下库的简单使用，我们的 main 方法就是在 [原有程序 RTIMULib2/Linux/RTIMULibDrive/RTIMULibDrive.cpp](https://github.com/richards-tech/RTIMULib2/blob/e541f972db2bb30c294b99d3d4bd928c61e6bdf5/Linux/RTIMULibDrive/RTIMULibDrive.cpp) 基础上进行更改的。
