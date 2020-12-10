# t25-satellite
<p align="center">
  Atmel Studio C code for the attiny25v satellite sculpture<br />
  <img width="572" src="https://i.postimg.cc/GpshTbcs/t25-satellite.jpg" alt="SAT Tiny attiny25v">
</p>

<p>Based around Richard Sappia's SAT Tiny, I decided to build one but didn't want to the the Arduino IDE, preferring Atmel/Microchip Studio instead.
You can find Richard Sappia's version here: <a href="https://hackaday.io/project/174462-sattiny">https://hackaday.io/project/174462-sattiny</a></p>
<p>The idea of this mini project is to make two LEDs blink in a predefined pattern that looks similar to an exponential function given by an RC circuit, creating a smooth fading effect.</p>
<p>This is the code running on the SAT Tiny. Based on Richard Sappia's measurements, it has been observed that the current consumption remains around 4μA when sleeping. During the day, when there is enough light to load the super cap, the Attiny wakes up every 8 seconds to checkout if PB3 is still high. In absence of light, PB3 sees a logical 0. At this point, the wake up interval changes to 4 seconds.</p>

<h2>SAT Tiny schematic</h2>

<p align="center">
  <img width="509" src="https://i.postimg.cc/Yqkk21Gg/t25-satellite-schematic.jpg" alt="SAT Tiny schematic">
</p>

<p>&nbsp;</p>
<h2>SAT Tiny BOM</h2>
<p>
1x 4F, 5.5V super capacitor: <a href="https://www.amazon.com/TOVOT-Value-Capacitors-capacitor-Assortment/dp/B073RY8B4Z/">https://www.amazon.com/TOVOT-Value-Capacitors-capacitor-Assortment/dp/B073RY8B4Z/</a><br />
1x 0.8mm Brass Rod Set: <a href="https://kitkraft.com/products/1-32-0-8mm-round-brass-rod">https://kitkraft.com/products/1-32-0-8mm-round-brass-rod</a><br />
1x 1.2mm Brass Rod Set: <a href="https://kitkraft.com/products/3-64-1-2mm-round-brass-rod">https://kitkraft.com/products/3-64-1-2mm-round-brass-rod</a><br />
1x 1.6mm Brass Rod Set: <a href="https://kitkraft.com/products/1-16-1-6mm-round-brass-rod">https://kitkraft.com/products/1-16-1-6mm-round-brass-rod</a><br />
1x Attiny25V microcontroller: <a href="https://www.digikey.com/en/products/detail/microchip-technology/ATTINY25V-10PU/735463">https://www.digikey.com/en/products/detail/microchip-technology/ATTINY25V-10PU/735463</a><br />
4x AM-1417CA Solar Panels: <a href="https://www.digikey.com/en/products/detail/panasonic-bsg/AM-1417CA/2165185">https://www.digikey.com/en/products/detail/panasonic-bsg/AM-1417CA/2165185</a><br />
2x 3mm Red LEDs: <a href="https://www.digikey.com/en/products/detail/everlight-electronics-co-ltd/MV50640/2675591">https://www.digikey.com/en/products/detail/everlight-electronics-co-ltd/MV50640/2675591</a><br />
1x BAT42 Schottky Diode: <a href="https://www.digikey.com/en/products/detail/stmicroelectronics/BAT42/603877">https://www.digikey.com/en/products/detail/stmicroelectronics/BAT42/603877</a><br />
4x Axial 10nF capacitors: <a href="https://www.digikey.com/en/products/detail/kemet/C410C103M1U5TA/12701371">https://www.digikey.com/en/products/detail/kemet/C410C103M1U5TA/12701371</a><br />
1x 200K resistor: <a href="https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNF14FTD200K/1975172">https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNF14FTD200K/1975172</a><br />
1x 10K resistor: <a href="https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNF14FTD10K0/1975090">https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNF14FTD10K0/1975090</a><br />
1x 1K resistor: <a href="https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNF14FTD1K00/1706678">https://www.digikey.com/en/products/detail/stackpole-electronics-inc/RNF14FTD1K00/1706678</a><br />
1x ATTINY USB Programmer: <a href="https://www.digikey.com/en/products/detail/sparkfun-electronics/PGM-11801/5230948">https://www.digikey.com/en/products/detail/sparkfun-electronics/PGM-11801/5230948</a><br />
</p>
<p>Have fun!</p>
