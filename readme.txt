This file is the brief instruction how to launch the online display with emssh command
Start communication:
cd /home/huagen/ems/build/commu
./commu -nb


1. Start ems_control in one new terminal 
 Type ems_control (enter) and the ems control interface will pop up;
 Click init button to load settings;
 Perform step 2
 Click start button for data taking;
 Run step 3 to start event_distributor
 Follow step 4 for online display
after performing step 4, the online display chain should be established 
  and step 4 could be disconnected at any time
 Run step 5 to show histogram.

2.open a terminal
cd to /home/huagen/ems/build/events++
./cluster2event -verbose -asynchron -maxmem 10000 :12345 :3333

3. Open a termial to run event_distributor
cd to /home/huagen/ems/build/events
./event_distributor localhost:3333 5555

4.open a terminal 
cd to /home/huagen/code/cluster_decoding/Day1Online
./datacli localhost 5555


5.open a termial 
cd to /home/huagen/code/cluster_decoding/Day1Online
./hisplot
 



