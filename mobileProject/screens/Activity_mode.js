import React, { Component } from 'react';
import { StyleSheet, Text, View, Button, Alert } from 'react-native';
import TimerCountdown from 'react-native-timer-countdown';


class Activity_mode extends Component {
    render() {

        const { navigation } = this.props
        curr_device = navigation.getParam('device', 'NO-DEVICE')
        new_this = navigation.getParam('this', 'NO-THIS')

        return (
            
            <View style={styles.container}>
                <Text style={styles.title}>ACTIVITY MODE</Text>
                {Object.keys(new_this.sensorData).map((sensor_name) => {
                    return <Text>
                                {sensor_name + ": " + new_this.sensorData[sensor_name] }
                            </Text>
                })}

                <View style={styles.button}>
                    <Button title="Start Tracking" onPress={()=> {  
                    }}/>
                </View>

                <TimerCountdown
                    initialSecondsRemaining={1000*30}
                    onTick={secondsRemaining => console.log('tick', secondsRemaining)}
                    onTimeElapsed={() => {
                        curr_device.readCharacteristicForService(new_this.NordicserviceUUID, new_this.TXcharacteristic)
                        .then((characteristic) => {
                            new_this.info('DATA RECEIVED');
                            incoming_data = new_this.dataReceiver(characteristic.value)
                            console.log('incoming_data: ' + incoming_data)
                            

                            if (incoming_data.indexOf('H') > -1) {
                                new_this.sensorData.HeartRate = incoming_data
                            } else if (incoming_data.indexOf('A') > -1) {
                                new_this.sensorData.Accelerometer = incoming_data
                            } else if (incoming_data.indexOf('G') > -1) {
                                new_this.sensorData.Gyroscope = incoming_data
                            }

                            new_this.setState(new_this.state)
                            return
                        })
                    }}
                    allowFontScaling={true}
                    style={{ fontSize: 20 }}
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