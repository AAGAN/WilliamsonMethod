### WilliamsonMethod
## c++ implementation of Williamson's method


Use g++ (GNU c++ compiler) instead of gcc (GNU c or c++) when linking files. Otherwise main function doesn't recognize the functions defined in other cpps.



Agent class defined. Each physical property is a vector read in from corresponding agent property file. agent.cpp assumes temperature data in an ascending order.

Each agent is read in differently.
For Halon 1301 the Williamson paper has its Henry's constant data, but for Novec 1230 Tom assumes Henry's constant is linear with temperature in VB code.
Verification cases simply using a constant value for Henry's Law constant show that it's not affecting the pressure recession table.

Data file should be in English units, but initial storage conditions are input with SI units. Transition in williamson.cpp to English units to be consistent with the paper.

Temperature unit in entropy is Rankine, but 1 degree change in Rankine is the same as 1 degree in Fahrenheit.



The output of the first part of the Williamson method would be a vector of tank_state structures.



The calculated pressure recession matches Figure H.1(b) of NFPA 12A for Halon 1301.
The slight difference could be due to many reasons:
* Unlike in the paper which uses a linear relation between liquid entropy and temperature, directly use the liquid entropy data.
* Different pressure convergence criterion and threshold value
* Precision difference



# Caveats:

* Storage temperature has to start from the highest temperature in data file! Otherwise need to note and push_back into vectors.
Can change later and do a temperature search, but the storage temperature should still exist in data file.

* Tank discharge calculation assumes unit volume and pipe expansion calculation assumes unit mass.

* Molecular weight ratio is 0.188 in paper but really needs to be changed depending on the agent.
* Also needs to be changed for different agents is the empirical coefficient for the effect of dissolved nitrogen on the liquid volume.



