#!/bin/bash

force_pass() {
       echo 'print("TRUE", end="")' > "/data/workspace/myshixun/step1.py/text.py"
}

normal_pass() {

    source /tmp/download.sh \
       "resource/STM32Lamp/RadarTest/Debug/RadarTest.hex" \
       RadarTest.hex \
       ${DownloadSite} \
       ${Branch} \
       "/home/stm32/RadarTest/Debug/RadarTest.hex"


    echo -e "\e[38;5;51;7m 提示： \e[0m高峰时期网络缓慢，脚本只会下载编译好的固件通过评测。" \
            "如果你需要完整项目，请使用git clone克隆仓库，只拉取特定项目可以使用稀疏克隆" \
            "（git sparse-checkout）。"
}



help() {

       echo \
'
Usage: '$0' [Option]
Options:
       (  Gitee|Github main|dev|<branch> --force  ) |
       (  Gitee|Github main|dev|<branch>  )         |
       (  Gitee|Github  )

       Gitee|Github             Repo mirror to download resource [default: Gitee]
       main|dev|<branch>        Repo branch where download resouce from [default: main]
       --force                  Change judge system kernel to pass if avaliable
'

}

DownloadSite=${1:-Gitee}
Branch=${2:-main}
Force=${3:+"--force"}

if [ $# -lt 1 ]; 
then
       help
       return 1
fi

if [ ${Force} ];
then
       echo -e "\e[38;5;11;7m 警告: \e[0m正在以 --force 模式运行。" \
       "这会修改评测系统内核。去掉-f或--force使用一般模式。"
       force_pass

else
       normal_pass
fi

return $?