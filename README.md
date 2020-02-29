# Video Code

![](https://img.shields.io/badge/build-passing-brightgreen)
![](https://img.shields.io/badge/dependencies-opencv4.2.0-brightgreen)

厦门大学信息学院计算机网络课程项目

作者： [chenxiaoquan233](https://github.com/chenxiaoquan233),[fester](https://github.com/ferster),[jrsmith12138](https://github.com/jrsmith12138),
[firstday1](https://github.com/firstday1)

## 注意

此项目仍在开发中，未发布release版本

<!-- TOC -->
## 目录

- [Video Code](#video-code)
- [注意](#注意)
- [简介](#简介)
- [编译环境](#编译环境)
- [运行环境](#运行环境)
- [使用教程](#使用教程)
  * [编码部分](#编码部分)
  * [解码部分](#解码部分)
- [Todo](#todo)
  * [编码部分](#编码部分)
  * [解码部分](#解码部分)
- [Change log](#change-log)
- [交流反馈](#交流反馈)
<!-- /TOC -->

## 简介
发送方将信息编码为一组信号，并通过可见光发送。 接收方通过手机录制视频的形式接收，并解码得到原始消息。  编码部分为，程序输入消息，生成一系列图像，将图像编码为视频播放在计算机显示器上。 解码部分为手机录制视频再通过 QQ 或 USB 线复制到计算机上，利用解码器解码得到消息。

## 编译环境
Visual Studio 2019  
OpenCV 4.2.0
## 运行环境
Windows10 64位
## 使用教程

### 编码部分
```
encode 输入文件 输出文件 视频时长（单位：毫秒）
```
### 解码部分
```
decode 输入文件 输出文件
```
## Todo
###编码部分
为视频添加起始帧、结束帧
添加第四个定位块
###解码部分
识别视频的起始帧、结束帧
识别第四个定位块，并进行排序

## Change log

## 交流反馈
chenxiaoquan233:  
邮箱: chenxiaoquan233@gmail.com  
QQ: 770355275

fester:  
邮箱: 1014483832@qq.com  
QQ: 1014483832

jrsmith12138:  
邮箱: 849070287@qq.com  
QQ: 849070287

firstday1:  
邮箱: 2602164122@qq.com  
QQ: 2602164122