#### Williamson Method

Based on the 1985 paper "Calculating H-1301 discharge pressures and densities" by H.V.Williamson, this theory calculates the container and pipe states during the instantaneous discharge of super-pressurized fire extinguishing agent (liquid) propelled by inert gas (N2).

Complex thermodynamics is going on inside the system:
Liquid will boil in the tank during discharge due to the super saturation effect from nitrogen.
Liquid-vapor two-phase flow inside pipeline.
This is a 1-D model that does not consider the hydraulics.

Specifically it consists of two parts:
1.: Pressure recession in the storage container during discharge.
2.: Pressure and density in the piping for specific increments of liquid leaving the storage container.

Refer to that paper for details and equations of this theory, which is based on Halon.
This code is capable to generalize to other futuristic agents or inert gases, and all equations would still hold. Only need to change the molecular weight ratio of inert gas vs agent (0.188), coefficient for the expansion effect of dissolved inert gas on the liquid volume (0.053), as well as the corresponding property data file.



# Limitations:

* Container state calculation is valid until the point when all liquid agent has been discharged and the pipeline begins to fill with vapor.
The theory is applicable to the whole pipeline calculation only starting from when the firstly discharged agent reach the nozzle and the whole pipe is filled with agent. Before that point pipe state can still be calculated for the part that's reached by discharged agent, but this is due to not considering the hydraulics.
So generally this theory fully reflects the container-pipe system only from when pipe is filled to the point when liquid agent depletes.

* Tank discharge calculation assumes unit volume and conservations of mass, volume and entropy, while pipe expansion calculation assumes unit agent mass and conservations of mass and enthalpy. Other than the difference in coupled equations the solution methodology is the same.
Isentropy (constant entropy) for discharge requires the process to be rapid so that the heat exchange from outside can be omitted (adiabatic) and no energy loss so that it is reversible (frictionless).
Isenthalpy (constant enthalpy) for pipeline means no energy in the form of work or heat is exchanged with the environment during agent expansion in pipe.





## C++ implementation of Williamson's method and How to use it

* STEP 1:
Type "g++ agent.cpp williamson.cpp test.cpp" to generate an executable that runs the program.
Use g++ (GNU c++ compiler) instead of gcc (GNU c or c++) when linking files. Otherwise main function doesn't recognize the functions defined in other cpps.

* STEP 2:
Specify the agent property file name and values for molecular weight ratio and liquid volume expansion coefficient due to solute.
Call the williamson constructor with them and create a williamson class instance.

* STEP 3:
Define the initial storage nitrogen partial pressure (not the total container storage pressure), temperature and agent filling density. Feed them (in English units) into "tank" method of the williamson instance for part 1.
Refer to williamson.hpp for the tank_state structure defined as a container for all physical quantities at a state.
Multiple printers, writers and accessors are implemented. Also refer to williamson.hpp on what is available and how to use them. Specifically tank state snapshot can be taken at a given temperature or percentage of discharge.

* STEP 4:
Define the nitrogen partial pressure and temperature of the tank state to be used in part 2 "pipe" method.
Similarly there are pipe state structure and printers, writers and accessors ready for use.

* Other:
Specify a pressure convergence criterion. The best shot for accuracy is testing various criteria to find the lower bound and using the lowest criterion that converges. Details about this will follow.

A verbose flag has default as False.
User can choose to turn on verbose in order to see detailed information during williamson calculation, including: input temperature interpolation, pressure at each convergence iteration, and the converged state at each temperature step.
Verbose messages can be too long, especially for the convergence steps, that the print screen cannot show all.



Williamson Method exit code:
* 0: Calculation finishes successfully
* 1: Input temperature outside range - bad input
* 2: Not converging - change convergence criterion





## Property file:

Each agent is read in from its specific file, which has to obey the following format:

* All quantities in the file are in English units.

* Each row has 9 numbers as quantities at a temperature. In sequence and separated by commas they are: temperature (F), agent vapor pressure (PSI), liquid specific volume (cubic foot/pound), vapor specific volume (cubic foot/pound), liquid enthalpy (btu/pound), vapor enthalpy (btu/pound), liquid entropy (btu/pound/rankine), vapor entropy (btu/pound/rankine), henry's law constant (psi/weight percent).

* Agent property file needs to be prepared with data in a temperature-ascending order for the rows, starting from the lowest temperature and ending with the highest temperature data available. There is no requirement on the temperature increment between lines. (Not necessarily integer temperature or incrementing 1 degree each line.)

* It must finish with the line for the highest temperature and cannot start with a new line at the end.



