import React, { Component } from 'react';
import { PermissionsAndroid, View, Text, StyleSheet, Button, Alert, Animated } from 'react-native';
import MapView, { PROVIDER_GOOGLE } from 'react-native-maps';

/* 
  This page gets the user location by accessing the android manifest and getting coordinate data
  from the GPS hardware on the phone.
  The coordinates are displayed using google maps 
*/

async function requestUserLocation(){
    try
    {
      const granted = await PermissionsAndroid.request(PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      {
        'title' : 'Get Current Location',
        'message': 'Allow app to access location services to get your current location'
      })
      
      if (granted === PermissionsAndroid.RESULTS.GRANTED){
        console.log("You can access Location now")
        return true;
      }
      else {
        console.log("Location Access Denied")
        return false;
      }
    } 
    catch(err) {
      return false;
    }
}

class Locator extends Component { 
  state = {
    location_marker: []
  }

  getUserLocation() {
    navigator.geolocation.getCurrentPosition(
      (position) => {
        this.state.location_marker.push({
          title: "location",
          coordinates: {
            latitude: position.coords.latitude,
            longitude: position.coords.longitude
          },
          key: "location",
          description: "(" + position.coords.latitude.toString() + "," + position.coords.longitude.toString() + ")"
        });
        this.setState(this.state)
      }
    )
  }
  
  render() {
    if(requestUserLocation()){
      this.getUserLocation();
    } else {
      Alert.alert(
        'Turn On GPS',
        [{text: 'OK', onPress: () => console.log('OK Pressed')}],
        { cancelable: false }
      )
    }
    return (
      <View style={styles.mapContainer} key={'initial'}>
        <MapView
          initialRegion={{
            latitude: 40.42877019,
            longitude: -86.91209748,
            latitudeDelta: 0,
            longitudeDelta: 0
          }}
          style={styles.map}
          onPress={
            (event) => {}
          }
          showsBuildings={true}
          provider={PROVIDER_GOOGLE}
          mapType = "standard"
          showsIndoors = {true}
          loadingEnabled = {true}
        >
        {this.state.location_marker.map(marker => (
          <MapView.Marker 
            coordinate={marker.coordinates}
            title={marker.title}
            key={marker.key}
            description={marker.description}
          />
        ))}
        
        </MapView>          
      </View>
    )
  }

}

export default Locator;



// Style Sheets //

const styles = StyleSheet.create({
  mapContainer: {
      width: '100%',
      height: 550,
      marginTop: 20
  },
  map: {
      width: '100%',
      height: '100%',
      justifyContent: 'center'
      
  },
  mapInstruction: {
    textAlign: 'center',
    color: "#9400d3",
    fontWeight: 'bold',
    fontSize: 18
  }
});