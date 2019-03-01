import React, {Component} from 'react';
import { createStackNavigator } from 'react-navigation';

import Splash from './screens/Splash';
import BLE from './screens/BLE';
import Sleep_mode from './screens/Sleep_mode';
import Activity_mode from './screens/Activity_mode';

export default class App extends Component {
  render() {
    return (
      <StackNav />
    );
  }
}

const StackNav = createStackNavigator({
  Splash: Splash,
  BLE: BLE,
  Activity_mode: Activity_mode,
  Sleep_mode: Sleep_mode
});


//hides weird timer related warning
import { YellowBox } from 'react-native';
import _ from 'lodash';

YellowBox.ignoreWarnings(['Setting a timer']);
const _console = _.clone(console);
console.warn = message => {
  if (message.indexOf('Setting a timer') <= -1) {
    _console.warn(message);
  }
};

