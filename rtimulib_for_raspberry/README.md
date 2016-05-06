# 注：请忽视这个文件，以及  rtimulib_for_raspberry 中的 cmake .
# 在我写了下面那么多之后，突然发现是在之前的实验中已经在 raspberry 下安装了 RTIMULib 库了，连接的并不是我自己提供的。但是整个想法应该是可以考虑的，毕竟人家 opencv 都可以不单独编译库的么。


# 今后在添加修改吧！







# 预编译的 RTIMULib.lib 和 RTIMULib.so

有说 .lib 是对应 windows 平台，.so 是对应于 linux 平台，但是至少在我这是两者任一都可用的。

# [RTIMULib.so 编译方法](https://github.com/richards-tech/RTIMULib2/blob/master/Linux/README.md)

```
进入  RTIMULib 目录

mkdir build
cd build
cmake ..
make -j4
~~sudo make install~~
~~sudo ldconfig~~

```

直接生成的是 libRTIMULib.so.8.0.0 文件，我们将其 拷贝更名为 RTIMULib.so（与 windows 下生成的  RTIMULib.lib 保持一致）

# [RTIMULib.lib 编译方法](https://github.com/richards-tech/RTIMULib2/tree/master/RTHost) 下对 windows 平台的说明（需要安装  cmake）

```
进入  RTIMULib 目录

mkdir build
cd build
cmake -DQT5=1 ..

```

vs 打开生成的工程，build 便可在 Debug 文件夹下生成我们需要的 RTIMULib.lib

# 使用
我们这里是把 .lib 和 .so 都放进了根目录下的 "rtimulib for raspberry" 目录中，并且在 "testmpu9250/cmake_with_pre_compiled_lib/" 进行使用。可以看一下 CMakeLists.txt ,应该是比较直观的。

# .lib 的原因
因为我们是通过 vs + visualgdb 来进行 raspberry 下的 linux 的编程的。目前还没有完全放弃在 windows 下编程的准备。

# 静态库的原因
直接引用生成的库而不是直接编译源代码文件，其实就像是直接引用 opencv 库而不是每次都直接编译它一样 -- 避免不必要的东西出现在工程中。