After being read into williamson class, each physical property is a private vector.

The current data file used for Halon-1301 is from Williamson paper and that for Novec-1230 is from Tom.
For Halon the Williamson paper has its Henry's constant data, but for Novec Tom assumes Henry's constant is linear with temperature in VB code, and we are currently using the same expression ( C_henry = 0.9286*T(F) + 3494.6 ).
Verification cases simply using constant values (3400 and 3600) for Henry's Law constant show that it's not affecting the pressure recession table, so we can reasonably worry less about it.





## Core algorithm for the theory - Pressure convergence criterion:

In solving for the tank or pipe state at a temperature, a pressure value is firstly guessed. After plugging this value into those coupled equations the resultant pressure should be the same as the assumed pressure.
If they are not equal (not converged), adjusting the assuming pressure until convergence.
Due to numerical issue, the guessed and resultant values have trouble coming infinitely close sometime, so a criterion for convergence is needed. When the input/output pressure difference is within this small criterion, we accept it as converged.

Converging pressure is really a flimsy process of finding the pressure solution of the coupled equations.
Whether the calculation could converge as well as the temperature at which the agent depletes could both depend on this criterion.
Theoretically calculation converges more precisely with a smaller criterion, but it turns out that if the criterion is too small the solution would even diverge and reduce to negative value without finding a solution.
Therefore using a lower threshold may not always be desirable. Some step is just not able to converge as accurate as asked.


* Pressure convergence criterion (P_thres) is a private member of the williamson class.

* Constructor initialization gives it a relatively large number (0.5 psi). User has the option to set it.

* For Halon P_thres can use 0.01 psi or even lower, and for Novec it needs around 0.3 psi. These choices of criteria work for now but are likely to fail for new agents or storage conditions. Always need to test for the best choice.

* It is reassuring that the tank state discharge curve (pressure vs. precent outage) does not change with the criterion.



Other than the convergence criterion, the pressure increment (P_inc), which is the step adjustment when not converging, can also affect the convergence. Solution could infinitely loop between two values without converging for too large of this increment.

* Convergence for an agent and initial conditions really depends on this combination of threshold and increment. Bad choice of either one could lead to divergence or infinite loop.

* This increment is allowed to be different for tank and pipe. Now using a pressure increment equal to convergence criterion for tank and (P_thres/2.5) for pipe.

* These two choices are not necessarily the best, but we are not giving user a knob to change the increment in the interface, since the non-convergence can mostly be solved by tuning up the convergence criterion. With that said, higher-level user can consider going in the tank and pipe methods in williamson.cpp and changing increments if not able to converge.





## Caveats:

* Molecular weight ratio (N2/agent) is 0.188 in paper but that's just for Halon. It really needs to be changed depending on the agent.

* Also need to be changed for different agents is the empirical coefficient of liquid agent volume expansion from the effect of saturated and dissolved inert gas. It's 0.053 for Halon according to Williamson and 0.0429 for Novec according to Tom.

* Storage temperature for tank and starting temperature for pipe don't have to exist in data file.
If temperature is starting from a mid-range temperature then it's linearly interpolated. Program aborts if temperature is outside range.

* The input used for pipe starting conditions also does not have to exist in the container recession table. It will be interpolated from the resultant container recession table from "tank" method.

* Units: Data file should be in English units, but initial storage conditions and pipe starting conditions need to be input with SI units in main function. Transition in williamson implementation to English units to be consistent with the paper.
Output can choose to be in English or SI units.

* For the four print-one-state functions, the state should be passed into the print function with corresponding units. User is responsible to guarantee this.

* The code would usually still finish if given a large pressure convergence criterion. The result is not closely converged in this case and cannot be trusted.
It is therefore important for the user to test various criteria and eventually use the lowest that converges, or a low enough criterion in the range that no longer affects results!





## Miscellaneous:

The calculated pressure recession matches Figure H.1(b) of NFPA 12A for Halon 1301.
The slight difference could be due to many reasons:
* Unlike in the paper which uses a linear relation between liquid entropy and temperature, directly use the liquid entropy data.
* Different pressure convergence criterion and threshold value
* Precision difference





## Developing notes:

* Agent class defined, but users don't really need to interact with the agent class.
In fact the agent class might be unnecessary. The agent constructor can just be implemented in the williamson constructor.

* Temperature unit in entropy is Rankine, but 1 degree change in Rankine is the same as 1 degree in Fahrenheit. Therefore no need for units transfer of temperature in entropy.

* In the current pressure convergence implementation since pressure is incremented and updated before convergence check, the true convergence threshold would actually be the summation of (criterion + increment).


