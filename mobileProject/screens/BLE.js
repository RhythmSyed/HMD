import React, { Component } from 'react';
import { Platform, View, Text, StyleSheet, Button, Alert } from 'react-native';
import { BleManager } from 'react-native-ble-plx'
import { Buffer } from 'buffer'
import TimerCountdown from 'react-native-timer-countdown'


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
        info: "", values: {}
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

    // componentWillMount() {
    //   if (Platform.OS === 'ios') {
    //       this.manager.onStateChange((state) => {
    //       if (state === 'PoweredOn') this.scanAndConnect()
    //       })
    //   } else {
    //       this.scanAndConnect()
    //   }
    // }

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
          
          <Text>{this.state.info}</Text>

          {Object.keys(this.sensorData).map((sensor_name) => {
              return <Text>
                        {sensor_name + ": " + this.sensorData[sensor_name] }
                     </Text>
          })}

          <View style={styles.button}>
            <Button title="START CONNECTION" onPress={()=> {
              if (Platform.OS === 'ios') {
                this.manager.onStateChange((state) => {
                if (state === 'PoweredOn') this.scanAndConnect()
                })
              } else {
                  this.scanAndConnect()
              }


            }}/>
          </View>

          {/* <View style={styles.button}>
            <Button title="LED OFF" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('LED OFF'))
            }}/>
          </View> */}

          <View style={styles.button}>
            <Button title="ACTIVITY MODE" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('ACTIVITY'))
              this.props.navigation.navigate('Activity_mode')
            }}/>
          </View>

          <View style={styles.button}>
            <Button title="SLEEP MODE" onPress={()=> {
              curr_device.writeCharacteristicWithResponseForService(this.NordicserviceUUID, this.RXcharacteristic, this.dataSender('SLEEP'))
              this.props.navigation.navigate('Sleep_mode')
            }}/>
          </View>

          <TimerCountdown
            initialSecondsRemaining={1000*30}
            onTick={secondsRemaining => console.log('tick', secondsRemaining)}
            onTimeElapsed={() => {
              curr_device.readCharacteristicForService(this.NordicserviceUUID, this.TXcharacteristic)
              .then((characteristic) => {
                this.info('DATA RECEIVED');
                incoming_data = this.dataReceiver(characteristic.value)
                console.log('incoming_data: ' + incoming_data)
                

                if (incoming_data.indexOf('H') > -1) {
                  this.sensorData.HeartRate = incoming_data
                } else if (incoming_data.indexOf('A') > -1) {
                  this.sensorData.Accelerometer = incoming_data
                } else if (incoming_data.indexOf('G') > -1) {
                  this.sensorData.Gyroscope = incoming_data
                }

                this.setState(this.state)
                return
              })
            }}
            allowFontScaling={true}
            style={{ fontSize: 20 }}
          />


        </View>
        
    )
    }
}


const styles = StyleSheet.create({
  button: {
    marginTop: 10
  }
});