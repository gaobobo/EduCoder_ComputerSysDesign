import * as oled from './oled_simer'

// 调试监控，单次QEMU输出的最大行数，总输出行数
let g_DgbMaxQemuOutLineCnt = 0;
let g_DbgCmdCnt = 0;

// 模块使能开关
let moduleEn = {
    oled: false
}

interface SdData {
    type: string,
    message: string
}

let sendDatas: string[][] = [];
let g_sdIdx = 0;
let g_sdMaxItems = 10000;
let g_sdCacheWarmMax = 1000;

function addSendData(_data: SdData) {
    if (sendDatas.length > g_sdCacheWarmMax) {
        console.warn('datas over, cur length is', sendDatas.length);
    }

    if (!sendDatas[g_sdIdx]) {
        sendDatas.push([]);
    }

    if (sendDatas[g_sdIdx].length > g_sdMaxItems) {
        let tmp = [ _data.message ];
        sendDatas.push(tmp);
        g_sdIdx = sendDatas.length - 1;
    }
    else {
        sendDatas[g_sdIdx].push(_data.message);
    }
}

function getSendData() {
    let it = sendDatas.shift();
    if (!it || it.length == 0) {
        return it;
    }
    if (g_sdIdx > 0) {
        g_sdIdx--;
    }

    return JSON.stringify(it);
}

function clearSendData() {
    // sendDatas = [];
}

/**
 * @description: oled模块引脚初始化，默认电平为0
 * @return {object}
 */
function oledPinInit() {
    let pins = [
        {
            name: 'clk',
            value: 0,
            pin: 'GPIOB3'
        },
        {
            name: 'din',
            value: 0,
            pin: 'GPIOB5'
        },
        {
            name: 'rst',
            value: 0,
            pin: 'GPIOB4'
        },
        {
            name: 'dc',
            value: 0,
            pin: 'GPIOB6'
        },
        {
            name: 'cs',
            value: 0,
            pin: 'GPIOB7'
        },
    ]
    return pins;
}

// oled模块引脚映射表
let g_oledPinsMap = oledPinInit();

/**
 * @description: oled模块处理
 * @param {string} outs QEMU的单行输出
 * @return {boolean} true表示处理成功，false表示失败
 */
function module_oled(outs: string[]) {
    if (outs.length == 6) {
        if (outs[0] == 'stm32f1_gpio_inner_set_level') {
            g_oledPinsMap.forEach((v) => {
                if ((outs[2] + outs[3]) == v.pin) {
                    v.value = Number(outs[5]);
                }
            });

            // 引脚调试
            // console.log("%d%d%d%d%d", g_oledPinsMap[0].value, g_oledPinsMap[1].value,
            //     g_oledPinsMap[2].value, g_oledPinsMap[3].value, g_oledPinsMap[4].value);
            let modleCmd = oled.simer(g_oledPinsMap[0].value, g_oledPinsMap[1].value,
                g_oledPinsMap[2].value, g_oledPinsMap[3].value, g_oledPinsMap[4].value);
            if (modleCmd.cmd != 0) {
                let tmp = JSON.stringify(modleCmd);
                // console.log(g_DbgCmdCnt++, tmp);
                addSendData({type: 'oled', message: tmp});
            }
            return true;
        }
        return false;
    }
    return false;
}

/**
 * @description: oled模块处理
 * @param {string} outs QEMU的单行输出
 * @return {boolean} true表示处理成功，false表示失败
 */
function module_other(outs: string[]) {
    // TODO 其他新增的模块中间层处理
    // ...

    return false;
}

/**
 * @description: 处理QEMU的输出
 * @param {string} str
 * @return {void}
 */
function outputHandle(str: string) {
    let isHandled = false;
    let out = str.split('\n')
    // 处理输出不带换行
    if (out.length == 0) {
        console.log('single line output', str);
        if (moduleEn.oled && module_oled(str.split(' '))) {
            isHandled = true;
        }
        else if (module_other(str.split(' '))) {
            isHandled = true;
        }
        
        if (isHandled == false) {
            addSendData({type: 'io', message: str})
        }
        return;
    }

    // 处理输出带换行
    out.forEach((ov, oi) => {
        if (ov === '') {
            return;
        }

        let iov = ov.split(' ')
        // demo: stm32f1_gpio_inner_set_level 6068 GPIOB 4 level 1
        // console.log(`${oi} ${iov[2]}${iov[3]}=${iov[5]}`)

        if (moduleEn.oled && module_oled(iov)) {
            isHandled = true;
        }
        else if (module_other(iov)) {
            isHandled = true;
        }
        
        if (isHandled == false) {
            addSendData({type: 'io', message: ov});
        }

        if (oi > g_DgbMaxQemuOutLineCnt) g_DgbMaxQemuOutLineCnt = oi;
    });
}

export {
    outputHandle,
    addSendData,
    getSendData,
    clearSendData
}