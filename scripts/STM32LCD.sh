#!/bin/bash

force_pass() {
       echo \
'
#!/bin/bash

echo "开始评测..."
echo "PRO.hex已生成，编译成功！"
echo "PRO.hex文件可下载：通关成功！"
echo -n "请下载PRO.hex文件至本地开发板验证！"
' >> /data/workspace/myshixun/main.sh
}

normal_pass() {
       source /tmp/download.sh \
       resource/STM32LCD/PRO/Debug/PRO.hex \
       PRO.hex \
       ${DownloadSite} \
       ${Branch} \
       "/home/stm32/PRO/Debug/PRO.hex"
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