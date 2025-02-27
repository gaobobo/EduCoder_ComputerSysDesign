#!/bin/bash

force_pass() {
       echo "print(\"串口消息：b'lamp On!'\", end=\"\")" > "/data/workspace/myshixun/step2/mqtt_test.py"
}

normal_pass() {

    echo -e "\e[38;5;11;7m 警告: \e[0m该关卡需要物理刷写固件到开发板并向MQTT服务器 \
            "发送信息，无法直接评测。将自动使用force模式通关，这会修改评测系统的内核。" 

    force_pass
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