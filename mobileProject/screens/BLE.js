import React, { Component } from 'react';
import { Platform, View, Text, StyleSheet, Button, Alert } from 'react-native';
import { BleManager } from 'react-native-ble-plx'
import { Buffer } from 'buffer'
import AwesomeButton from "react-native-really-awesome-button";


curr_device = ''
NordicserviceUUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
RXcharacteristic = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
TXcharacteristic = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
heartRateValue = '-'
accelValue = '-'
gyroScope = '-'

export default class SensorsComponent extends Component {

    constructor() {
      super()
      this.manager = new BleManager()
      
      this.state = {
        info: "PAIR", values: {}
      }

      this.sensorData = {
        HeartRate: "",
        Accelerometer: "",
        Gyroscope: ""
      }

      this.prefixUUID = "6e400001"
      this.suffixUUID = "-b5a3-f393-e0a9-e50e24dcca9e"
      this.NordicserviceUUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
      this.RXcharacteristic = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
      this.TXcharacteristic = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
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
            device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.TXcharacteristic, this.dataSender('BLE READY'))
              .then((characteristic) => {
                this.info('BLE READY');
                curr_device = device
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
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('LED OFF'))
            }}/>
          </View> */}

          <View style={styles.activity_button}>
            <Button title="ACTIVITY MODE" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('ACTIVITY'))
              this.props.navigation.navigate('Activity_mode', {
                device: curr_device,
                this: this
              })
            }}/>
          </View>

          <View style={styles.sleep_button}>
            <Button title="SLEEP MODE" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('SLEEP'))
              this.props.navigation.navigate('Sleep_mode', {
                device: curr_device,
                this: this
              })
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