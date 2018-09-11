# A full stack indoor localization implementation with the help of a Nordic device, various sensors and signals from bluetooth beacons.
 
This project contains the following files which we enumerate and properly document for use by anyone:
1. Firmware: This file contains the source code of the firmware developed. In order for someone to execute it, other than creating a new project on the os.mbed.com he should also add some common libraries for mbed projects like ble(https://github.com/ARMmbed/ble) and mbed. We note we just post the source code as the whole project is several MBs.
2. Cloud: This file, contains the flask script we ran on the cloud that creates are output result ready to be editted by a script. 
3. Scripts: This file, contains the python script responsible for all the editing and the output .csv file. It reads as input from 2 possible files (training or real_tests) and outputs its results at results.csv

The only thing misssing is the Android app which is the zipped file contains the android application we created to handle the data from the board and upload them alongside with ground truth collection, in proper format to the cloud. The reason it is not uploaded is because it was mainly created by another team member.
