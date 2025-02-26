#!/bin/bash

helpInfo(){

    echo \
'
Usage: start.sh <Level> [-f|--force] [-m|--mirror [Gitee|Github]] [-b|--brach [main|dev|<branch>]]
Options:
        <Level>                              Level Code. See at /scripts/ in repo
        -f|--force                           Change judge system kernel to pass if avaliable
        -m|--mirrior [Gitee|Github]          Repo mirror to download resource [default: Gitee]
        -b|--brach [main|dev|<branch>]       Repo branch where download resouce from [default: main]

用法: start.sh <Level> [-f|--force] [-m|--mirror [Gitee|Github]] [-b|--brach [main|dev|<branch>]]
选项:
        <Level>                              关卡代码。详见仓库的/scripts/目录。
        -f|--force                           修改评测系统内核通关。
        -m|--mirrior [Gitee|Github]          下载资源文件时使用的仓库镜像。 [默认: Gitee]
        -b|--brach [main|dev|<branch>]       下载资源文件时的分支。 [默认: main]

============================================
         EduCoder_ComputerSysDesign
        Copyright (c) GaoShibo, 2024
See more at: github.com/gaobobo/EduCoder_ComputerSysDesign

'

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
        wget -O /tmp/download.sh https://gitee.com/coconut_floss/EduCoder_ComputerSysDesign/raw/${Brach}/scripts/download.sh
        ;;
    Github)
        wget -O /tmp/download.sh https://raw.githubusercontent.com/gaobobo/EduCoder_ComputerSysDesign/${Brach}/scripts/download.sh
        ;;
esac

if [ $? -ne 0 ]; then
    echo "Download download.sh failed. Is the network disconnected?"
    echo "下载 download.sh 失败。是否网络不通？"
    exit 1
fi

source /tmp/download.sh ${DownloadSite} ${Brach} scripts/${LevelCode}.sh

if [ $? -ne 0 ]; then
    echo "Download ${LevelCode}.sh failed. Is the LevelCode correct?"
    echo "下载 ${LevelCode}.sh 失败。关卡编号是否正确？"
    exit 1
fi

source /tmp/${LevelCode}.sh ${DownloadSite} ${Brach}

if [ $? -eq 0 ]; then
    echo "脚本运行完成。可直接评测。"
else
    echo "脚本运行失败。请重试。"
fi