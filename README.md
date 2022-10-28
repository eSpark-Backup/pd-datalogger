# PD-DataLogger
 To develop a data logger that can be installed in Hospitals for logging environmental data and storing them onto the host system

## Process to build the project
1. Open keil project.
2. Go to options for target.
3. Select "Use default compiler" version from the target tab.
4. Select St-link debugger from the debug tab and click ok.
5. Build the project using F7.
6. Download the code to the board by pressing F8.

## Process to rebuild the project
1. If the code is regenerated through cubeMX replace the userdiskio.c file from the path \Libraries\SDCARD\ to the path \FATFS\Target.
2. Go to options for file(main.c) in keil and paste the path from misc. controls to include paths.
