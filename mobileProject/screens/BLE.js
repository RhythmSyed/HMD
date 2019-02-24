import React, { Component } from 'react';
import { Platform, View, Text, StyleSheet, Button, Alert } from 'react-native';
import { BleManager } from 'react-native-ble-plx'


curr_device = ''
NordicserviceUUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
RXcharacteristic = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

export default class SensorsComponent extends Component {

    constructor() {
      super()
      this.manager = new BleManager()
      this.state = {info: "", values: {}}
      this.prefixUUID = "6e400001"
      this.suffixUUID = "-b5a3-f393-e0a9-e50e24dcca9e"
      this.sensors = {
        1: "Heartrate"
      }
      this.NordicserviceUUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
      this.RXcharacteristic = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

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

          // device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, 'TEVEIE9O')
          //   .then((characteristic) => {
          //     this.info(characteristic.value);
          //     return 
          //   })

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
          {Object.keys(this.sensors).map((key) => {
              return <Text key={key}>
                      {this.sensors[key] + ": " + (this.state.values[this.notifyUUID(key)] || "-")}
                      </Text>
          })}

          <View style={styles.button}>
            <Button title="LED ON" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(NordicserviceUUID, RXcharacteristic, 'TEVEIE9O')
            }}/>
          </View>

          <View style={styles.button}>
            <Button title="LED OFF" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(NordicserviceUUID, RXcharacteristic, 'TEVEIE9GRg==')
            }}/>
          </View>

          <View style={styles.button}>
            <Button title="ACTIVITY MODE" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(NordicserviceUUID, RXcharacteristic, 'QUNUSVZJVFk=')
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