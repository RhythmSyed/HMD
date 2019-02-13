import React, { Component } from 'react';
import { Platform, View, Text } from 'react-native';
import { BleManager } from 'react-native-ble-plx'

//const manager = new BleManager()

export default class SensorsComponent extends Component {

    constructor() {
      super()
      this.manager = new BleManager()
      this.state = {info: "", values: {}}
      this.prefixUUID = "6E400001"
      this.suffixUUID = "-B5A3-F393-E0A9-E50E24DCCA9E"
      this.sensors = {
        1: "Heartrate",
        2: "Accelerometer",
        3: "Gyroscope",
      }
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
    this.manager.startDeviceScan(null,
                                    null, (error, device) => {
        this.info("Scanning...")
        console.log(device)

        if (error) {
        this.error(error.message)
        return
        }

        if (device.name === 'HMD_wearable') {
        this.info("Connecting to HMD")
        this.manager.stopDeviceScan()
        device.connect()
            // .then((device) => {
            // this.info("Discovering services and characteristics")
            // return device.discoverAllServicesAndCharacteristics()
            // })
            // .then((device) => {
            // this.info("Setting notifications")
            // return this.setupNotifications(device)
            // })
            .then(() => {
            this.info("Listening...")
            }, (error) => {
            this.error(error.message)
            })
        }
    });
    }

    async setupNotifications(device) {
    for (const id in this.sensors) {
        const service = this.serviceUUID(id)
        const characteristicW = this.writeUUID(id)
        const characteristicN = this.notifyUUID(id)

        const characteristic = await device.writeCharacteristicWithResponseForService(
        service, characteristicW, "AQ==" /* 0x01 in hex */
        )

        device.monitorCharacteristicForService(service, characteristicN, (error, characteristic) => {
        if (error) {
            this.error(error.message)
            return
        }
        this.updateValue(characteristic.uuid, characteristic.value)
        })
    }
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
        </View>
    )
    }
}