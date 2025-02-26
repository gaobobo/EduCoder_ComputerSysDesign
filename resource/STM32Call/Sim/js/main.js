"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.oled_simer = void 0;
var http = require("http");
var url = require("url");
var g_CmdType = {
    invalid: 0,
    reset: 1,
    data: 2,
    showOn: 3,
    showOff: 4
};
var g_lastRecord = {
    cs: 0,
    clk: 0,
    data: 0,
    status: 0,
    x: 0,
    xLow: 0,
    xHigh: 0,
    y: 0,
    posIsUpdated: false,
};
/**
 * @description: 显示器驱动仿真
 * @param {number} clk 时钟信号值
 * @param {number} din 数据信号值
 * @param {number} rst 复位信号值
 * @param {number} dc  数据/命令信号值
 * @param {number} cs  片选信号值
 * @return {string} json格式的字符串，详情见 https://yunfeng-il.coding.net/p/stm32sim/wiki/3
 */
function oled_simer(clk, din, rst, dc, cs) {
    var outStr = {
        device: "oled-display",
        protocol: "Simulating-SPI",
        cmd: g_CmdType.invalid,
        width_max: 128,
        heigth_max: 64,
        dir: 0,
        x: 0,
        y: 0,
        data: [0],
        pattern: "forward",
        lattice: "positive",
        color: "#ffffff"
    };
    var isCmd = false, isData = false;
    if (g_lastRecord.cs == 1 && cs == 0) {
        g_lastRecord.status = 1;
        g_lastRecord.data = 0;
    }
    if (rst == 0) {
        outStr.cmd = g_CmdType.reset;
        g_lastRecord.status = 0;
    }
    else if (g_lastRecord.status == 0) {
        outStr.cmd = g_CmdType.invalid;
    }
    // 命令传输中
    else if (g_lastRecord.status == 1) {
        if (g_lastRecord.clk == 0 && clk == 1) {
            g_lastRecord.data = (g_lastRecord.data << 1) | din;
        }
        if (cs == 1) {
            // 传输结束，取数据
            if (dc == 1) {
                isData = true;
            }
            else {
                isCmd = true;
            }
            g_lastRecord.status = 0;
        }
    }
    else {
        console.log('无效状态', g_lastRecord.status);
    }
    // 命令
    if (isCmd) {
        // 关闭显示
        if (g_lastRecord.data == 0xAE) {
            outStr.cmd = g_CmdType.showOff;
        }
        // 开启显示
        else if (g_lastRecord.data == 0xAF) {
            outStr.cmd = g_CmdType.showOn;
        }
        // 坐标
        else if ((g_lastRecord.data & 0xF0) == 0xB0) {
            g_lastRecord.y = g_lastRecord.data & 0x0F;
            g_lastRecord.y *= 8;
            g_lastRecord.posIsUpdated = true;
        }
        else if ((g_lastRecord.data & 0xF0) == 0) {
            g_lastRecord.xLow = g_lastRecord.data & 0x0F;
            g_lastRecord.x |= g_lastRecord.xLow;
            g_lastRecord.posIsUpdated = true;
        }
        else if ((g_lastRecord.data & 0xF0) == 0x10) {
            g_lastRecord.xHigh = (g_lastRecord.data & 0x0F) << 4;
            g_lastRecord.x = g_lastRecord.xHigh | g_lastRecord.xLow;
            g_lastRecord.posIsUpdated = true;
        }
        // console.log(g_lastRecord.data);
    }
    // 数据
    else if (isData) {
        // 没有更新坐标值，则x自动循环递增
        if (g_lastRecord.posIsUpdated == false) {
            if (++g_lastRecord.x >= outStr.width_max) {
                g_lastRecord.x = 0;
            }
        }
        outStr.data[0] = g_lastRecord.data;
        outStr.cmd = g_CmdType.data;
        outStr.x = g_lastRecord.x;
        outStr.y = g_lastRecord.y;
        // console.log('x=', outStr.x, 'y=', outStr.y);
        g_lastRecord.posIsUpdated = false;
    }
    g_lastRecord.clk = clk;
    g_lastRecord.cs = cs;
    return JSON.stringify(outStr);
}
exports.oled_simer = oled_simer;
/**
 * @description: 创建HTTP服务器，调试专用
 * @param {*} http
 * @return {*}
 */
var server = http.createServer(function (req, res) {
    var outStr = '{ "msg": Hello World }';
    if (req.method === 'GET') {
        var tUrl = url.parse(req.url || "", true);
        var tQuery = Object.assign({}, tUrl.query);
        // 处理GET请求参数
        console.log(tQuery);
        // 处理HTTP请求
        res.statusCode = 200;
        res.setHeader('Content-Type', 'application/json');
        res.end(outStr);
    }
    else if (req.method === 'POST') {
        var body_1 = '';
        req.on('data', function (chunk) {
            body_1 += chunk.toString();
        });
        req.on('end', function () {
            var tUrl = new URL(req.url || '', 'http://localhost');
            var oData = '';
            if (tUrl.pathname === '/') {
                var ioData = JSON.parse(body_1);
                // 处理POST请求参数
                // console.log(ioData);
                oData = oled_simer(ioData.clk, ioData.din, ioData.rst, ioData.dc, ioData.cs);
            }
            else if (tUrl.pathname === '/simp') {
            }
            // 处理HTTP请求
            res.statusCode = 200;
            res.setHeader('Content-Type', 'application/json');
            res.end(oData);
        });
    }
});
// 启动HTTP服务器，调试专用
var port = process.env.PORT || 3000;
server.listen(port, function () {
    console.log("\u670D\u52A1\u5668\u6B63\u5728\u8FD0\u884C\uFF0C\u76D1\u542C\u7AEF\u53E3 ".concat(port));
});
