/*
  This is the main application that contains 3 screens:
    Splash screen: provides a simple title and waits for the user to fall
    Countdown: once the user falls, the countdown will start from 30 sec until the user pressed "im good"
    Locator: if the user is not good, this screen will grab the GPS info from the user's phone and send it to multiple 'important' phone numbers
*/


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

const ID = '_' + Math.random().toString(36).substr(2, 9);

console.log("This is the ID: "+ID);





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

