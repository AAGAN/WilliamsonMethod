#include "williamson.hpp"
#include "agent.hpp"
#include <iostream>
#include <stdlib.h> // abort
#include <cmath> // abs


//! Williamson method implementation
int williamson(agent *extinguishing_agent, int agent_type, std::vector<tank_state> &Tankstate, double P, double T, double D)
{
  // Convert storage conditions from SI units to English units
  P = P / 6894.76; // convert from Pa to psi
  T = (T - 273.15)*9/5 + 32; // convert from kelvin to fahrenheit
  D = D * 0.062428; // convert from kg/(m^3) to lb/(ft^3)
  
  
  // Accessing agent properties
  std::vector<double> temperature = extinguishing_agent->get_temperature();
  std::vector<double> vapor_p = extinguishing_agent->get_vapor_p();
  std::vector<double> liquid_spec_vol = extinguishing_agent->get_liquid_spec_vol();
  std::vector<double> vapor_spec_vol = extinguishing_agent->get_vapor_spec_vol();
  std::vector<double> liquid_enthal = extinguishing_agent->get_liquid_enthal();
  std::vector<double> vapor_enthal = extinguishing_agent->get_vapor_enthal();
  std::vector<double> liquid_entro = extinguishing_agent->get_liquid_entro();
  std::vector<double> vapor_entro = extinguishing_agent->get_vapor_entro();
  std::vector<double> c_henry = extinguishing_agent->get_c_henry();
  
  
  //! Initialize the tank state
  // Assume the property data file starts with the storage temperature and drops one degree K each line!!
  tank_state current_state; // define a current tank state
  int count_T = 0; // count the temperature step number
  current_state.temperature = T;
  current_state.pressure = P + vapor_p[count_T];
  current_state.discharge = 0.0;
  current_state.liquid = (1-D*vapor_spec_vol[count_T])/(liquid_spec_vol[count_T]*(1+0.053*P/c_henry[count_T])-vapor_spec_vol[count_T]); // assuming unit volume!!
  current_state.vapor = D - current_state.liquid;
  current_state.n_pressure = P;
  current_state.liquid_density = (1+0.01*P/c_henry[count_T])/(liquid_spec_vol[count_T]*(1+0.053*P/c_henry[count_T]));
  current_state.percent_discharge = 0.0;
  Tankstate.push_back(current_state); // Record the initial tank state
  count_T++;
  
  
  
  double molecular_weight_ratio, N, L1, L2, V2, P2, P3, E1, E5, E6, R1, R2, R3, A1, A2, A3;
  
  //! Set molecular weight ratio of nitrogen vs agent
  if (agent_type == 1) // Halon
  {
    molecular_weight_ratio = 0.189;
  }
  else if (agent_type == 2) // Novec
  {
    molecular_weight_ratio = 0.0891;
  }
  else // Neither Halon nor Novec. Bad input. Abort the program.
  {
    std::cout << "Wrong agent indicator!" << std::endl;
    abort();
  }
  
  
  //! Loop through all temperatures
  for ( ; count_T < temperature.size(); count_T++)
  {
    N = current_state.liquid*0.01*current_state.n_pressure/c_henry[count_T-1] + current_state.vapor*molecular_weight_ratio*current_state.n_pressure/vapor_p[count_T-1]; // weight of nitrogen in container
    P2 = current_state.n_pressure - 1; // assumed new nitrogen pressure after discharge
    P3 = P2 - 1; // random initialization for P3
    
    //! Intermediate parameters
    E1 = (1 + 0.053*current_state.n_pressure/c_henry[count_T-1]) * liquid_entro[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry[count_T-1];
    R1 = (1 + molecular_weight_ratio*current_state.n_pressure/vapor_p[count_T-1]) / E6;
    A1 = R1*E1 - (1 + current_state.n_pressure/vapor_p[count_T-1]) * vapor_entro[count_T-1];
    
    //! Converging pressure
    double P_thres = 0.02; //!< pressure threshold (psi)
    while ( std::abs(P3 - P2) > P_thres ) // loop if pressure difference is larger than threshold
    {
      E5 = (1 + 0.053*P2/c_henry[count_T]) * liquid_spec_vol[count_T];
      R2 = (1 + 0.01*P2/c_henry[count_T]) / E6;
      R3 = (1 + molecular_weight_ratio*P2/vapor_p[count_T]) / E6;
      A2 = R2*E1 - (1 + 0.053*P2/c_henry[count_T]) * liquid_entro[count_T];
      A3 = R3*E1 - (1 + P2/vapor_p[count_T]) * vapor_entro[count_T];
      
      L2 = (current_state.vapor*A1*vapor_spec_vol[count_T] - A3) / (A2*vapor_spec_vol[count_T] - E5*A3); //!< new liquid weight
      V2 = (1 - L2*E5) / vapor_spec_vol[count_T]; //!< new vapor weight
      L1 = current_state.liquid + current_state.vapor - L2 - V2; //!< incremental liquid weight
      P3 = (N - L1*0.01*current_state.n_pressure/c_henry[count_T-1]) / (L2*0.01/c_henry[count_T] + V2*molecular_weight_ratio/vapor_p[count_T]); // calculated new nitrogen pressure after discharge
      
      //! Adapt P2 towards convergence
      // if (P3 < (P2-P_thres*5)) P2 = P2 - P_thres*10;
      // else if (P3 < (P2-P_thres)) P2 = P2 - P_thres;
      if (P3 < (P2-P_thres)) P2 = P2 - P_thres;
      else if (P3 > (P2+P_thres)) P2 = P2 + P_thres;
      else ; // do nothing
      
      // std::cout << count_T << std::endl;
    }
    
    //! Update current container state
    current_state.temperature = temperature[count_T];
    current_state.pressure = P2 + vapor_p[count_T];
    current_state.discharge = current_state.discharge + L1;
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.liquid_density = (1+0.01*P2/c_henry[count_T]) / (liquid_spec_vol[count_T]*(1+0.053*P2/c_henry[count_T]));
    current_state.percent_discharge = P3; // need to be updated
    Tankstate.push_back(current_state);
  
  }

  return 0;
}
