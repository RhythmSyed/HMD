import React, { Component } from 'react';
import { StyleSheet, Text, View, Button, Alert } from 'react-native';
import TimerCountdown from 'react-native-timer-countdown';


class Activity_mode extends Component {
    constructor() {
        super()
        this.sensorData = {
            HeartRate: "",
            Accelerometer: "",
            Gyroscope: ""
        }
    }
    
    dataHandler(incoming_data) {
        if (incoming_data.indexOf('H') > -1) {
            this.sensorData.HeartRate = incoming_data
        } else if (incoming_data.indexOf('A') > -1) {
            this.sensorData.Accelerometer = incoming_data
        } else if (incoming_data.indexOf('G') > -1) {
            this.sensorData.Gyroscope = incoming_data
        }


    }

    render() {
        const { navigation } = this.props
        ble_context = navigation.getParam('ble_context', 'NO-THIS')
        ddb = navigation.getParam('ddb', 'NO-DDB')

        return (
            
            <View style={styles.container}>
                <Text style={styles.title}>ACTIVITY MODE</Text>
                
                {Object.keys(this.sensorData).map((sensor_name) => {
                    return <Text>
                                {sensor_name + ": " + this.sensorData[sensor_name] }
                            </Text>
                })}

                <TimerCountdown
                    initialSecondsRemaining={1000*1}
                    onTick={secondsRemaining => console.log('tick', secondsRemaining)}
                    onTimeElapsed={() => {
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
                    }}
                    allowFontScaling={true}
                    style={{ fontSize: 20,
                        marginTop: 200 }}
                />
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
        marginBottom: 20
    }
});