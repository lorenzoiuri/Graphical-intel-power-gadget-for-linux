# Graphical intel power gadget for linux
A simple graphical front-end for the intel power gadget utility for linux

The official Intel power gadget for linux lacks a graphical interface unlike the windows and mac version (see <https://software.intel.com/en-us/articles/intel-power-gadget-20>).

## Features

<img align="right" width="227" height="482" src="https://raw.githubusercontent.com/lorenzoiuri/Graphical-intel-power-gadget-for-linux/master/res/linux1.png">

* customizable colors
* customizable window size
* customizable number of horizontal lines

<!--
<img align="left" width="302" height="632" src="https://raw.githubusercontent.com/lorenzoiuri/Graphical-intel-power-gadget-for-linux/master/res/linux2.png">
-->


## Requirements
* SDL2
* an Intel cpu (2nd generation onwards)

## How to use
1. Download and compile the Intel power gadget from [here](https://software.intel.com/en-us/articles/intel-power-gadget-20), if it doesn't compile try the version hosted in this repository ([here](https://github.com/lorenzoiuri/Graphical-intel-power-gadget-for-linux/raw/master/intel_own_gadget.tar.gz))
2. Rename the executable to "watts" and place it in `/usr/local/sbin/`, otherwise change the path in the source code of the `intel_power_gadget.c`

