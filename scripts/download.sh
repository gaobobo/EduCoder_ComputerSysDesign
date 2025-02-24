#!/bin/bash

if [ -z "$1" ]; then
    exit 0
fi

DownloadSite=${1:-Gitee}
Brach=${2:-main}
FileURL=$3
FileName=$4
FilePath=${5:-"/tmp/${FileName}"}

case $DownloadSite in
    Gitee)
        wget -O ${FilePath} https://gitee.com/coconut_floss/EduCoder_ComputerSysDesign/raw/${Brach}/${FileURL}
        ;;
    Github)
        wget -O ${FilePath} https://raw.githubusercontent.com/gaobobo/EduCoder_ComputerSysDesign/${Brach}/${FileURL}
        ;;
esac

if [ $? -ne 0 ]; then
    echo "Download ${FileName}.sh failed. Is the network disconnected?"
    echo "下载 ${FileName}.sh 失败。是否网络不通？"
    exit 1
fi

exit 0