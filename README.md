<h1>iot-santa</h1>
<p>Merry Christmas to you! This repository holds information about your Secret Santa gift, and the code powering it. The code was designed to demonstrate what you can do with an ESP8266, and give you ideas for what to do next. I hope you enjoy it!</p>

<h2>What's included?</h2>
<p>Your gift includes three devices, pre-configured to demonstrate some basic IoT-like interconnectivity:</p>
<table>
  <thead>
    <tr>
      <th>Device</th>
      <th>Details</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>
        NodeMCU ESP8266<br>
        <br>
        <a href="#"><img src="https://imgaz1.staticbg.com/thumb/large/oaupload/banggood/images/67/10/0e04f320-e4f9-45bb-a24e-0b66a9000dee.JPG" width="320px"></a>
      </td>
      <td>
        This is an Arduino-compatible WiFi development board. It's been flashed with the Arduino code in the <a href="iot-santa.ino">iot-santa.ino</a> sketch, and files from <a href="data">data</a> have been uploaded to <a href="https://tttapa.github.io/ESP8266/Chap11%20-%20SPIFFS.html">SPIFFS</a> (SPI flash file system).<br>
        <br>
        In this demo, the ESP8266 serves the following purposes:<br>
        <ul>
          <li><b>Access point</b>: WiFi access point for you and the Sonoff (see below) to connect
            <ul>
              <li><b>SSID</b>: Secret Santa</li>
              <li><b>Password</b>: Christmas 2019!</li>
            </ul>
          </li>
          <li><b>Web server</b>: HTTP web server for you to browse, and to allowing you to indirectly control the Sonoff
            <ul>
              <li><b>Address</b>: http://192.168.4.1</li>
            </ul>
          </li>
          <li><b><a href="http://mqtt.org/">MQTT</a> broker</b>: MQTT broker, which sends messages to - and receives messages from - you and the Sonoff
            <ul>
              <li><b>Port</b>: 1883</li>
              <li><b>Max subscriptions</b>: 30</li>
              <li><b>Max retained topics</b>: 30</li>
            </ul>
          </li>
        </ul>
        All of this is configurable within the Arduino sketch code in this repo.<br>
        <br>
        Once both you and the Sonoff are connected to the access point on the ESP8266, you can browse the website hosted on the ESP8266 (at http://192.168.4.1) and control the Sonoff with the TOGGLE button.
      </td>
    </tr>
    <tr>
      <td>
        Sonoff Basic R2<br>
        <br>
        <a href="#"><img src="https://imgaz.staticbg.com/thumb/large/oaupload/banggood/images/52/BD/a8f165ed-35a6-48ef-a8f3-87bfaee9e83f.jpg" width="320px"></a>
      </td>
      <td>
        This is a 10 amp 90-250V AC 50/60Hz smart switch which relays the voltage from INPUT to OUTPUT when turned on. With it, you can control a lamp, fan, wax warmer, etc.<br>
        <br>
        As gifted to you, the Sonoff will:<br>
        <ol>
          <li>Attempt to connect to the ESP8266 via WiFi</li>
          <li>Subscribe to the following MQTT topics:
            <ul>
              <li><b>cmnd/sonoff/POWER</b>: to listen for commands</li>
              <li><b>stat/sonoff/POWER</b>: to report its state</li>
            </ul>
          </li>
          <li>Wait for MQTT messages from the website hosted on the ESP8266, and update its ON/OFF state accordingly</li>
        </ol>
      </td>
    </tr>
    <tr>
      <td>
        Mini USB to TTL Serial Adapter<br>
        <br>
        <a href="#"><img src="https://images-na.ssl-images-amazon.com/images/I/61Nfdfx9ZxL._AC_SX679_.jpg" width="320px"></a>
      </td>
      <td>
        This USB to TTL serial adapter allows you to connect your computer to a Sonoff, ESP8266, many smart bulbs and plugs, and even Arduino Pro Minis I believe, to flash firmware and code sketches.<br>
        <br>
        <b>It's important you don't update the Windows driver</b>, as it appears this is <a href="https://www.amazon.com/HiLetgo-FT232RL-Converter-Adapter-Breakout/dp/B00IJXZQ7C">a "cloned" FTDI device</a> which may become bricked by the newest FTDI drivers. In short, there's been an ongoing battle between FTDI and manufacturers making cheap clones of FTDI's devices. If you do end up bricking the adapter, it's no big deal - you can <a href="http://electropit.com/index.php/2016/02/01/unbrick-your-non-genuine-ftdi-device-2016-january/">unbrick the device</a> to restore its functionality.
      </td>
    </tr>
  </tbody>
</table>

<h2>Arduino IDE Notes</h2>
<ul>
  <li>To get your Arduino IDE connected to the ESP8266, you need to ensure ESP8266 is in your boards manager:
    <ol>
      <li>In the Arduino IDE, go to <i>Tools</i> > <i>Board</i> > <i>Boards Manager...</i></li>
      <li>Search for <i>ESP8266</i> and install <i><b>esp8266</b> by <b>ESP8266 Community</b></i></li>
    </ol>
  </li>
  <li>To upload this sketch, you need to install the uMQTTBroker, ESPAsyncTCP, and ESPAsyncWebServer libraries:
    <ol>
      <li>Visit each of the repositories and download the repository as a zip file:
        <ul>
          <li><b>uMQTTBroker</b>: https://github.com/martin-ger/uMQTTBroker/archive/master.zip (<a href="https://github.com/martin-ger/uMQTTBroker">Repo</a>)</li>
          <li><b>ESPAsyncTCP</b>: https://github.com/me-no-dev/ESPAsyncTCP/archive/master.zip (<a href="https://github.com/me-no-dev/ESPAsyncTCP">Repo</a>)</li>
          <li><b>ESPAsyncWebServer</b>: https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip (<a href="https://github.com/me-no-dev/ESPAsyncWebServer">Repo</a>)</li>
        </ul>
      </li>
      <li>Extract the contents of each zip file to your <i>libraries</i> directory. The end result should look something like this: <a href="#"><img src="https://i.imgur.com/mvSVOcH.png"></a></li>
    </ol>
  </li>
  <li>To upload data to SPIFFS, follow the steps on the <a href="https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system">ESP8266 filesystem documentation page</a>.</li>
</ul>

<h2>Sonoff Flashing Notes</h2>
<p>Out of the box the Sonoff Basic R2 comes with <a href="https://www.itead.cc/">ITEAD</a>'s (the manufacturer) firmware, which connects to servers in China to allow you to use your phone/tablet/etc to control the device remotely without opening up ports in your firewall or using a VPN. People in the home automation community have expressed concerns regarding the device being remotely controllable, which could present security risks, and for that reason (in addition to increased functionality such as control via MQTT and other options that are added), I have flashed the Sonoff with the <a href="https://github.com/arendst/Tasmota">Tasmota</a> firmware.</p>
<p>By flashing the device with Tasmota, it will no longer contact the servers in China to receive commands, but a side effect of this is that you will no longer be able to use the eWeLink app (<a href="https://play.google.com/store/apps/details?id=com.coolkit&hl=en_US"></a>Google Play</a>, <a href="https://apps.apple.com/us/app/ewelink-smart-home-control/id1035163158">Apple App Store</a>) to control it. You <i>can</i> still control it by either connecting it to a WiFi access point (I've configured it to automatically connect to the access point on the ESP8266) or by performing a factory reset (holding the button for 20 seconds) and connecting to it directly (it will present itself as an access point named <i>tasmota-####</i> where <i>####</i> is a random 4-digit number) and browsing to its default IP address, http://192.168.4.1.</p>
<p>Alternatively you can <a href="https://blog.hobbytronics.pk/sonoff-original-firmware-backup-restore/">restore the stock firmware</a> if you want that, but ultimately the choice is yours.</p>
<p>The initial flashing process from the stock firmware to Tasmota or from Tasmota back to the stock firmware requires a serial adapter like the one I've included, but subsequent flashings from one version of Tasmota to another can be performed through the device's web interface (you can use the client list I incorporated on the ESP8266 or if you do reset it, check your router's DHCP table).</p>

<h2>Closing</h2>
<p>If you have any questions, I'll gladly help. Youtube and Google are great resources, as is the Reddit community (but you probably already know that)!</p>

