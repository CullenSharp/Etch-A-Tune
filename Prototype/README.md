#Current Prototype

Included in this folder are breadboard prototypes for Etch-A-Tune

## Log
### Date: 11-5-2025
Hardware was passed out the week of November 3rd 2025, so real progress can be made now.  

The screen is connected to one of the dev boards in the same family as the final chip, but a different sub SKU. Running on the board is the LCD test example code found here: https://github.com/RobertoBenjami/stm32\_hal\_graphics\_display\_drivers  


It currently doesn't display correctly, but the noise on the display implies that the communication is half working. We will currently be working on debugging the project above and implement portions into our final program.  

We will also be working on the IFFT in order to convert bit maps into waveforms, ideally the same way as spectrograms are produced and interpreted. 

### Date: 11-7-2025
The initialization function for the LCD driver chip driver was rewritten by Cullen Sharp. We are able to run an example program and have a framework for how to develop the program in terms of the rest of the code

Touch has been confirmed working with the default ada\_fruit\_ts.h and c. Calibration was added and put into ada\_fruit\_ts.c. Paint is current example code from mcma branch



