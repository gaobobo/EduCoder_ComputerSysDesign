
import * as log4js from 'log4js';

// https://blog.csdn.net/weixin_42214717/article/details/128332445
log4js.configure({
    appenders: { // 输出源
        out: {
            type: 'stdout',
            layout: {
                type: 'pattern',
                pattern: '[%d][%p][PID:%z][%f{1}:%l] %m'
            } 
        },
        app: {
            type: 'dateFile',
            filename: 'log/jsrun.log',
            pattern: 'yyyy-MM-dd-hh',
            maxLogSize: '500K',
            numBackups: 3,
            keepFileExt: true,
            layout: {
                type: 'pattern',
                pattern: '[%d][%p][PID:%z][%f{1}:%l] %m'
            } 
        }
    },
    categories: { // 类别
        default: { 
            appenders: ['out', 'app'],
            enableCallStack: true,
            level: 'trace'
        }
    }
});

const logger = log4js.getLogger();

export {
    logger
}