### WilliamsonMethod
## c++ implementation of Williamson's method


Use g++ (GNU c++ compiler) instead of gcc (GNU c or c++) when linking files. Otherwise main function doesn't recognize the functions defined in other cpps.



Agent class defined. Each physical property is a vector read in from corresponding agent property file.

Each agent is read in from its specific file.
The current data file used for Halon is from Williamson paper and that for Novec is from Tom.
For Halon 1301 the Williamson paper has its Henry's constant data, but for Novec 1230 Tom assumes Henry's constant is linear with temperature in VB code, and we are currently using the same expression.
Verification cases simply using a constant value for Henry's Law constant show that it's not affecting the pressure recession table.


Agent property file needs to be prepared with data in a temperature-ascending order.


Data file should be in English units, but initial storage conditions are input with SI units. Transition in williamson implementation to English units to be consistent with the paper.
Output can choose to be in English or SI units.


Temperature unit in entropy is Rankine, but 1 degree change in Rankine is the same as 1 degree in Fahrenheit.





Tank discharge calculation assumes unit volume and conservations of mass, volume and entropy, while pipe expansion calculation assumes unit agent mass and conservations of mass and enthalpy.
Other than the difference in equations the solution methodology is the same.





The calculated pressure recession matches Figure H.1(b) of NFPA 12A for Halon 1301.
The slight difference could be due to many reasons:
* Unlike in the paper which uses a linear relation between liquid entropy and temperature, directly use the liquid entropy data.
* Different pressure convergence criterion and threshold value
* Precision difference








# Caveats:

* Change the agent file name, molecular weight ratio and liquid volume expansion coefficient, as well as the storage conditions (starting P, T, D) for each run.

* Molecular weight ratio is 0.188 in paper but really needs to be changed depending on the agent.
* Also needs to be changed for different agents is the empirical coefficient for the effect of dissolved nitrogen on the liquid volume. It's 0.053 for Halon according to Williamson and 0.0429 for Novec according to Tom.


* Storage temperature for tank and starting temperature for pipe has to exist in data file.
* If temperature is starting from a mid-range temperature then it's rounded to the closest temperature in data and start from there. Need to switch to a temperature search if the temperature in data is not integer.


* It seems the temperature at which the agent depletes depends a lot on the threshold. Theoretically it converges more precisely with smaller threshold, but it turns out that if the threshold is too small the solution would even diverge.
The threshold also depends on initial conditions.
It is reassuring that the tank state discharge curve (pressure vs. precent outage) does not change with the threshold.


* Still need to decide on the method of input to be used for pipe starting conditions, so that it starts from an existing state in the recession table!!


* Agent property file must finish with the line for the highest temperature and cannot start with a new line at the end.


