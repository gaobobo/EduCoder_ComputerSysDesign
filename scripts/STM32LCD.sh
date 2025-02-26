#!/bin/bash

DownloadSite=${1:-Gitee}
Brach=${2:-main}

source /tmp/download.sh \
       ${DownloadSite} \
       ${Brach} \
       resource/STM32LCD/PRO/Debug/PRO.hex \
       /home/stm32/PRO/Debug/PRO.hex