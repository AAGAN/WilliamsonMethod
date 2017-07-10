#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>

int main()
{
  
  // read in property data from files or best fit function: 'agent property file'
  int agent_type = 2;
  /*
  initialize the instance with agent type
  1 = Halon 1301
  2 = Novec 1230
  */
  agent extinguishing_agent(agent_type); // agent instance
  agent * p_agent = & extinguishing_agent;
  
  // Initial storage conditions
  double P = 1010.3 * 6894.76;                    // Partial pressure of nitrogen (Pa)
  double T = (70 - 32)/(9/5.0) + 273.15;          // Storage temperature (Kelvin)
  double D = 1400; // 70 * 16.0185;               // Filling density of storage container (kg/(m^3))

  std::vector<tank_state> Tank_state;
  
  
  int error_code = williamson
  (
    p_agent,
    agent_type,
    Tank_state,
    P, 
    T, 
    D
  );
  // Tank_state has been updated after running williamson

  
  if (error_code == 0)
  {
    std::cout << "temperature (F)" << ", " 
              << "pressure (psig)" << ", " 
              << "discharge (lb)" << ", " 
              << "liquid (lb)" << ", " 
              << "vapor (lb)" << ", " 
              << "n_pressure (psi)" << ", " 
              << "liquid_density (lbs/cu.ft.)" << ", " 
              << "percent_discharge" << std::endl;
              
    std::string container_discharge_file;                             // The file to hold container state during discharge
    // container_discharge_file = "Halon1301_container_discharge_density80.csv";
    container_discharge_file = "Novec1230_container_discharge.csv";
    std::ofstream outfile(container_discharge_file.c_str());
  
    for (int i = 0; i < Tank_state.size(); i++) 
    {
      // Print to screen
      std::cout << Tank_state[i].temperature << ", " 
                << Tank_state[i].pressure - 14.7 << ", " 
                << Tank_state[i].discharge << ", " 
                << Tank_state[i].liquid << ", " 
                << Tank_state[i].vapor << ", " 
                << Tank_state[i].n_pressure << ", " 
                << Tank_state[i].liquid_density << ", " 
                << Tank_state[i].percent_discharge << std::endl;
      
      // Print to file
      outfile   << Tank_state[i].temperature << ", " 
                << Tank_state[i].pressure - 14.7 << ", " 
                << Tank_state[i].discharge << ", " 
                << Tank_state[i].liquid << ", " 
                << Tank_state[i].vapor << ", " 
                << Tank_state[i].n_pressure << ", " 
                << Tank_state[i].liquid_density << ", " 
                << Tank_state[i].percent_discharge << std::endl;
    }
    outfile.close();
  }
  
  else
  {
    std::cout << "williamson code exited with code " << error_code << std::endl;
  }
  
  
  return 0;
}
