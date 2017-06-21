#include "williamson.hpp"
#include "agent.hpp"
#include <iostream>


int williamson(agent *extiguishig_agent, std::vector<tank_state> &Tankstate, double P, double T, double D)
{
  // Accessing agent properties
  std::vector<double> vapor_p = extiguishig_agent->get_vapor_p();
  std::vector<double> liquid_spec_vol = extiguishig_agent->get_liquid_spec_vol();
  std::vector<double> vapor_spec_vol = extiguishig_agent->get_vapor_spec_vol();
  std::vector<double> c_henry = extiguishig_agent->get_c_henry();

  // Initialize the tank state
  // Assume the property data file starts with the storage temperature and drops one degree K each line!!
  tank_state current_state; // define a current tank state
  current_state.temperature = T;
  current_state.pressure = P + vapor_p[0];
  current_state.discharge = 0.0;
  current_state.liquid = (1-D*vapor_spec_vol[0])/(liquid_spec_vol[0]*(1+0.053*P/c_henry[0])-vapor_spec_vol[0]); // assuming unit volume (1L)!!
  current_state.vapor = D - current_state.liquid;
  current_state.n_pressure = P;
  current_state.liquid_density = (1+0.01*P/c_henry[0])/(liquid_spec_vol[0]*(1+0.053*P/c_henry[0]));
  current_state.percent_discharge = 0.0;
  Tankstate.push_back(current_state); // Record the initial tank state

  return 0;
}
