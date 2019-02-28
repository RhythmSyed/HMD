import React, { Component } from 'react';
import { Platform, View, Text, StyleSheet, Button, Alert } from 'react-native';
import { BleManager } from 'react-native-ble-plx'
import { Buffer } from 'buffer'


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
        info: "", values: {},
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

    serviceUUID(num) {
      return this.prefixUUID + num + "0" + this.suffixUUID
    }
  
    notifyUUID(num) {
      return this.prefixUUID + num + "1" + this.suffixUUID
    }
  
    writeUUID(num) {
      return this.prefixUUID + num + "2" + this.suffixUUID
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

    componentWillMount() {
      if (Platform.OS === 'ios') {
          this.manager.onStateChange((state) => {
          if (state === 'PoweredOn') this.scanAndConnect()
          })
      } else {
          this.scanAndConnect()
      }
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
            this.info('READY');
            curr_device = device

          // device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.TXcharacteristic, 'NzZCUE0=')
          //   .then((characteristic) => {
          //     //this.info(characteristic.value);
          //     return 
          //   }) 
          //   .then (() => {
          //     device.readCharacteristicForService(this.NordicserviceUUID, this.TXcharacteristic)
          //       .then((characteristic) => {
          //         text = Buffer.from(characteristic.value, 'base64').toString('ascii')
          //         this.info(text);
          //         return
          //       })
          //   })
            
          device.readCharacteristicForService(this.NordicserviceUUID, this.TXcharacteristic)
          .then((characteristic) => {
            this.info('BPM RECEIVED');
            current_heartRateValue = this.dataReceiver(characteristic.value)
            this.setState(this.state)
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
          
          <Text>{this.state.info}</Text>

          {Object.keys(this.state).map((key) => {
            if (key == 'HeartRate') {
              return <Text>
                        {this.state[key] + ": " + heartRateValue }
                     </Text>
            }
          })}

          <View style={styles.button}>
            <Button title="LED ON" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('LED ON'))
            }}/>
          </View>

          <View style={styles.button}>
            <Button title="LED OFF" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('LED OFF'))
            }}/>
          </View>

          <View style={styles.button}>
            <Button title="ACTIVITY MODE" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('ACTIVITY'))
            }}/>
          </View>

          <View style={styles.button}>
            <Button title="READ HEARTRATE " onPress={()=> {
              curr_device.readCharacteristicForService(this.NordicserviceUUID, this.TXcharacteristic)
              .then((characteristic) => {
                this.info('BPM RECEIVED');
                incoming_data = this.dataReceiver(characteristic.value)
                 if (incoming_data[-1] == 'H') {
                    heartRateValue = incoming_data.split('H')[0]
                 }
                this.setState(this.state)
                return
              })
            }}/>
          </View>


        </View>
        
    )
    }
}


const styles = StyleSheet.create({
  button: {
    marginTop: 10
  }
});