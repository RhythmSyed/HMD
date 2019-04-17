import React, { Component } from 'react';
import { Platform, View, Text, StyleSheet, Button, Alert } from 'react-native';
import { BleManager } from 'react-native-ble-plx'
import { Buffer } from 'buffer'
import AwesomeButton from "react-native-really-awesome-button";
import awsIot from 'aws-iot-device-sdk';
var AWS = require('aws-sdk');
AWS.config.update({accessKeyId: 'AKIAIUIKD4PNBBO2TT5A', secretAccessKey: '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', region: 'us-east-2'});
var iotdata = new AWS.IotData({endpoint:"a358ffdo7gf05m-ats.iot.us-east-2.amazonaws.com"});
var ddb = new AWS.DynamoDB({apiVersion: '2012-10-08'});

const AWSdevice = awsIot.device({
  host: "a358ffdo7gf05m-ats.iot.us-east-2.amazonaws.com",
  clientId: Math.random(),
  protocol: "wss",
  accessKeyId: "AKIAIUIKD4PNBBO2TT5A",
  secretKey: "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"
});

AWSdevice.on('error', error => {
  console.log(error)
})


export default class BLEPairing_mode extends Component {

    constructor() {
      super()
      this.manager = new BleManager()
      
      this.state = {
        info: "PAIR", values: {}
      }

      this.prefixUUID = "6e400001"
      this.suffixUUID = "-b5a3-f393-e0a9-e50e24dcca9e"
      this.NordicserviceUUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
      this.RXcharacteristic = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
      this.TXcharacteristic = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
      this.curr_device = null

    }
  
    dataSender(data) {
      return Buffer.from(data).toString('base64')
    }

    dataReceiver(data) {
      return Buffer.from(data, 'base64').toString('ascii')
    }

    info(message) {
        this.setState({info: message})
    }
    
    error(message) {
    this.setState({info: "ERROR: " + message})
    }

    updateValue(key, value) {
    this.setState({values: {...this.state.values, [key]: value}})
    }

    getTimeStamp() {
      var date = new Date().getDate(); //Current Date
      var month = new Date().getMonth() + 1; //Current Month
      var year = new Date().getFullYear(); //Current Year
      var hours = new Date().getHours(); //Current Hours
      var min = new Date().getMinutes(); //Current Minutes
      var sec = new Date().getSeconds(); //Current Seconds
      var timeStamp = date + '/' + month + '/' + year + ' ' + hours + ':' + min + ':' + sec
      return timeStamp
    }

    scanAndConnect() {
      this.manager.startDeviceScan(null, null, (error, device) => {
        this.info("Scanning...");
        console.log(device);
  
        if (error) {
          this.error(error.message);
          return
        }
  
        if (device.name ==='HMD_wearable') {
        this.info("Connecting to HMD");
        this.manager.stopDeviceScan();

        device.connect()
          .then((device) => {
            this.info("Discovering services and characteristics");
            return device.discoverAllServicesAndCharacteristics()
          })
          .then((device) => {
            device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.TXcharacteristic, this.dataSender('BLEREADY'))
              .then((characteristic) => {
                this.info('BLE READY');
                this.curr_device = device
                return 
              }) 
          })
          .catch((error) => {
            this.error(error.message)
          })
        }

     });
    }



    render() {
    return (
        <View>
          <Text style={styles.title}>PAIRING MODE</Text>
          <View style={styles.button}>

            <AwesomeButton
              progress
              stretch
              onPress={next => {  
                if (Platform.OS === 'ios') {
                  this.manager.onStateChange((state) => {
                  if (state === 'PoweredOn') this.scanAndConnect()
                  })
                } else {
                    this.scanAndConnect()
                }
                next();
              }}
            >
              {this.state.info}
            </AwesomeButton>


          </View>

          {/* <View style={styles.button}>
            <Button title="LED OFF" onPress={()=> {
              this.curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('LED OFF'))
            }}/>
          </View> */}

          <View style={styles.activity_button}>
            <Button title="ACTIVITY MODE" onPress={()=> { 
              if (this.curr_device == null){
                Alert.alert(
                  'ERROR',
                  'HMD NOT PAIRED',
                  [
                    {text: 'OK', onPress: () => console.log('OK Pressed')},
                  ],
                  {cancelable: false}
                );
              }
              else {
                this.curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('ACTIVITY'))
                this.props.navigation.navigate('Activity_mode', {
                  ble_context: this,
                  AWSdevice: AWSdevice,
                  ddb: ddb
                })
              }
            }}/>
          </View>

          <View style={styles.sleep_button}>
            <Button title="SLEEP MODE" onPress={()=> {
              if (this.curr_device == null){
                Alert.alert(
                  'ERROR',
                  'HMD NOT PAIRED',
                  [
                    {text: 'OK', onPress: () => console.log('OK Pressed')},
                  ],
                  {cancelable: false}
                );
              }
              else {
                this.curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('SLEEP'))
                this.props.navigation.navigate('Sleep_mode', {
                  ble_context: this,
                  AWSdevice: AWSdevice,
                  ddb: ddb
                })
              }

            }}/>
          </View>
        </View>
        
    )
    }
}


const styles = StyleSheet.create({
  activity_button: {
    marginTop: 100
  },
  sleep_button: {
    marginTop: 10
  },
  title: {
    fontWeight: 'bold',
    fontSize: 18,
    color: 'black',
    marginBottom: 20,
    textAlign: 'center'
  }



}); 