#!/bin/sh
exec_dir=$(pwd)
script_dir=$(dirname $0)
bin_dir=.pio/build/genericSTM32F103RB
bin_file=firmware.elf

# 切到编译目录
cd $script_dir
mkdir -p build

# TODO 后续可以加入脚本参数，删除build文件夹后再编译

# 移除上一次编译缓存的elf,bin,hex文件
find "./build" -type f -name "*.elf" -o -name "*.bin" -o -name "*.hex" | while read file; do
    rm $file
done

# 开始编译
make
fw_code_build_result=$?

# 打印并输出编译结果字符串
if [ $fw_code_build_result -eq 0 ]; then
    # 只有成功编译了，才会更新bin文件
    mkdir -p $bin_dir

    # 移除上一次二进制文件存放路径的elf,bin,hex文件
    find "$bin_dir" -type f -name "*.elf" -o -name "*.bin" -o -name "*.hex" | while read file; do
        rm $file
    done

    cp build/*.elf $bin_dir/$bin_file
    echo output to $bin_dir/$bin_file

    echo "YF_SIM_MCU_FW_CODE_BUILD_RESULT__OK"
else
    echo "YF_SIM_MCU_FW_CODE_BUILD_RESULT__FAIL"
fi

# 切回原来目录
cd $exec_dir

# 返回编译结果
exit $fw_code_build_result

