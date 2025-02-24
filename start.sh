#!/bin/bash

helpInfo(){
    echo "Usage: $0 <Level> [Gitee|Github] [main|dev]"
    echo "                   ~~~~~         ~~~~~~~~~~"
    echo "                   default        branch   "
    echo "Example: $0 STM32LCD"
    echo "用法：$0 <关卡> [Gitee|Github] [main|dev]"
    echo "                 ~~~~~         ~~~~~~~~~~"
    echo "                 默认             分支   "
    echo "============================================"
    echo "       EduCoder_ComputerSysDesign           "
    echo "      Copyright (c) GaoShibo, 2024          "
    echo "See more at: github.com/gaobobo/EduCoder_ComputerSysDesign"
}

if [ -z "$1" ]; then
    helpInfo
    exit 1
fi

LevelCode=$1
DownloadSite=${2:-Gitee}
Brach=${3:-main}

case $DownloadSite in
    Gitee)
        wget -O /tmp/${LevelCode}.sh https://gitee.com/coconut_floss/EduCoder_ComputerSysDesign/raw/${Brach}/scripts/${LevelCode}.sh
        ;;
    Github)
        wget -O /tmp/${LevelCode}.sh https://raw.githubusercontent.com/gaobobo/EduCoder_ComputerSysDesign/${Brach}/scripts/${LevelCode}.sh
        ;;
esac

if [ $? -ne 0 ]; then
    echo "Download ${LevelCode}.sh failed. Is the LevelCode correct?"
    echo "下载 ${LevelCode}.sh 失败。关卡编号是否正确？"
    exit 1
fi

case $DownloadSite in
    Gitee)
        source /tmp/${LevelCode}.sh Gitee
        ;;
    Github)
        source /tmp/${LevelCode}.sh Github
        ;;
esac

if [ $? -eq 0 ]; then
    echo "脚本运行完成。可直接评测。"
else
    echo "脚本运行失败。请重试。"
fi