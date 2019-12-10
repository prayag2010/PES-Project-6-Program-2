# PES Project 6 Readme


**Team Members:** 
- Prayag Milan Desai
- Rahul Ramaprasad 


---
**Extra Credits:**  
Implemented FreeRTOS queue to store the ADC values and transferred using the DMA. Also implemented a mutex in order to lock the LED resource while blinking at the begining of DMA transfer.  

**Description of repository contents:**  
>This repository contains ten folders, each having multiple source and header files. The description of the folders are as follows:  

**Folder board:**  
This folder contains the library and source files related to the KL25Z FRDM board which have pin init functions and configuration functions. 

**Folder CMSIS:**  
This folder contains source and header files which provide interfaces to processor and peripherals, real-time operating systems, and middleware components.

**Folder drivers:**  
This folder contains drivers for clock, gpio, communication protocols, touch sensor, timer, flash,etc.

**Folder utilities:** 
This folder contains source and header file which facilitate the debugging process.

**Folder startup:**
This folder contains source and header file for the boot process of the freedom board.

**Folder source:** 
This folder contains source files which are common to all build configurations and are compiled based on the macro definitions. The application is made
modular by defining multiple source files. The PES Project 4.c is defined here, which has the main() function and  is compiled 
for all targets.

**Folder Debug:**
This folder contains the debug target which are created after the compilation of all the source files. This folder also contains the output of the compilation process which is ***'.axf'*** file. The '.axf' file will be executed on the FRDM board.

**Folder Release:**
This folder contains the release target, which are created after the compilation of all the source files. This folder also contains the output of the compilation process which is ***'.axf'*** file. The '.axf' file will be executed on the FRDM board.  

**Folder FreeRTOS:**  
This folder contains the FreeRTOS library functions. Source files such as Task.c, Semphr.c, Queue.c and FreeRTOS.c are included in this folder.


---

**Installation/execution notes for the C code in this repository:**  
All the code in this repository should be executed using the following command:  
`make -r -j3 "target"`  
Where, "target" has three choices
- *Debug*: This target builds the output for the KL25Z. The output LEDs will change according to the states and the messages will be logged.   
- *Release*: This target builds the output for the KL25Z. The output LEDs will change according to the state changes, and a log output is displayed (The Maximum, Minimum, Average and Standard Deviation values are displayed).  

   

Always run the 'make clean' command before rebuilding other targets.  
Executing the above command results in .axf file which can be executed on FRDM board by clicking the debug option.  
This project has the following requirements, that are fulfilled using the aforementioned command:  
- ARM-GCC cross-compiler : The 'arm-none-eabi-gcc' command is used to compile the FRDM board target code
- Macro flag : The '-D $(target_macro_definition)' to compile the target specific sections of code 
- No errors and warnings: The -Wall flag enables a set of warnings, and the -Werror flag treats the warnings as errors. Since the output of the compiler shows no error, the code in this repo is without any compiler warnings and errors  


**Issues faced**  
The values from the sine lookup table were not scaled properly, hence the lookup table generator equation was changed in order to feed the right values into the DAC. Tried using TaskResumeFromISR() function to trigger a FreeRTOS task from DMA Interrupt Handler. This gave a lot of issues and since we could not get around that, we switched to using the callback function to trigger the task.  





