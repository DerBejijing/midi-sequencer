# midi-sequencer
midi sequencer firmware for raspberry pico (moog 960-sequencer inspired)  
Inspired by moog, but turned out to look more like the MAQ16 lul  

## about  
This is a midi step sequencer inspired by the 960 made by moog in the 70s.  
Since getting to know it, I have always loved the moog IIIp modular synthesizer, which is why I decided to build my own version of the sequencer complement B.  
The cabinet holds two of those modules plus an additional "half-height" (actually about 60%) row, like the original one.  
During the building process, I diverged from my original design over and over again, and in the end, I even had to borrow some parts from the other sequencer in the cabinet.  

## features  
- 3 rows with eight steps  
- rows can be played in parallel or serial  
- the lengths of all rows can be adjusted independently  
- a variable amount of ratcheting can be set per step  
- speed is adjustable  
- all values are sent over midi (channels 0-3 in parallel mode, 0 in serial)  

<br/>


<img src="https://github.com/DerBejijing/midi-sequencer/blob/main/images/sequencer_front_rack.png" width=50% align=left margin-right=50px>  

TODO-List:  
---> add serial communication to controller  
---> find out how to make markdown add padding to a list next to an image >:(
