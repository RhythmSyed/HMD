import React, { Component } from 'react';
import { StyleSheet, Text, View, Button, Alert } from 'react-native';
import awsIot from 'aws-iot-device-sdk';


class Sleep_mode extends Component {
    render() {
        return (
            <View style={styles.container}>
                <Text style={styles.title}>SLEEP MODE</Text>

                <View style={styles.button}>
                    <Button title="Start Tracking" onPress={()=> {  
                    }}/>
                </View>

            </View>
        );
    }
}

export default Sleep_mode;


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