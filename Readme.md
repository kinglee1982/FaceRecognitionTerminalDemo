# 人脸识别终端 Demo

[官网](https://www.suanzi.ai)
[文档地址](https://docs.suanzi.ai)

### 获取项目代码

可以直接[下载](https://github.com/suanzi-ai/FaceRecognitionTerminalDemo/archive/master.zip)。

或者也可以直接克隆方式：

```bash
sudo apt install git git-lfs
git clone https://github.com/suanzi-ai/FaceRecognitionTerminalDemo.git
```

### 编译依赖：

```bash
sudo apt install cmake ninja
```

### 编译

```bash
cd FaceRecognitionTerminalDemo
./build.sh --build-type=Debug --build-dir=$(pwd)/build --ninja
```

### 运行

为了适配中文，项目使用的是免费使用的开源字体：[思源宋体](https://github.com/adobe-fonts/source-han-serif/tree/release/)。

如想使用其他字体，请自行修改 [源码](src/lib/vctext.cpp) 中字体的文件名，然后将字体文件拷贝至运行目录即可。

```bash
ntpd -qNn -p ntp4.aliyun.com # 校准系统时间
./main -display hdmi #输出视频至与开发板连接的 HDMI 显示器
```
