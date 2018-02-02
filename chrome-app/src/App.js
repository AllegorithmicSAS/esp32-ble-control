import React, { Component } from 'react';
import EventEmitter from 'events';
import LedDriver from './LedDriver';
import './App.css';
import {
  Button,
  Menu,
  MenuItem,
  MenuDivider,
  Popover,
  Position,
  Slider
} from '@blueprintjs/core';

import '@blueprintjs/core/dist/blueprint.css';

class Header extends Component {
  render() {
    return (
      <nav className="pt-navbar pt-dark">
        <div className="pt-navbar-group pt-align-left">
          <div className="pt-navbar-heading">Bluetooth LED Controller</div>
        </div>
      </nav>
    );
  }
}

const menu = (
  <Menu>
    <MenuItem text="New" />
    <MenuItem text="Open" />
    <MenuItem text="Save" />
    <MenuDivider />
    <MenuItem text="Settings..." />
  </Menu>
);

const NoBluetooth = () => {
  return (
    <div class="no-bluetooth pt-non-ideal-state">
      <div class="pt-non-ideal-state-visual pt-non-ideal-state-icon">
        <span class="pt-icon pt-icon-cell-tower" />
      </div>
      <h4 class="pt-non-ideal-state-title">Bluetooth not available</h4>
      <div class="pt-non-ideal-state-description">
        We recommend using Google Chrome to use this app. If you're already on
        this browser, you can enable it here :
        <pre>chrome://flags/#enable-experimental-web-platform-features</pre>
      </div>
    </div>
  );
};

const ConnectToBluetooth = ({ onConnected }) => {
  return <div />;
};

class App extends Component {
  state = {
    hasBle: navigator.bluetooth,
    connected: false,
    singleBrightness: 0.5,
    multiBrightness: 0.25,
    periodMs: 2000
  };

  constructor() {
    super();
    this.handleSingleBrightness = this.handleSingleBrightness.bind(this);
    this.handleMultiBrightness = this.handleMultiBrightness.bind(this);
    this.handlePeriod = this.handlePeriod.bind(this);
  }

  handleClick() {
    LedDriver.connect()
      .then(() => {
        this.setState({ connected: true });
      })
      .catch(console.error);
  }

  syncChaserParams() {
    if (!this.state.connected) return;
    LedDriver.setChaserParams({
      singleBrightness: this.state.singleBrightness,
      multiBrightness: this.state.multiBrightness,
      periodMs: this.state.periodMs
    }).catch(() => this.setState({ connected: false }));
  }

  handlePeriod(val) {
    this.setState({ periodMs: val });
    setTimeout(() => this.syncChaserParams(), 0);
  }

  handleSingleBrightness(val) {
    this.setState({ singleBrightness: val });
    setTimeout(() => this.syncChaserParams(), 0);
  }

  handleMultiBrightness(val) {
    this.setState({ multiBrightness: val });
    setTimeout(() => this.syncChaserParams(), 0);
  }

  render() {
    return (
      <div style={{ maxWidth: '1160px', margin: '0 auto' }}>
        <div>
          <Header />
          <div className="main-content" style={{ padding: '1em' }}>
            {!navigator.bluetooth && <NoBluetooth />}

            {this.state.connected && <div>Connected to {LedDriver.name}</div>}
            {!this.state.connected && (
              <Button onClick={() => this.handleClick()}>
                Connect to BLE Device
              </Button>
            )}

            <div>Brightness when single LED is ON.</div>
            <Slider
              min={0}
              max={1}
              labelStepSize={0.25}
              stepSize={0.05}
              onChange={this.handleSingleBrightness}
              value={this.state.singleBrightness}
            />
            <div>Albedo capture brightness</div>
            <Slider
              min={0}
              max={0.75}
              labelStepSize={0.25}
              stepSize={0.05}
              onChange={this.handleMultiBrightness}
              value={this.state.multiBrightness}
            />
            <div>Delay between each change (in ms).</div>
            <Slider
              min={100}
              max={4000}
              stepSize={100}
              labelStepSize={500}
              onChange={this.handlePeriod}
              value={this.state.periodMs}
            />
          </div>
        </div>
      </div>
    );
  }
}

export default App;
