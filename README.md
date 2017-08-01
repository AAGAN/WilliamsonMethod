### WilliamsonMethod
## c++ implementation of Williamson's method


Use g++ (GNU c++ compiler) instead of gcc (GNU c or c++) when linking files. Otherwise main function doesn't recognize the functions defined in other cpps.



# Property file:

Agent class defined. Each physical property is a vector read in from corresponding agent property file.

Each agent is read in from its specific file.
The current data file used for Halon is from Williamson paper and that for Novec is from Tom.
For Halon 1301 the Williamson paper has its Henry's constant data, but for Novec 1230 Tom assumes Henry's constant is linear with temperature in VB code, and we are currently using the same expression ( C_henry = 0.9286*T(F) + 3494.6 ).
Verification cases simply using a constant value for Henry's Law constant show that it's not affecting the pressure recession table.

Data file should be in English units, but initial storage conditions and pipe starting conditions need to be input with SI units in main function. Transition in williamson implementation to English units to be consistent with the paper.
Output can choose to be in English or SI units.


Temperature unit in entropy is Rankine, but 1 degree change in Rankine is the same as 1 degree in Fahrenheit.





# Pressure convergence:

Pressure convergence threshold is now a private member of the williamson class.
Initialization gives it a relatively large number. User has the option to set it.
For Halon P_thres can use 0.01 psi or even lower, and resultant tank state is not threshold dependent. For Novec it needs around 0.3 psi.

Converging pressure is a flimsy process of finding the pressure solution of the coupled equations. Works now but not sure whether it would fail for new agents or storage conditions. Always need to test threshold choice.
Now using a pressure increment (P_inc) different from (typically smaller than) threshold. This increment is allowed to be different for tank and pipe. Also consider changing it if not able to converge.

In the current pressure convergence implementation since pressure is incremented and updated before threshold check, the true convergence threshold would actually be the summation of (threshold + increment).

Convergence for an agent and initial conditions really depends on this combination of threshold and increment. Bad choice of either one could lead to divergence or infinite loop.

It seems the temperature at which the agent depletes could depend on the threshold. Theoretically it converges more precisely with smaller threshold, but it turns out that if the threshold is too small the solution would even diverge.
Therefore using a lower threshold may not always be desirable. Some step is not able to converge as accurate as asked.
Use the lowest threshold that converges.

It is reassuring that the tank state discharge curve (pressure vs. precent outage) does not change with the threshold.



Williamson Method exit code:
* 0: calculation finishes successfully
* 1: input temperature outside range
* 2: not converging





Tank discharge calculation assumes unit volume and conservations of mass, volume and entropy, while pipe expansion calculation assumes unit agent mass and conservations of mass and enthalpy.
Other than the difference in equations the solution methodology is the same.





Tank state can now be accessed through a given temperature or percentage of discharge.





Verbose flag default as False.
User can choose to turn on verbose in order to see detailed information during williamson calculation, including: input temperature interpolation, pressure at each convergence iteration, and the converged state at each temperature step.
Verbose messages can be too long, especially for the convergence steps, that the print screen cannot show all.





The calculated pressure recession matches Figure H.1(b) of NFPA 12A for Halon 1301.
The slight difference could be due to many reasons:
* Unlike in the paper which uses a linear relation between liquid entropy and temperature, directly use the liquid entropy data.
* Different pressure convergence criterion and threshold value
* Precision difference








# Caveats:

* Change the agent file name, molecular weight ratio and liquid volume expansion coefficient, as well as the storage conditions (starting P, T, D) for each run.

* Molecular weight ratio is 0.188 in paper but really needs to be changed depending on the agent.
* Also needs to be changed for different agents is the empirical coefficient for the effect of dissolved nitrogen on the liquid volume. It's 0.053 for Halon according to Williamson and 0.0429 for Novec according to Tom.


* Storage temperature for tank and starting temperature for pipe don't have to exist in data file.
* If temperature is starting from a mid-range temperature then it's linearly interpolated. Program aborts if temperature is outside range.

* The input used for pipe starting conditions also does not have to exist in the container recession table!!

* Agent property file needs to be prepared with data in a temperature-ascending order.
* It also must finish with the line for the highest temperature and cannot start with a new line at the end.

* For the four print one state functions, the state should be passed into the print function with corresponding units. User is responsible to guarantee this.

