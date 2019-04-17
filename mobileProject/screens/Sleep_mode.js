import React, { Component } from 'react';
import { StyleSheet, Text, View, Button, Alert } from 'react-native';
import TimerCountdown from 'react-native-timer-countdown';
import awsIot from 'aws-iot-device-sdk';
var AWS = require('aws-sdk');
AWS.config.update({accessKeyId: 'AKIAIUIKD4PNBBO2TT5A', secretAccessKey: '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', region: 'us-east-2'});
var lambda = new AWS.Lambda();

const AWSdevice = awsIot.device({
    host: "a358ffdo7gf05m-ats.iot.us-east-2.amazonaws.com",
    clientId: Math.random(),
    protocol: "wss",
    accessKeyId: "AKIAIUIKD4PNBBO2TT5A",
    secretKey: "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"
  });

  sleepAnalysisResponse = null

  AWSdevice.on('connect', ()=>{
      AWSdevice.subscribe('sleep_mode/analysis')
  })

  AWSdevice.on('message', (topic, message) => {
    sleepAnalysisResponse = message.toString()
  })


class Activity_mode extends Component {
    constructor() {
        super()
        this.sensorData = {
            HeartRate: "bad",
            Accelerometer: "bad"
        }
        this.timer_enable = false
    }

    dataHandler(incoming_data) {
        if (incoming_data.indexOf('H') > -1) {
            this.sensorData.HeartRate = incoming_data.split('H')[0]
        } else if (incoming_data.indexOf('A') > -1) {
            this.sensorData.Accelerometer = incoming_data.split('A')[0]
        }
    }
    
    render() {
        const { navigation } = this.props
        ble_context = navigation.getParam('ble_context', 'NO-THIS')
        ddb = navigation.getParam('ddb', 'NO-DDB')
        
        if (sleepAnalysisResponse != null) {
            this.props.navigation.navigate('Analytics', {
                sleepAnalysis: sleepAnalysisResponse
            })
            sleepAnalysisResponse = null

        }

        return (
            
            <View style={styles.container}>
                <Text style={styles.title}>SLEEP MODE</Text>
                
                {Object.keys(this.sensorData).map((sensor_name) => {
                    return <Text>
                                {sensor_name + ": " + this.sensorData[sensor_name] }
                            </Text>
                })}

                <TimerCountdown
                    initialSecondsRemaining={1000*1}
                    onTimeElapsed={() => {
                        if (this.timer_enable === true) {
                            ble_context.curr_device.readCharacteristicForService(ble_context.NordicserviceUUID, ble_context.TXcharacteristic)
                            .then((characteristic) => {
                                ble_context.info('DATA RECEIVED');
                                incoming_data = ble_context.dataReceiver(characteristic.value)
                                this.dataHandler(incoming_data)

                                var params = {
                                    TableName:'HMD_DATA',
                                    Item:{
                                    'TimeStamp': {S: String(ble_context.getTimeStamp())},
                                    'HeartRate': {S: String(this.sensorData.HeartRate)},
                                    'Accelerometer': {S: String(this.sensorData.Accelerometer)}
                                    }
                                };
                                
                                ddb.putItem(params, function(err, data) {
                                    if (err) {
                                        console.error("Unable to add item. Error JSON:", JSON.stringify(err, null, 2));
                                    } else {
                                        console.log("Added item:", JSON.stringify(data, null, 2));
                                    }
                                });

                                this.setState(this.sensorData)
                                return
                            })
                        } else if (this.timer_enable === false) {
                            this.setState(this.sensorData)
                            return
                        }
                    }}
                    allowFontScaling={true}
                    style={{ fontSize: 10 }}
                />

                <View style={styles.button}>
                    <Button title="START SLEEP" onPress={()=> {     
                        ble_context.curr_device.writeCharacteristicWithResponseForService(ble_context.NordicserviceUUID, ble_context.RXcharacteristic, ble_context.dataSender('STARTSLEEP'))
                        this.timer_enable = true
                    }}/>
                </View>

                <View style={styles.button}>
                    <Button title="STOP SLEEP" onPress={()=> {              
                        ble_context.curr_device.writeCharacteristicWithResponseForService(ble_context.NordicserviceUUID, ble_context.RXcharacteristic, ble_context.dataSender('STOPSLEEP'))
                        this.timer_enable = false
                    }}/>
                </View>

                <View style={styles.button}>
                    <Button title="COMPLETE SLEEP" onPress={()=> {              
                        var params = {
                            FunctionName: 'HMDsleep_mode',
                            Payload: JSON.stringify({
                                "key1": "value1",
                                "key2": "value2",
                                "key3": "value3"
                              })
                        }

                        lambda.invoke(params, (err, data) => {
                            if (err) console.log(err, err.stack)
                            else console.log(data)
                        })

                    }}/>
                </View>

            </View>
        );
    }
}

export default Activity_mode;


const styles = StyleSheet.create({
    container: {
        backgroundColor: 'white',
        flex: 1,
        alignItems: 'center',
        justifyContent: 'center',
    },
    title: {
        fontWeight: 'bold',
        fontSize: 18,
        color: 'black',
        marginBottom: 20
    },
    button: {
        marginTop: 50
    }
});