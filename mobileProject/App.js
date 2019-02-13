import React, {Component} from 'react';
import { createStackNavigator } from 'react-navigation';

import Locator from './screens/Locator';
import Splash from './screens/Splash';
import Countdown from './screens/Countdown';

export default class App extends Component {
  render() {
    return (
      <StackNav />
    );
  }
}

const StackNav = createStackNavigator({
  Splash: Splash,
  Countdown: Countdown,
  Locator: Locator
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

