A fatal error occurred: Could not open /dev/ttyUSB0, the port doesn't exist
*** [upload] Error 2
========================================================================= [FAILED] Took 3.67 seconds =========================================================================

 *  The terminal process "platformio 'run', '--target', 'upload', '--environment', 'esp32dev'" terminated with exit code: 1. 
 *  Terminal will be reused by tasks, press any key to close it. 



Solution:
`sudo usermod -a -G dialout $USER`
