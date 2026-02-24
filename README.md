This is intended to run on a STM32F446 evaluation board plugger on the SProc interface board.

To do signal processing, edit file app.c, in particular function app_adc() that 
basically copy incoming readings to the DACs
