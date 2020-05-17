"use strict";
const mri         = require("mri");
const SerialPort  = require('serialport')
const Readline	  = require('@serialport/parser-readline')
const mqtt        = require('mqtt')
const argv        = process.argv.slice(2);

let options = mri( argv, { default: {
    serialPortName: "/dev/ttyACM0",
    serialPortRate: 921600,
    host:           'localhost',
    port:           1883,
    topic:          'ventilator/measurement/wilson',
    location:       'makeSpace',
    version:        '0.81'
}});

const noLog        = function(){};
const showLog      = function(log){ console.log(log); };
const logMessage   = options.verbose!==undefined?showLog:noLog;
const debugMessage = options.debug!==undefined?showLog:noLog;

const port = new SerialPort(
    options.serialPortName, { 
        baudRate: options.serialPortRate,
    }, function (err) {
        if (err) {
            showLog('Serial port error: '+ err.message);
            process.exit(1);
        }
    }
);

const parser    = port.pipe(new Readline({ delimiter: '\r\n' }));
let mqttOptions = { port: options.port, protocolVersion:4 }
var client      = mqtt.connect('mqtt://'+options.host, mqttOptions)

showLog("Connecting...");

client.on('connect', function() {

    showLog('Mqtt client connected');

    let pacient  = options.topic.split('/').pop();
    let location = options.location;
    let version  = options.version;
    let topic    = options.topic;

    parser.on('data',function(data){
    
        let receiveData = data.toString();
        logMessage("Serial port data: "+ receiveData);

        try {

            let ventilatorData  = JSON.parse(receiveData);
            let messageType     = ventilatorData.type;

            ventilatorData.time = Date.now()+'000000';
                          
            let payload = [
                ventilatorData,
                { 
                    tag1: pacient ,
                    tag2: location,
                    tag3: version
                }
            ];
    
            let mqttPayload = JSON.stringify(payload);
            let mqttTopic   = topic+'/'+messageType;

            logMessage('Mqtt payload: '+ mqttPayload)
            client.publish(mqttTopic, mqttPayload);
        }
        catch(e){
            debugMessage("Debug: "+ receiveData);
        }
    });
    
    port.on('error', function(err) {
        showLog('Serial port error: '+ err.message);
        process.exit(1);
    })

    port.on('close', function() {
        showLog('Port closed');
    })
});

client.on('error', function(err) {
    showLog('Mqtt error: '+ err,message);
    process.exit(1);
});