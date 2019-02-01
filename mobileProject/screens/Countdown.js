import TimerCountdown from 'react-native-timer-countdown';
import React, { Component } from 'react';
import { StyleSheet, Text, View, Button } from 'react-native';
import awsIot from 'aws-iot-device-sdk';

// Reference Attribution: https://www.npmjs.com/package/react-native-timer-countdown
/*
    countdown sequence once fall is detected. Connection to AWS is authenticated with:
        host: direct API to IOT 'thing' with publish and subscribe permissions
        accessKeyID, secretKey: generated with AWS IAM (Identity and Access Management)
*/

const device = awsIot.device({
    host: "a2pv8pigeo9wbh-ats.iot.us-east-2.amazonaws.com",
    clientId: Math.random(),
    protocol: "wss",
    accessKeyId: "AKIAJBDBZHG3VXU5TLQQ",
    secretKey: "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"
});

class Countdown extends Component { 
    render() {
        return (
            <View>
                <TimerCountdown
                    initialSecondsRemaining={1000*30}
                    onTick={secondsRemaining => console.log('tick', secondsRemaining)}
                    onTimeElapsed={() => {this.props.navigation.navigate('Locator')}}
                    allowFontScaling={true}
                    style={{ fontSize: 20 }}
                />
                <Button title="I'm Good" onPress={()=> {
                    device.publish('im_good', JSON.stringify(1))    
                    this.props.navigation.navigate('Splash')
                }}/>
            </View>
        )
    }
}

export default Countdown;
