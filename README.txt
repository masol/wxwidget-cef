一. 编译
1. 执行cmake,按照提示，安装缺失库。sudo apt-get install libgtkglext1-dev
2. 安装cef: 直接从(http://opensource.spotify.com/cefbuilds/index.html#linux64_builds)下载对应包，解压之后
   mkdir build; cd build;cmake -DCMAKE_BUILD_TYPE=Release ..;make -j 8
   然后，将CEF_ROOT环境变量设置在cef目录里。
