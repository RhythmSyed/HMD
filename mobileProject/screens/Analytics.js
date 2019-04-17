import React, { Component } from 'react';
import { StyleSheet, Text, View, Button, Alert, BackAndroid } from 'react-native';


class Analytics extends Component {
    constructor() {
        super()
        this.sleepData = {
            maxHeartRate: "bad",
            minHeartRate: "bad",
            maxMovement:  "bad",
            minMovement:  "bad",
            worstSleepTime: "bad",
            worstSleepHR: "bad",
            worstSleepMovement: "bad",
            duration: "bad",
            totalREMcycles: "bad"
        }
    }
    
    render() {
        const { navigation } = this.props
        sleepAnalysis = navigation.getParam('sleepAnalysis', 'NO-SLEEP-ANALYSIS')
        sleepAnalysis = JSON.parse(sleepAnalysis)
        this.sleepData = {
            maxHeartRate: sleepAnalysis.maxHeartRate,
            minHeartRate: sleepAnalysis.minHeartRate,
            maxMovement: sleepAnalysis.maxMovement,
            minMovement: sleepAnalysis.minMovement,
            worstSleepTime: sleepAnalysis.worstSleepTime,
            worstSleepHR: sleepAnalysis.worstSleepHR,
            worstSleepMovement: sleepAnalysis.worstSleepMovement,
            duration: sleepAnalysis.duration,
            totalREMcycles: sleepAnalysis.totalREMcycles
        }

        return (
            
            <View style={styles.container}>
                <Text style={styles.title}>SLEEP ANALYTICS FROM LAST NIGHT</Text>

                {Object.keys(this.sleepData).map((data_prop) => {
                    return <Text>
                                {data_prop + ": " + this.sleepData[data_prop] }
                            </Text>
                })}  

                <View style={styles.button}>
                    <Button  title="CLOSE APP" onPress={()=> {              
                        BackAndroid.exitApp();
                    }}/>
                </View>     
                     
            </View>
            
        );
    }
}

export default Analytics;


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
        marginTop: 100
    }
});