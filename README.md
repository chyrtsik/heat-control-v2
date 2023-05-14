# heat-control-v2

This project demonstrates the usage of ESP8266 with a customized board as a controller for the heating system. It requires a custom-made board, which uses an SPI interface and extends the number of output ports of the controller to 2x8 (see ParallelBus implementation for details).
<br>Despite being a hobby project this solution proves to be reliable enough to control the author’s house equipment since 2018.
<br>It is minimalistic and does not require electronic controllers installed in rooms of the house: positioning the mechanical thermostat on the heating battery is enough for the system to notice, when to turn on / off the heating.

![A day of heating work](/img/title.png)

## Features
1. Constantly monitor temperatures and turn on / off heating automatically.
2. Adjust the heating mode to the weather conditions to minimize losses.
3. Works in pair with a solid-fueled boiler: switch to the flame control mode when the boiler is fired up, and switch to electric mode when the boiler is out of fuel.
4. Check the state of the heating equipment and raise alarms in case of broken equipment.
5. Integrates with a Consul server, and provides REST API, basic UI, and metrics.

## API

1. PUT /status?workflow-state=Heating|Idle
<br>Turn on / off the heating manually. It only changes heating conditions when 

2. PUT /config?vacationMode.enabled=false|true
<br>Enable/disable vacation mode. Use it together with custom vacation settings overrides to lower the demand for heating when the house is empty for a long time.

3. PUT /config?<other configuration params>
<br>Updates configuration parameters as specified.
Sample request:
<pre>
    curl --location --request PUT 'http://localhost:82/api/config?vacationMode.maxOutsideTemperature=5&vacationMode.minOutsideTemperature=-20&vacationMode.electricHeatingMaxTemperature=40&vacationMode.electricHeatingMinTemperature=20&vacationMode.electricHeatingDeltaTemperature=2.5&vacationMode.heatingTurnOnMaxOutsideTemperature=6&vacationMode.heatingTurnOnMaxInsideTemperature=12&vacationMode.heatingTurnOffMinOutsideTemperature=2&vacationMode.heatingTurnOffOutsideTemperatureToForceHeatingOff=10'
</pre>

4. GET /api/status
Retrieves a full status of a heating system. It includes information about the current mode, sensors, switches, serves, etc.
Example output is shown below:
<pre>
    {
      "version": "Jan 23 2023 13:00:01",
      "uptime": 365228327,
      "sensors": {
        "boardTemperature": 23.5,
        "boilerTemperature": 16.3,
        "feedTemperature": 15.3,
        "returnTemperature": 15.1,
        "outsideTemperature": 28.7,
        "insideTemperature": 21.5,
        "flueTemperature": 20,
        "exhaustTemperature": 19
      },
      "switches": {
        "pump": "off",
        "pump2": "off",
        "heater1": "off",
        "heater2": "off",
        "heater3": "off",
        "cooler": "off"
      },
      "flowSensor": {
        "ticks": 358177,
        "litresTotal": 1790.885,
        "litresPerMinute": 0
      },
      "power": {
        "heating": 0,
        "heating1m": -2.802597e-45,
        "heating15m": -6.165713e-44,
        "heating1h": -2.171375e-26,
        "heating24h": -0.000728,
        "heating7d": -0.001422
      },
      "valves": {
        "flueValve": 100,
        "boilerValve": 100
      },
      "workflow": {
        "currentState": {
          "name": "Idle",
          "pumpIsHealthy": true,
          "pumpLastMeasuredFlow": 7.26
        }
      }
    }
</pre>

## How to upload firmware to the sep chip 
1. Use OTA (if the device is online: /update - will open upload form)
2. Use USB to TTL converted. Ex: PL2303
 - sample converter: https://www.aliexpress.com/item/32757800387.html
 - driver download link (installs as COM5 by default): http://www.prolific.com.tw/US/ShowProduct.aspx?p_id=225&pcid=41
 When connecting, do not forget to connect Rx->Tx, Tx -> Rx on board and cable.

