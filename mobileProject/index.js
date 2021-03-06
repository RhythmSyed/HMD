/** @format */

/* 
    react-native syntax: registers AppRegistry with App.js 
*/

import './shim'
import {AppRegistry} from 'react-native';
import App from './App';
import {name as appName} from './app.json';
import Splash from './screens/Splash';
import Countdown from './screens/Countdown';
import BLEPairing_mode from './screens/BLEPairing_mode';

AppRegistry.registerComponent(appName, () => App);
