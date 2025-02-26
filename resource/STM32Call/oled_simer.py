#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
import json
import argparse
import os

dataFmt = {
    "device": "oled-display",
    "protocol": "Simulating-SPI",
    "cmd": 0,
    "width_max": 128,
    "heigth_max": 64,
    "dir": 0,
    "x": 0,
    "y": 0,
    "data": [0],
    "pattern": "forward",
    "lattice": "positive",
    "color": "#ffffff"
}

def ReadFile(fpath: str) -> tuple:
    content = ""
    res = True
    try:
        # 打开文件，返回文件对象
        file = open(fpath, 'r')
        # 读取文件内容
        content = file.read()
    except FileNotFoundError:
        print("文件不存在！")
        res = False
    except IOError:
        print("文件操作错误！")
        res = False
    finally:
        # 关闭文件
        if (res):
            file.close()
    return res, content

def WriteFile(fpath: str, content: str) -> bool:
    res = True
    try:
        # 打开文件，返回文件对象
        file = open(fpath, 'w')
        file.write(content)
    except FileNotFoundError:
        print("文件不存在！")
        res = False
    except IOError:
        print("文件操作错误！")
        res = False
    finally:
        # 关闭文件
        if (res):
            file.close()
    return res


def OledSimer(clk: str, din: str, res: str, dc: str, cs: str) -> str:
    lastRecord = {
        "lastCs": '0',
        "lastClk": '0',
        "data": '0',
        "status": '0',
        "x": 0,
        "y": 0
    }
    
    if (res == '0'):
        dataFmt['cmd'] = 1
        lastRecord['status'] = '0'
        if (os.path.exists('./last_record.txt')):
            os.remove('./last_record.txt')

    isOk, content = ReadFile('./last_record.txt')
    if (isOk):
        lastRecord = json.loads(content)

    isCmd = False
    isData = False
    # 0: 待机
    # 1: 数据传输中
    # 2: 传输结束，取数据
    
    if (lastRecord['lastCs'] == '1' and cs == '0'):
        lastRecord['status'] = '1'
        lastRecord['data'] = '0'

    if (res == '0'):
        dataFmt['cmd'] = 1
        lastRecord['status'] = '0'
    elif (lastRecord['status'] == '0'):
        dataFmt['cmd'] = 0
    elif (lastRecord['status'] == '1'): # 命令传输中
        if (lastRecord['lastClk'] == '0' and clk == '1'):
            lastRecord['data'] = str(int(lastRecord['data']) << 1 | int(din))
        if (cs == '1'): # 传输结束，取数据
            if (dc == '0'):
                # 数据
                isData = True
            else:
                # 命令
                isCmd = True
            lastRecord['status'] = '0'
    else:
        print('无效状态', lastRecord['status'])

    if (isCmd):
        print('处理命令和坐标', bin(int(lastRecord['data'])))
    elif (isData):
        dataFmt['data'][0] = lastRecord['data']
        dataFmt['cmd'] = 2
    
    lastRecord['lastClk'] = clk
    lastRecord['lastCs'] = cs
    WriteFile('./last_record.txt', json.dumps(lastRecord))

    return json.dumps(dataFmt)

def test():
    OledSimer('0', '1', '0', '1', '1')

    OledSimer('0', '1', '1', '1', '1')
    OledSimer('0', '1', '1', '1', '0')

    OledSimer('0', '0', '1', '1', '0')
    OledSimer('0', '1', '1', '1', '0')
    OledSimer('1', '1', '1', '1', '0')

    OledSimer('0', '1', '1', '1', '0')
    OledSimer('0', '0', '1', '1', '0')
    OledSimer('1', '0', '1', '1', '0')

    OledSimer('1', '0', '1', '1', '1')

# * 脚本执行入口
if __name__ == '__main__':
    exitRes = False
    parser = argparse.ArgumentParser(description='oled drive pin analysis')
    parser.add_argument('--clk', '-c', help='CLK/D0 pin value', required=True)
    parser.add_argument('--din', help='DIN/D1 pin value', required=True)
    parser.add_argument('--res', help='RES pin value', required=True)
    parser.add_argument('--dc', help='DC pin value', required=True)
    parser.add_argument('--cs', help='CS pin value', required=True)
    
    # test()
    args = parser.parse_args()

    outStr = OledSimer(args.clk, args.din,
                       args.res, args.dc, args.cs)
    outJson = json.loads(outStr)
    if (outJson['cmd'] != 0):
        print(outStr)

    sys.exit(not exitRes)
