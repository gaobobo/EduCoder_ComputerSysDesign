import * as child_process from 'node:child_process'
import * as readline from 'node:readline'
import * as events from 'node:events'
import * as net from 'node:net'

type QemuConfig = {
    qemuPath: string,
    machineName: string,
    socName: string,
    firmwarePath: string,
    eventsPath: string,
    serialNumber: number,
    hasQmp: boolean,
    hasMonitor: boolean,
    hasDebug: boolean,
}

type QemuData = {
    type: string,
    message: string,
}

type MachineData = {
    vtime: string,
    type: string,
    value: string,
}

abstract class QemuDistribute extends events.EventEmitter {
    readonly regexp: string;
    readonly eventName: string;

    constructor(regexp: string, eventName: string) {
        super();
        this.regexp = regexp;
        this.eventName = eventName;
    }

    abstract listener(input: string): void;
}

abstract class QemuMachine extends QemuDistribute {}

class QemuStm32f1 extends QemuMachine {
    private readonly table: Map<string, (input: string[]) => void>;
    private readonly event: events.EventEmitter;

    constructor() {
        super('^stm32f1', 'qemu-stm32f1');
        this.event = new events.EventEmitter();
        this.table = new Map([
            ['GPIOA', this.gpio],
            ['GPIOB', this.gpio],
            ['GPIOC', this.gpio],
            ['GPIOD', this.gpio],
            ['GPIOE', this.gpio],
            ['GPIOF', this.gpio],
            ['GPIOG', this.gpio],
        ]);
        this.register();
    }

    private register = () => {
        for (const [key, value] of this.table) {
            this.event.on(key, value);
        }
    }

    private gpio = (arg: string[]) => {
        const event = `${arg[1]}${arg[2]}`;
        const data: MachineData = { vtime: arg[0], type: arg[3], value: arg[4] };
        this.emit(event, data);
    }

    listener = (input: string) => {
        const data: QemuData = { type: 'stm32f1', message: input };
        // logger
        // console.log(data);
        this.emit('data', data);

        const arg = input.split(' ').slice(1);
        this.event.emit(arg[1], arg);
    }
}

class QemuSystem extends QemuDistribute {
    private readonly event: events.EventEmitter;
    private communications: Map<string, net.Socket>;
    private readonly table: Map<string, (arg: string[]) => void>;

    constructor() {
        super('^qemu-system', 'qemu-system');
        this.event = new events.EventEmitter();
        this.communications = new Map();
        this.table = new Map([
            ['-monitor', this.communication],
            ['-qmp', this.communication],
            ['-serial', this.communication]
        ]);
        this.register();
    }

    private register = () => {
        for (const [key, value] of this.table) {
            this.event.on(key, value);
        }
    }

    private communication = (arg: string[]) => {
        // -xxxx unix:xxxx.sock,server=on,wait=on:
        const sockName = arg[1].split(':')[1].split(',')[0];
        const type = sockName.split('.')[0];

        const sock = net.connect(sockName);
        sock.on('connect', () => {
            // const data: QemuData = { type: type, message: 'connected' };
            // logger
            // console.log(data);
            this.communications.set(type, sock);
        });
        sock.on('data', (input: string) => {
            const data: QemuData = { type: type, message: input };
            // logger
            // console.log(data);
            this.emit('data', data);
        });
        sock.on('end', () => {
            // const data: QemuData = { type: type, message: 'disconnect' };
            // logger
            // console.log(data);
            this.communications.delete(type);
        });
    }

    listener = (input: string) => {
        // const data: QemuData = { type: 'qemu-system', message: input };
        // logger
        // console.log(data);
        const arg = input.split(' ').slice(1);
        this.event.emit(arg[0], arg);
    }

    write = (data: QemuData) => {
        // logger
        // console.log(data);
        const sock = this.communications.get(data.type);
        if (typeof sock !== 'undefined') {
            sock.write(data.message);
        }
    }
}

class Qemu extends events.EventEmitter {
    private readonly event: events.EventEmitter;
    private readonly cfg: QemuConfig;
    private command: string;
    private options: string[];
    private table: Map<string, string>;
    machine: QemuMachine;
    system: QemuSystem;

    constructor(cfg: QemuConfig) {
        super();
        this.event = new events.EventEmitter();
        this.cfg = cfg;
        this.command = '';
        this.options = new Array();
        this.table = new Map();
        this.config();
        this.machine = Qemu.createMachine(this.cfg.socName);
        this.system = new QemuSystem();
        this.register(this.machine);
        this.register(this.system);
    }

    private static createMachine = (type: string): QemuMachine => {
        if (type === 'stm32f1') {
            return new QemuStm32f1;
        } else {
            throw new Error('UndefinedMachine');
        }
    }

    private config = () => {
        this.command = this.cfg.qemuPath;
        this.options.push('-machine');
        this.options.push(this.cfg.machineName);
        this.options.push('-kernel');
        this.options.push(this.cfg.firmwarePath);
        this.options.push('-icount');
        this.options.push('shift=auto,align=off,sleep=on');
        this.options.push('-trace');
        this.options.push(`events=${this.cfg.eventsPath}`);
        this.options.push('-nographic');
        for (let i = 0; i < this.cfg.serialNumber; i++) {
            this.options.push('-serial');
            this.options.push(`unix:serial${i + 1}.sock,server=on,wait=on`);
        }
        if (this.cfg.hasMonitor) {
            this.options.push('-monitor');
            this.options.push('unix:monitor.sock,server=on,wait=on');
        } else {
            this.options.push('-monitor');
            this.options.push('none');
        }
        if (this.cfg.hasQmp) {
            this.options.push('-qmp');
            this.options.push('unix:qmp.sock,server=on,wait=on');
        }
        if (this.cfg.hasDebug) {
            this.options.push('-gdb');
            this.options.push('tcp::1234');
            this.options.push('-s');
        }

        // logger
        // let data: QemuData = { type: 'qemu', message: this.command }
        // console.log(data);
        // data: QemuData = { type: 'qemu', message: this.options }
        // console.log(data);
    }

    private register = (distribute: QemuDistribute) => {
        this.table.set(distribute.regexp, distribute.eventName);
        this.event.on(distribute.eventName, distribute.listener);
        distribute.on('data', (data: QemuData) => {
            // TODO: filter
            this.emit('data', data);
        });
    }

    start = () => {
        const qemuProcess = child_process.spawn(this.command, this.options);
        qemuProcess.on('close', (code) => {
            // logger
            // const data: QemuData = { type: 'qemu', message: `exit ${code}` }
            // console.log(data);
            this.emit('close', code);
        });

        const qemuOutput = readline.createInterface(qemuProcess.stderr);
        qemuOutput.on('line', (input: string) => {
            for (const [key, value] of this.table) {
                if (input.match(key)) {
                    this.event.emit(value, input);
                    break;
                }
            }
        });
    }

    write = (data: QemuData) => {
        this.system.write(data);
    }
}

export { Qemu, QemuConfig, QemuData, MachineData };