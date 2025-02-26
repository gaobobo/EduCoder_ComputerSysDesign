const child_process = require('node:child_process');
const readline = require('node:readline');
const events = require('node:events');
const net = require('node:net');
const process = require('node:process');

/*!
class Stm32f1GPIO extends events.EventEmitter {
    constructor() {
        super();
        this.eventNames = ['GPIOA', 'GPIOB', 'GPIOC', 'GPIOD', 'GPIOE', 'GPIOF', 'GPIOG'];
        this.event = new events.EventEmitter;
    }

    handle = (data) => {
        console.log(`qemu-stm32f1: ${data}`);
        // this.event.emit('', 'level', level);
        // this.event.emit('', 'mode', mode);
    }
}
*/

class QemuStm32f1 extends events.EventEmitter {
    constructor() {
        super();
        this.regexp = '^stm32f1';
        this.eventName = 'qemu-stm32f1';
    }

    handle = (data) => {
        // console.log(`qemu-stm32f1: ${data}`);
        // data = data.split(' ');
        // this.emit(data[1], data);
        data = { type: 'stm32f1', message: data };
        this.emit('data', data);
    }
}

class QemuSystem extends events.EventEmitter {
    constructor() {
        super();
        this.regexp = '^qemu-system';
        this.eventName = 'qemu-system';
        this.communications = {};
        this.on('-monitor', this.communication);
        this.on('-qmp', this.communication);
        this.on('-serial', this.communication);
    }

    communication = (data) => {
        let sockName = data[2].split(':')[1].split(',')[0];
        let commName = sockName.split('.')[0];

        let sock = net.connect(sockName);
        sock.on('connect', () => {
            // console.log(`${commName} connected`);
            this.communications[commName] = sock;
        });
        sock.on('data', (data) => {
            // console.log(`${commName}: ${data}`);
            data = { type: commName, message: data };
            this.emit('data', data);
        });
        sock.on('end', () => {
            // console.log(`${commName} disconnect`);
            delete this.communications[commName];
        });
    }

    write = (data) => {
        // console.log(`${data['type']}: ${data}`);
        this.communications[data['type']].write(data['message']);
    }

    handle = (data) => {
        // console.log(`qemu-system: ${data}`);
        data = data.split(' ');
        this.emit(data[1], data);
    }
}

class Qemu extends events.EventEmitter {
    constructor(cfg) {
        super();
        this.command = '';
        this.options = [];
        this.config(cfg);
        this.table = {};
        this.machine = new QemuStm32f1;
        this.system = new QemuSystem;
        this.register(this.machine);
        this.register(this.system);
    }

    config = (cfg) => {
        this.command = cfg['path'];
        this.options.push('-machine');
        this.options.push(cfg['machine']);
        this.options.push('-kernel');
        this.options.push(cfg['firmware']);
        this.options.push('-icount');
        this.options.push('shift=auto,align=off,sleep=on');
        this.options.push('-trace');
        this.options.push(`events=${cfg['events']}`);
        this.options.push('-nographic');
        if (cfg['monitor']) {
            this.options.push('-monitor');
            this.options.push('unix:monitor.sock,server=on,wait=on');
        } else {
            this.options.push('-monitor');
            this.options.push('none');
        }
        if (cfg['qmp']) {
            this.options.push('-qmp');
            this.options.push('unix:qmp.sock,server=on,wait=on');
        }
        for (let i = 0; i < cfg['serial']; i ++) {
            this.options.push('-serial');
            this.options.push(`unix:serial${i+1}.sock,server=on,wait=on`);
        }
        if (cfg['debug']) {
            this.options.push('-gdb');
            this.options.push('tcp::1234');
            this.options.push('-s');
        }
        console.log(this.command);
        console.log(this.options);
    }

    register = (obj) => {
        this.table[obj.regexp] = obj.eventName;
        this.on(obj.eventName, obj.handle);
        obj.on('data', (data) => {
            this.emit('data', data);
        });
    }

    write = (data) => {
        this.system.write(data);
    }

    start = () => {
        const qemuProcess = child_process.spawn(this.command, this.options);
        qemuProcess.on('close', (code) => {
            console.log(`qemu process exit ${code}`);
            this.emit('close', code);
        });

        const qemuOutput = readline.createInterface(qemuProcess.stderr);
        qemuOutput.on('line', (input) => {
            // console.log(`${input}`);
            for (const [regexp, eventName] of Object.entries(this.table)) {
                if (input.match(regexp)) {
                    this.emit(eventName, input);
                    break;
                }
            }
        });
    }
}

function main() {
    const cfg = {
        path: '/home/ubuntu/Work/qemu/qemu-7.1.0/build/arm-softmmu/qemu-system-arm',
        machine: 'yunfeng-es1',
        firmware: '/home/ubuntu/Work/STM32F103RB_Blink/.pio/build/genericSTM32F103RB/firmware.elf',
        events: '/home/ubuntu/Work/qemu/qemu-7.1.0/build/arm-softmmu/stm32f1_events.txt',
        serial: 3,
        qmp: true,
        monitor: true,
        debug: false,
    };

    const qemu = new Qemu(cfg);
    qemu.start();
    qemu.on('data', (data) => {
        // console.log(data);
    });

    const stdin = readline.createInterface(process.stdin);
    stdin.on('line', (input) => {
        qemu.write({ type: 'monitor', message: `${input}\n` });
    });

    qemu.on('close', (code) => {
        stdin.close();
    });
}

main();