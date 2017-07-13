#include "williamson.hpp"
#include "agent.hpp"
#include <iostream>
#include <stdlib.h>     // abort
#include <cmath>        // abs
#include <algorithm>    // std::lower_bound


// ************************************************************
// Williamson method implementation part 1
// ************************************************************

int williamson(agent *extinguishing_agent, int agent_type, std::vector<tank_state> &Tankstate, double P, double T, double D)
{
  //! Convert storage conditions from SI units to English units
  P = P / 6894.76;                  // convert from Pa to psi
  T = (T - 273.15)*(9/5.0) + 32;    // convert from kelvin to fahrenheit
  D = D * 0.062428;                 // convert from kg/(m^3) to lb/(ft^3)
  
  
  
  double molecular_weight_ratio,      // molecular weight ratio of nitrogen vs agent
         coeff_dissol_expan;          // effect of dissolved nitrogen on the liquid volume
  
  //! Set molecular weight ratio and coefficient of dissolved volume expansion according to agent type
  if (agent_type == 1) // Halon
  {
    molecular_weight_ratio = 0.188;
    coeff_dissol_expan = 0.053;
  }
  else if (agent_type == 2) // Novec
  {
    molecular_weight_ratio = 0.088636;
    coeff_dissol_expan = 0.0429;
  }
  else // Neither Halon nor Novec. Bad input. Abort the program.
  {
    std::cout << "Wrong agent indicator!" << std::endl;
    abort();
  }
  
  
  //! Accessing agent properties
  std::vector<double> temperature = extinguishing_agent->get_temperature();
  std::vector<double> vapor_p = extinguishing_agent->get_vapor_p();
  std::vector<double> liquid_spec_vol = extinguishing_agent->get_liquid_spec_vol();
  std::vector<double> vapor_spec_vol = extinguishing_agent->get_vapor_spec_vol();
  // std::vector<double> liquid_enthal = extinguishing_agent->get_liquid_enthal();
  // std::vector<double> vapor_enthal = extinguishing_agent->get_vapor_enthal();
  std::vector<double> liquid_entro = extinguishing_agent->get_liquid_entro();
  std::vector<double> vapor_entro = extinguishing_agent->get_vapor_entro();
  std::vector<double> c_henry = extinguishing_agent->get_c_henry();
  
  
  
  // Assume the property data file is in integer temperature and drops one degree F each line!!
  // Find the integer temperature in data file closest to the storage temperature and starts from there.
  int count_T = round(temperature[0] - T); // count the temperature step number
  // If data is not degree by degree in temperature then attempt to do a temperature search.
  // std::vector<double>::reverse_iterator T_iter = std::lower_bound (temperature.rbegin(), temperature.rend(), T);
  // int count_T = temperature.rend() - T_iter - 1;
  
  // If storage temperature is outside the temperature range of data file, abort the program.
  if ((T < temperature.back()) || (T > (temperature.front()+1)))
  {
    std::cout << "Storage temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  //! Initialize the tank state
  tank_state current_state; // define a current tank state
  current_state.temperature = temperature[count_T];
  current_state.pressure = P + vapor_p[count_T];
  current_state.discharge = 0.0;
  current_state.liquid = (1-D*vapor_spec_vol[count_T]) / (liquid_spec_vol[count_T]*(1+coeff_dissol_expan*P/c_henry[count_T]) - vapor_spec_vol[count_T]); // assuming unit volume!!
  current_state.vapor = D - current_state.liquid;
  current_state.n_pressure = P;
  current_state.liquid_density = (1+0.01*P/c_henry[count_T]) / (liquid_spec_vol[count_T]*(1+coeff_dissol_expan*P/c_henry[count_T]));
  current_state.percent_discharge = 0.0;
  Tankstate.push_back(current_state); // Record the initial tank state
  count_T++;

  
  
  //! Loop through all temperatures
  /**
   * Solve for the discharged weight and pressure at each temperature step through the conservations of mass, volume and entropy.
   */
  for ( ; count_T < temperature.size(); count_T++)
  {
    double N, L1, L2, V2, P2, P3, E1, E5, E6, R1, R2, R3, A1, A2, A3; // intermediate variables
    
    N = current_state.liquid*0.01*current_state.n_pressure/c_henry[count_T-1] + current_state.vapor*molecular_weight_ratio*current_state.n_pressure/vapor_p[count_T-1]; // weight of nitrogen in container
    P2 = current_state.n_pressure - 1;    // assumed new nitrogen pressure after discharge
    P3 = P2 - 1;                          // random initialization for P3
    
    // Intermediate parameters
    E1 = (1 + coeff_dissol_expan*current_state.n_pressure/c_henry[count_T-1]) * liquid_entro[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry[count_T-1];
    R1 = (1 + molecular_weight_ratio*current_state.n_pressure/vapor_p[count_T-1]) / E6;
    A1 = R1*E1 - (1 + current_state.n_pressure/vapor_p[count_T-1]) * vapor_entro[count_T-1];
    
    //! Converging pressure at one step
    double P_thres = 0.05;                          // pressure threshold (psi)
    while ( std::abs(P3 - P2) > P_thres )           // loop if pressure difference is larger than threshold
    {
      E5 = (1 + coeff_dissol_expan*P2/c_henry[count_T]) * liquid_spec_vol[count_T];
      R2 = (1 + 0.01*P2/c_henry[count_T]) / E6;
      R3 = (1 + molecular_weight_ratio*P2/vapor_p[count_T]) / E6;
      A2 = R2*E1 - (1 + coeff_dissol_expan*P2/c_henry[count_T]) * liquid_entro[count_T];
      A3 = R3*E1 - (1 + P2/vapor_p[count_T]) * vapor_entro[count_T];
      
      L2 = (current_state.vapor*A1*vapor_spec_vol[count_T] - A3) / (A2*vapor_spec_vol[count_T] - E5*A3);  // new liquid weight
      V2 = (1 - L2*E5) / vapor_spec_vol[count_T];                                                         // new vapor weight
      L1 = current_state.liquid + current_state.vapor - L2 - V2;                                          // incremental liquid weight
      P3 = (N - L1*0.01*current_state.n_pressure/c_henry[count_T-1]) / (L2*0.01/c_henry[count_T] + V2*molecular_weight_ratio/vapor_p[count_T]); // calculated new nitrogen pressure after discharge
      
      // Adapt P2 towards convergence
      // if (P3 < (P2-P_thres*5)) P2 = P2 - P_thres*10;
      // else if (P3 < (P2-P_thres)) P2 = P2 - P_thres;
      if (P3 < (P2-P_thres))        P2 = P2 - P_thres;
      else if (P3 > (P2+P_thres))   P2 = P2 + P_thres;
      else ; // do nothing
      
      // std::cout << count_T << std::endl;
    }
    
    // std::cout << P2 << ' ' << P3 << std::endl;
    //! Update current container state
    current_state.temperature = temperature[count_T];
    current_state.pressure = P2 + vapor_p[count_T];
    current_state.discharge = current_state.discharge + L1;
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.liquid_density = (1+0.01*P2/c_henry[count_T]) / (liquid_spec_vol[count_T]*(1+coeff_dissol_expan*P2/c_henry[count_T]));
    current_state.percent_discharge = current_state.discharge / D;
    
    //! Make sure there is still liquid agent in container
    // Tankstate.push_back(current_state);
    if (L2 >= 0.0) // Still liquid left
    {
      Tankstate.push_back(current_state);
    }
    else
    {
      std::cout << std::endl << "Liquid agent depleted at temperature " << current_state.temperature << "(F)" << std::endl << std::endl;
      return 0;
    }
    
  
  }

  return 0;
}





// ************************************************************
// Williamson method implementation part 2 (overloading)
// ************************************************************

int williamson(agent *extinguishing_agent, int agent_type, std::vector<pipe_state> &Pipestate, double P, double T)
{
  // assume the pressure and temperature read in here are already in English units
  
  
  double N;                           // nitrogen weight
  
  double molecular_weight_ratio,      // molecular weight ratio of nitrogen vs agent
         coeff_dissol_expan;          // effect of dissolved nitrogen on the liquid volume
  
  //! Set molecular weight ratio and coefficient of dissolved volume expansion according to agent type
  if (agent_type == 1)                    // Halon
  {
    molecular_weight_ratio = 0.188;
    coeff_dissol_expan = 0.053;
  }
  else if (agent_type == 2)               // Novec
  {
    molecular_weight_ratio = 0.088636;
    coeff_dissol_expan = 0.0429;
  }
  else                                    // Neither Halon nor Novec. Bad input. Abort the program.
  {
    std::cout << "Wrong agent indicator!" << std::endl;
    abort();
  }
  
  
  //! Accessing agent properties
  std::vector<double> temperature = extinguishing_agent->get_temperature();
  std::vector<double> vapor_p = extinguishing_agent->get_vapor_p();
  std::vector<double> liquid_spec_vol = extinguishing_agent->get_liquid_spec_vol();
  std::vector<double> vapor_spec_vol = extinguishing_agent->get_vapor_spec_vol();
  std::vector<double> liquid_enthal = extinguishing_agent->get_liquid_enthal();
  std::vector<double> vapor_enthal = extinguishing_agent->get_vapor_enthal();
  // std::vector<double> liquid_entro = extinguishing_agent->get_liquid_entro();
  // std::vector<double> vapor_entro = extinguishing_agent->get_vapor_entro();
  std::vector<double> c_henry = extinguishing_agent->get_c_henry();
  
  
  
  // Since the temperature from recession table comes from the data temperature and therefore exists, no need for temperature search and check and directly find its position.
  int count_T = round(temperature[0] - T); // count the temperature step number
  
  //! Initialize the pipe state
  pipe_state current_state;                           // define a current pipe state
  N = 0.01*P/c_henry[count_T];                        // nitrogen weight does not change at each step
  current_state.temperature = temperature[count_T];
  current_state.pressure = P + vapor_p[count_T];
  current_state.liquid = 1;                           // expansion starts with unit mass of liquid agent and no vapor
  current_state.vapor = 0; 
  current_state.n_pressure = P;                       // share tank nitrogen pressure at pipe beginning
  current_state.density = (1+N) / (liquid_spec_vol[count_T]*(1+coeff_dissol_expan*P/c_henry[count_T])); // initial density (only liquid exists)
  Pipestate.push_back(current_state);                 // Record the initial pipe state
  count_T++;

  
  
  //! Loop through all temperatures
  /**
   * Solve for the pressure and density at each temperature step through the conservations of mass and enthalpy.
   */
  for ( ; count_T < temperature.size(); count_T++)
  {
    double L2, V2, P2, P3, E1, E2, E6, E7, E3, E4, E8, E9; // intermediate variables
    P2 = current_state.n_pressure - 1;    // assumed new nitrogen pressure after discharge
    P3 = P2 - 1;                          // random initialization for P3
  
    // Intermediate parameters
    E1 = (1 + coeff_dissol_expan*current_state.n_pressure/c_henry[count_T-1]) * liquid_enthal[count_T-1];
    E2 = (1 + current_state.n_pressure/vapor_p[count_T-1]) * vapor_enthal[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry[count_T-1];
    E7 = 1 + molecular_weight_ratio*current_state.n_pressure/vapor_p[count_T-1];
    
    
    //! Converging pressure at one step
    double P_thres = 0.1;                           // pressure threshold (psi)
    while ( std::abs(P3 - P2) > P_thres )           // loop if pressure difference is larger than threshold
    {
      E3 = (1 + coeff_dissol_expan*P2/c_henry[count_T]) * liquid_enthal[count_T];
      E4 = (1 + P2/vapor_p[count_T]) * vapor_enthal[count_T];
      E8 = 1 + 0.01*P2/c_henry[count_T];
      E9 = 1 + molecular_weight_ratio*P2/vapor_p[count_T];
      
      L2 = (current_state.liquid*(E1*E9-E6*E4) + current_state.vapor*(E2*E9-E7*E4)) / (E3*E9-E8*E4);  // new liquid weight
      V2 = 1 - L2;                                                                                    // new vapor weight
      P3 = N / (L2*0.01/c_henry[count_T] + V2*molecular_weight_ratio/vapor_p[count_T]);               // calculated new nitrogen pressure after expansion
  
      // Adapt P2 towards convergence
      if (P3 < (P2-P_thres))        P2 = P2 - P_thres;
      else if (P3 > (P2+P_thres))   P2 = P2 + P_thres;
    }
    
    
    //! Update current pipe state
    current_state.temperature = temperature[count_T];
    current_state.pressure = P2 + vapor_p[count_T];
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.density = (1+N) / (L2*liquid_spec_vol[count_T]*(1+coeff_dissol_expan*P2/c_henry[count_T]) + V2*vapor_spec_vol[count_T]);
    
    //! Store the current state
    Pipestate.push_back(current_state);
  
  }
  
  
  return 0;
}
