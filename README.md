This can be treated as the smallest host program for [kylinbot](https://github.com/mobangjack/kylinbot) or more recent project [icra2017rm-mmc-emb](https://github.com/whukylin/icra2017rm-mmc-emb). Which use Robomasters controller (STM32F427IIH6) to control a four mecanum wheel robot and make it do many stuffs, see the mentioned projects or [www.robomasters.com](https://www.robomasters.com/zh-CN) for more information.

This fork deal with the cross-platform serial problem by using the well-known [wjwwood/serial](https://github.com/wjwwood/serial) library, to be more specific, the `windows` branch of [dbolkensteyn/serial](https://github.com/dbolkensteyn/serial) fork, which remove the `catkin` and `python` dependences and still keep the library cmake friendly.

## Step by step guide

We will demonstrate on how to use it on windows platform, and without the dependencies of `catkin` nor `python` etc. And in the Linux/raspberry world, just `cmake ..` then `make` and `make install` is enough. However, in window, the default install path is under `C:/Program Files/`, which will need Administration permission to install and will pollute our files, so we install all cmake build libs into `C:/tools/cmake_install_libs/<LIB_NAME>`, thus we will also make it easier to use it latter.

### Serial library

``` vi
git clone https://github.com/dbolkensteyn/serial.git
cd serial
git checkout windows # checkout the standalone version
mkdir build
cd build
cmake-gui .. 

# And specify the install path to be `C:/tools/cmake_install_libs/serial`, manually!
```

Open the solution, build, configure the `INSTALL`, build again to install.

Note that, the build can be `DEBUG` or `RELEASE`, I am not so sure how to make both.

TEST. Besides the example codes included in the library, we can take a minimal example to demonstrate how to use our built lib via cmake:

``` vi
git clone https://github.com/dbolkensteyn/serial-lib-consumer.git
cd serial-lib-consumer

# And change `find_package(serial REQUIRED)` in `CMakeLists.txt
` into:
# if( WIN32 )
#    include(C:/tools/cmake_install_libs/serial/share/serial/cmake/serialConfig.cmake)
# else()
#    find_package(serial REQUIRED)
# endif()
```

And cmake, build and run.

When plug in Arduino MEGA2560 via USB, the output of `SerialTest.exe` here is:

``` vi
(COM14, Arduino Mega 2560 (COM14), USB\VID_2341&PID_0042&REV_0001)
```


### This project

Just like the example mentioned above, `mkdir build`, `camke(-gui)` and `make` (in Linux) is enough.

And for testing, maybe the easiest way is to connect the `RX` and `TX` pins on the `USB-SERIAL` chip together (a loop) and see the results. Of course, communicate with [kylinbot](https://github.com/mobangjack/kylinbot) is what we want to do.