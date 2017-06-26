# WilliamsonMethod
c++ implementation of Williamson's method


Use g++ (GNU c++ compiler) instead of gcc (GNU c or c++) when linking files. Otherwise main function doesn't recognize the functions defined in other cpps.


Agent class defined. Each physical property is a vector read in from corresponding agent property file. agent.cpp assumes temperature data in an ascending order.
Data file should be in English units, but initial storage conditions are input with SI units. Transition in williamson.cpp to English units to be consistent with the paper.
Storage temperature has to start from the highest temperature in data file! Should change later and do a temperature search, but the storage temperature should still exist in data file.

The output of the first part of the Williamson method would be a vector of tank_state structures.
percent_discharge is not well-defined yet.
Currently assuming Henry's constant to be linear with temperature with the same expression for all agents.

Molecular weight ratio is 0.188 in paper but really depends on the agent.

Calculation assumes unit volume.

Unlike in the paper which uses a linear relation between liquid entropy and temperature, directly use the liquid entropy data.