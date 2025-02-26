import * as child_process from 'child_process';
import * as qemu_analysis from './qemu_analysis';
import * as http from 'http';
import * as url from 'url';
import * as WebSocket from 'ws';

let cmd = './qemu-system-arm';
let args = ['-machine', 'yunfeng-es1', '-kernel', './firmware.elf', '-nographic',
    '-trace', 'events=events.txt'];
let g_child: child_process.ChildProcessWithoutNullStreams | undefined = undefined;
let g_webSocket: WebSocket.WebSocket | undefined = undefined;

// 启动websocket
let webSerPort = 8080;
let webSocketServer = new WebSocket.Server({ port: webSerPort });
webSocketServer.on('connection', (socket) => {
    console.log('Client connected', webSocketServer.clients.size);
    socket.on('message', (message) => {
        console.log(`Received message: ${message}`);
    });

    socket.on('close', () => {
        console.log('Client disconnected');
        g_webSocket = undefined;
    });
    g_webSocket = socket;
});

setInterval(() => {
    if (g_webSocket) {
        let msg = qemu_analysis.getSendData();
        if (msg) {
            g_webSocket.send(msg, (err) => {
                if (err) console.log(err);
            });
            qemu_analysis.clearSendData();
        }
    }
}, 100);

/**
 * @description: 以新建进程的方式启动QEMU，并捕获控制台输出
 * @param {string} _cmd QEMU执行路径和文件名
 * @param {string} _args 执行的参数
 * @return {child_process.ChildProcessWithoutNullStreams} 进程变量
 */
function qemuStart(_cmd: string, _args: string[]) {
    // child_process.execSync('pkill qemu-system-arm');

    let child = child_process.spawn(cmd, args);

    // 实时监控标准输出和标准错误输出
    child.stdout.on('data', (data: object) => {
        qemu_analysis.outputHandle(String(data));
    });
    child.stderr.on('data', (data: object) => {
        qemu_analysis.outputHandle(String(data));
    });

    return child;
}

/**
 * @description: 关闭QEMU进程
 * @param {child_process.ChildProcessWithoutNullStreams} _process 进程变量
 * @return {boolean}
 */
function qemuStop(_process: child_process.ChildProcessWithoutNullStreams) {
    _process.stdin.write('q\n');
    if (_process.killed == false) {
        _process.kill();
    }
    return _process.killed;
}


let server = http.createServer((req, res) => {
    let reqUrl = url.parse(req.url || '', true);
    if (reqUrl.pathname === '/simulation/start-all') {
        console.log('start');
        g_child = qemuStart(cmd, args);
    }
    else if (reqUrl.pathname === '/simulation/stop-all') {
        if (g_child) {
            console.log('stop');
            qemuStop(g_child);
            g_child = undefined;
        }
        // if (g_webSocket) {
        //     g_webSocket.close();
        //     g_webSocket = undefined;
        // }
    }
    let query = reqUrl.query;
    let callback = query.callback;
    let data = { code: 0, message: '' };
    let json = JSON.stringify(data);
    let script = `${callback}(${json})`;
    res.writeHead(200, { 'Content-Type': 'application/javascript' });
    res.write(script);
    res.end();
});

let httpSerPort = 8081;
server.listen(httpSerPort, () => {
  console.log(`HTTP server is listening on port ${httpSerPort}, WebServer is listening on port ${webSerPort}`);
});