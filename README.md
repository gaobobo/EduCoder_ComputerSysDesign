> [!IMPORTANT]
>
> 本人该门课程已经结课，故不再更新。

# 头歌STM32单片机开发闯关脚本

该脚本用于通过头歌的单片机实践项目。支持的关卡参见本仓库的 [`/scripts/`](https://github.com/gaobobo/EduCoder_ComputerSysDesign/tree/main/scripts) 或参见 [微信文章](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=Mzg4Njg3NDQ2Ng==&action=getalbum&album_id=3875099464167505928#wechat_redirect) 。

--------

## 快速开始

```shell
# Gitee镜像（中国）：
wget -O /tmp/start.sh https://gitee.com/coconut_floss/EduCoder_ComputerSysDesign/raw/dev/start.sh
# 或使用Github：
wget -O /tmp/start.sh https://raw.githubusercontent.com/gaobobo/EduCoder_ComputerSysDesign/main/start.sh

chmod +x /tmp/start.sh
/tmp/start.sh <Level> [-f|--force] [-m|--mirror [Gitee|Github]] [-b|--Branch [main|dev|<branch>]]

```

## 如何使用

不需要克隆本仓库，只需要下载仓库根目录下的[`start.sh`](https://github.com/gaobobo/EduCoder_ComputerSysDesign/blob/main/start.sh)，该脚本会自动下载所需文件，不会拷贝完整仓库。

之后，运行start.sh。

```
start.sh <Level> [-f|--force] [-m|--mirror [Gitee|Github]] [-b|--branch [main|dev|<branch>]]
```

- `<Level>`：关卡代码。该代码是[`/scripts/`](https://github.com/gaobobo/EduCoder_ComputerSysDesign/tree/main/scripts)中的文件名，不需要加后缀。例如`start.sh STM32Lamp`；
- `-f|--force`：使用`force`模式通关。该模式下会尝试修改评测系统的内核通关。该选项仅作为**备选方案**，因为更改后**除非手动重置环境，否则无法恢复更改**，即使资源被释放也无法恢复。部分情况下常规模式无法通关会自动使用`force`模式；
- `-m|--mirror`：下载时使用的镜像点，默认为`Gitee`；
- `-b|--branch`：下载时仓库的分支，默认为`main`。注意，**`dev`分支并非一个稳定的分支**。

## 贡献项目

该项目运作的原理是先运行`start.sh`，运行后会先下载[`/scripts/download.sh`](https://github.com/gaobobo/EduCoder_ComputerSysDesign/blob/main/scripts/download.sh)。

`download.sh`是一个封装了下载操作的`shell`脚本，其用于管理文件的下载点。通过封装该脚本，可以保证下载操作是从指定的镜像点下载而非第三方的站点，方便进行代码审查和CI/CD。

```
Usage: download.sh <FileURL> <FileName> [options]
Options:
        (  Gitee|Github main|dev|<branch> <FileSavePath>  ) |
        (  Gitee|Github main|dev|<branch>  )                  |
        (  Gitee|Github  )

        Gitee|Github                       Repo mirror to download resource [default: Gitee]
        main|dev|<branch>                  Repo branch where download resouce from [default: main]
        <FileSavePath>                     The file save position

```

如果要打造你自己的项目，你需要修改`download.sh`。

之后将每关的`shell`脚本放入`/scripts/`中，以`.sh`结尾。`start.sh`会寻找`/scripts/$1.sh`（`$1`是第一个参数，即`<Level>`）并下载执行它。

为了目录整洁，在`/resource/`目录中新建一个同名的文件夹并将该脚本使用的相关文件一并放入。

### 编写脚本

每一关的脚本必须接受以下参数：

```
Usage: '$0' [Option]
Options:
       (  Gitee|Github main|dev|<branch> --force  ) |
       (  Gitee|Github main|dev|<branch>  )         |
       (  Gitee|Github  )

       Gitee|Github             Repo mirror to download resource [default: Gitee]
       main|dev|<branch>        Repo branch where download resouce from [default: main]
       --force                  Change judge system kernel to pass if avaliable
```

于此同时，由于执行时使用的是`resouce`且已经设置了错误时的提示信息，因此脚本在出现异常时必须返回值，而非使用`exit`退出。约定：

- `return 0`：成功；
- `return 1`：失败，但是是意料内的错误，比如无法下载；
- `return 2`：失败，但是是一个内部命令错误，比如某命令抛出错误；
- 其他：失败，且是意料外的错误。

建议将下列内容作为模板：

```shell
#!/bin/bash

force_pass() {
    echo "force模式"
}

normal_pass() {
    echo "常规模式"
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
```