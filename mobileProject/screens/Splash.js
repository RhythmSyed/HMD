import React, { Component } from 'react';
import { StyleSheet, Text, View, Button, Alert } from 'react-native';
import awsIot from 'aws-iot-device-sdk';


const device = awsIot.device({
    host: "a358ffdo7gf05m-ats.iot.us-east-2.amazonaws.com",
    clientId: Math.random(),
    protocol: "wss",
    accessKeyId: "AKIAIUIKD4PNBBO2TT5A",
    secretKey: "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"
});


class Splash extends Component {
    render() {
        device.on('connect', () => {   
            //device.subscribe('upload_topic')
        })

        // device.on('message', (topic, message) => {
        //     console.log(topic, message.toString())
        //     this.props.navigation.navigate('Countdown')
        // })
          
        device.on('error', error => {
            console.log(error)
        })

        return (
            <View style={styles.container}>
                <Text style={styles.title}>HMD: Health Monitoring Device</Text>

                <View style={styles.button}>
                    <Button title="Send Instruction" onPress={()=> {
                        device.publish('hmd_test', JSON.stringify(1))    
                    }}/>
                </View>

                <View style={styles.button}>
                    <Button title="Connect to HMD" onPress={()=> {
                        this.props.navigation.navigate('BLE')
                    }}/>
                </View>

            </View>
        );
    }
}

export default Splash;


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