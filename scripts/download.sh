#!/bin/bash

if [ $# -lt 2 ]; then
    echo \
'
Usage: '$0' <FileURL> <FileName> [options]
Options:
        (  Gitee|Github main|dev|<branch> <FileSavePath>  ) |
        (  Gitee|Github main|dev|<branch>  )                  |
        (  Gitee|Github  )

        Gitee|Github                       Repo mirror to download resource [default: Gitee]
        main|dev|<branch>       Repo branch where download resouce from [default: main]
        <FileSavePath>                       The file save position
'
    return 1
fi

FileURL=${1}
FileName=${2}
DownloadSite=${3:-"Gitee"}
Branch=${4:-"main"}
FilePath=${5:-"/tmp/${FileName}"}

mkdir -p "$(dirname "${FilePath}")"

case $DownloadSite in
    Gitee)
        wget -O ${FilePath} https://gitee.com/coconut_floss/EduCoder_ComputerSysDesign/raw/${Branch}/${FileURL}
        ;;
    Github)
        wget -O ${FilePath} https://raw.githubusercontent.com/gaobobo/EduCoder_ComputerSysDesign/${Branch}/${FileURL}
        ;;
esac

if [ $? -ne 0 ]; then
    echo -e "\e[38;5;9;7m Error: \e[0mDownload ${FileName} failed. Is the network disconnected or wrong LevelCode?"
    echo -e "\e[38;5;9;7m 错误: \e[0m下载 ${FileName} 失败。是否网络不通或错误的关卡代码？"
    return 1
fi

return 0