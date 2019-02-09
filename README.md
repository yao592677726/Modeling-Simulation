# CSE 6730 Project 0: Online store profit optimization based on discrete event simulation
### Author: Chengxi Yao(chengxi.yao@gatech.edu)

The files included in the simulation are:
sim.h		Interface between the simulation engine and the application
application.c	Sample simulation application modeling the service platform
engine.c	Simulation engine

1. To run this code, you need to open the terminal, switch to the directory of application.c and engine.c, then compile it using gcc:  

  ```c
  gcc application.c engine.c
  ```
2. After the file is compiled, the compiler will create an executable file named "a.exe" in the directory. To run this exe file, just type in the parameters of simulation. The usage is :
  ```c
  a.exe
  ```
  
3. After inputting all the parameters, the program will automatically run the engine and application. Results are printed in the terminal. For example:
```c
Average total profit: 648.262327, a = 1, b = 1, c = 1
Average total profit: 610.132699, a = 1, b = 1, c = 2
Average total profit: 547.035625, a = 1, b = 1, c = 3
Average total profit: 517.465596, a = 1, b = 1, c = 4
Average total profit: 465.507716, a = 1, b = 1, c = 5
Average total profit: 446.471061, a = 1, b = 1, c = 6
Average total profit: 597.167314, a = 1, b = 2, c = 1
Average total profit: 540.921179, a = 1, b = 2, c = 2
Average total profit: 517.302624, a = 1, b = 2, c = 3
Average total profit: 473.702304, a = 1, b = 2, c = 4
Average total profit: 475.635517, a = 1, b = 2, c = 5
Average total profit: 551.851357, a = 1, b = 2, c = 6
Average total profit: 529.666612, a = 1, b = 3, c = 1
Average total profit: 481.241330, a = 1, b = 3, c = 2
Average total profit: 482.132855, a = 1, b = 3, c = 3
Average total profit: 556.927566, a = 1, b = 3, c = 4
Average total profit: 720.498411, a = 1, b = 3, c = 5
Average total profit: 986.746964, a = 1, b = 3, c = 6
Average total profit: 597.932693, a = 2, b = 1, c = 1
Average total profit: 635.006203, a = 2, b = 1, c = 2
Average total profit: 670.002392, a = 2, b = 1, c = 3
Average total profit: 876.497155, a = 2, b = 1, c = 4
Average total profit: 1169.040350, a = 2, b = 1, c = 5
Average total profit: 1415.837459, a = 2, b = 1, c = 6
Average total profit: 672.072743, a = 2, b = 2, c = 1
Average total profit: 850.457412, a = 2, b = 2, c = 2
Average total profit: 1064.522619, a = 2, b = 2, c = 3
Average total profit: 1429.779126, a = 2, b = 2, c = 4
Average total profit: 1794.109198, a = 2, b = 2, c = 5
Average total profit: 2064.035142, a = 2, b = 2, c = 6
Average total profit: 1139.230594, a = 2, b = 3, c = 1
Average total profit: 1554.410837, a = 2, b = 3, c = 2
Average total profit: 1806.828091, a = 2, b = 3, c = 3
Average total profit: 2101.081377, a = 2, b = 3, c = 4
Average total profit: 2439.308559, a = 2, b = 3, c = 5
Average total profit: 2850.289356, a = 2, b = 3, c = 6
The best evening arrangement is 2 A-level agent, 3 B-level agent, 6 C-level agent, total_profit is 2850.289356
From 17:00 to 18:00:
 Income:1660.042200, Cost: 993.885150, Profit: 666.157050
From 18:00 to 19:00:
 Income:1950.229200, Cost: 1134.162690, Profit: 816.066510
From 19:00 to 20:00:
 Income:1880.755800, Cost: 1079.660780, Profit: 801.095020
From 20:00 to 21:00:
 Income:1338.855000, Cost: 828.711920, Profit: 510.143080



Statistics of Average Wait_Time: 45.601221 of 407 Guests



Statistics of incoming guests:
 In_Answered=0.048595
 In_Wait=0.033979
 In_Hangup=0.010251
 In_Selfserve=0.907175

```

