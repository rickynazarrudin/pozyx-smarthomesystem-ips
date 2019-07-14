# Pozyx Smart Home System - Indoor Positioning System Based
Elderly tends to have a high degree of dependency to survive. The rate of dependence on Indonesian elderly in 2017 is at 13.28%. This can be a problem if the elderly do not have people who are always ready to accompany them at any time. Actually to overcome these problems, a development of a robotic service and vital sign based system has been carried out. However, the developed system has not been able to monitor the elderly to the level of activity carried out by them. Therefore, in this Final Project, a design is carried out regarding the smart home system for seniors using sensors and actuators. This smart home system is composed of five sub-systems, including: indoor positioning system, database system, data processing system, actuator system and android application user interface.

# 1. Indoor Positioning System (IPS)
- This sub-system is responsible for gathering elderly's data through a sensor called pozyx. Pozyx sensor will give us data about the position of the elderly in 2D (x,y) coordinate form. There are 3 main things that has to be considered carefully when we want to implement indoor positioning system. 

	(a) Technology we use: in this project, i am going to use pozyx sensor, which is working by using radio frequency signal called 	ultrawideband. I am using ultrawideband because it provides me the most precise result (up to 10 cm) compare to the others technology like RFID, WiFi, Bluetooth, GPS, etc. 

	(b) The method we use: I am choosing lateration method and time of arrival. These methods are the most suitable way for ultrawideband to maximize indoor positioning of an object. 

	(c) IPS components, we have to prepare these three things before we could do indoor positioning: Map, tag device, and anchors.

After getting all the data, this sub-system must parse those data to the database system. So we can play more with it!

# 2. Database System
- This sub-system used by the main system for storing all the raw data. Raw data is received data from indoor positioning system and didn't get any process yet by the system. In this project i am using MySQL database as my storage system. The figure below represents how i construct the database.


# 3. Data Processing System
- There is a reason why i do not use the data resulted by indoor positioning system directly. If i use the data directly, it will lead me into invalid calculations or monitoring. So before i use those data, i have to filter it in order to clean the data from noise of radio frequency usage. In this project, i am using average filter, since i do not really need a complex filter, i think average filter is simple and good enough for this project.
- Done with filtering business, i can go to the next step which is the main function of the system, estimating elderley's distance coverage activity. To get this job done, i am using euclidean distance.


# 4. Actuator System
- This sub-system is responsible for the door automation task. In order to do that, this system must be able to read current position of the elderly. When the elderly is detected walking or standing around door area, so i can simply assume it that they want to open it. Then this system will do the task.

# 5. Android Application UI
- Last but not least, i also provide application user interface based on android OS for another user of this system. Another user of this system is elderly's family or maybe their private doctor.
