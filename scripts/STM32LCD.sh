#!/bin/bash

if [ $# -lt 1 ]; then
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

       exit 1
fi


DownloadSite=${1:-Gitee}
Brach=${2:-main}

source /tmp/download.sh \
       ${DownloadSite} \
       ${Brach} \
       resource/STM32LCD/PRO/Debug/PRO.hex \
       /home/stm32/PRO/Debug/PRO.hex