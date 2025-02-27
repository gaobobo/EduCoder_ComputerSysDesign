#!/bin/bash

helpInfo(){

    echo \
'
Usage: '$0' <Level> [-f|--force] [-m|--mirror [Gitee|Github]] [-b|--brach [main|dev|<branch>]]
Options:
        <Level>                              Level Code. See at /scripts/ in repo
        -f|--force                           Change judge system kernel to pass if avaliable
        -m|--mirrior [Gitee|Github]          Repo mirror to download resource [default: Gitee]
        -b|--brach [main|dev|<branch>]       Repo branch where download resouce from [default: main]

用法: '$0' <Level> [-f|--force] [-m|--mirror [Gitee|Github]] [-b|--brach [main|dev|<branch>]]
选项:
        <Level>                              关卡代码。详见仓库的/scripts/目录。
        -f|--force                           如可用，修改评测系统内核通关。
        -m|--mirrior [Gitee|Github]          下载资源文件时使用的仓库镜像。 [默认: Gitee]
        -b|--brach [main|dev|<branch>]       下载资源文件时的分支。 [默认: main]

============================================
         EduCoder_ComputerSysDesign
        Copyright (c) GaoShibo, 2024
See more at: github.com/gaobobo/EduCoder_ComputerSysDesign
'

}


# exit if no params
if [ -z "$1" ]; then
    helpInfo
    exit 0
fi



LevelCode=${1}
DownloadSite="Gitee"
Brach="main"
Force=""

# save params
for i in "$@"; do
    params_num=${#params[*]}
    ((params_num++))
    params[$params_num]=$i
done

# recongnize params
i=2
while [ ${i} -le "${#params[*]}" ]
do

    if [ "${params[$i]}" = "-f" ] || [ "${params[$i]}" = "--force" ]
    then 

        ((i++))
        Force="--force"

    elif [ "${params[$i]}" = "-m" ] || [ "${params[$i]}" = "--mirror" ]
    then 

        ((i++))

        case "${params[$i]}" in

            "Gitee") DownloadSite="Gitee" ;;
            "Github") DownloadSite="Github" ;;
            *) 
                echo -e "\e[38;5;9;7m Error: \e[0mUnknow download mirror."
                echo -e "\e[38;5;9;7m 错误: \e[0m未知的镜像下载点。"
                helpInfo 1
            ;;
        esac

    elif [ "${params[$i]}" = "-b" ] || [ "${params[$i]}" = "--branch" ]
    then 

        ((i++))
        Brach="${params[$i]}"

    else

        echo -e "\e[38;5;9;7m Error: \e[0mUnknow option: ${params[$i]}"
        echo -e "\e[38;5;9;7m 错误: \e[0m未知的选项：${params[$i]}"
        helpInfo 1

    fi

done

case $DownloadSite in
    Gitee)
        wget -O /tmp/download.sh https://gitee.com/coconut_floss/EduCoder_ComputerSysDesign/raw/${Brach}/scripts/download.sh
        ;;
    Github)
        wget -O /tmp/download.sh https://raw.githubusercontent.com/gaobobo/EduCoder_ComputerSysDesign/${Brach}/scripts/download.sh
        ;;
esac

if [ $? -ne 0 ]; then
    echo -e "\e[38;5;9;7m Error: \e[0mDownload download.sh failed. Is the network disconnected?"
    echo -e "\e[38;5;9;7m 错误: \e[0m下载 download.sh 失败。是否网络不通？"
    exit 1
fi

source /tmp/download.sh scripts/${LevelCode}.sh ${LevelCode}.sh ${DownloadSite} ${Brach}

source /tmp/${LevelCode}.sh ${DownloadSite} ${Brach} ${Force}

if [ $? -eq 0 ]; then
    echo -e "\e[38;5;10;7m 完成: \e[0m脚本运行完成。可直接评测。"
elif [ $? -eq 1 ]; then
    echo -e "\e[38;5;9;7m 意料内错误: \e[0m脚本运行失败。请重试。"
elif [ $? -eq 2 ]; then
    echo -e "\e[38;5;9;7m 未知的内部错误: \e[0m脚本运行时报告了内部命令错误。如果反复出现可能是脚本问题。" \
            "请至\e[4mgithub.com/gaobobo/EduCoder_ComputerSysDesign\e[0m反馈问题。"
else
    echo -e "\e[38;5;9;7m 意料外错误: \e[0m脚本运行失败。请重试。"
fi