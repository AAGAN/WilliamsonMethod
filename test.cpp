#include "agent.hpp"
#include "williamson.hpp"
#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>


//! Define the problem and call Williamson Method
/** 
Define agent type and initial storage conditions (filling density, temperature, nitrogen partial temperature).
Print out the state during discharge, until the point when agent is depleted or reaching the lowest temperature available in data.
*/

int main()
{
  
  // read in property data from files or best fit function: 'agent property file'
  int agent_type = 1;
  /*
  initialize the instance with agent type
  1 = Halon 1301
  2 = Novec 1230
  */
  agent extinguishing_agent(agent_type); // agent instance
  agent * p_agent = & extinguishing_agent;
  
  // Initial storage conditions
  double P = 401 * 6894.76; // 1010.3 * 6894.76;                    // Partial pressure of nitrogen (Pa)
  double T = (70 - 32)/(9/5.0) + 273.15;          // Storage temperature (Kelvin)
  double D = 70 * 16.0185;               // Filling density of storage container (kg/(m^3))

  std::vector<tank_state> Tank_state;
  std::vector<pipe_state> Pipe_state;
  
  
  
  // ************************************************************
  // Solve for tank state
  // ************************************************************
  
  int error_code_1 = williamson
  (
    p_agent,
    agent_type,
    Tank_state,
    P, 
    T, 
    D
  );
  // Tank_state has been updated after running williamson

  
  if (error_code_1 == 0)
  {
    std::cout << "Tank discharge state with a storage temperature of " << Tank_state[0].temperature << " (F) ," 
              << "a pressure of " << Tank_state[0].pressure << " (psia) " 
              << "and a filling density of " << D * 0.062428 << " (lb/(ft^3))"
              << std::endl;
    std::cout << "temperature (F)" << ", " 
              << "pressure (psig)" << ", " 
              << "discharge (lb)" << ", " 
              << "liquid (lb)" << ", " 
              << "vapor (lb)" << ", " 
              << "n_pressure (psi)" << ", " 
              << "liquid_density (lbs/cu.ft.)" << ", " 
              << "percent_discharge" << std::endl;
              
    std::string container_discharge_file;                             // The file to hold container state during discharge
    container_discharge_file = "Halon1301_container_discharge.csv";
    // container_discharge_file = "Novec1230_container_discharge.csv";
    std::ofstream outfile(container_discharge_file.c_str());
  
    for (int i = 0; i < Tank_state.size(); i++) 
    {
      // Print to screen
      std::cout << Tank_state[i].temperature << ", " 
                << Tank_state[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
                << Tank_state[i].discharge << ", " 
                << Tank_state[i].liquid << ", " 
                << Tank_state[i].vapor << ", " 
                << Tank_state[i].n_pressure << ", " 
                << Tank_state[i].liquid_density << ", " 
                << Tank_state[i].percent_discharge << std::endl;
      
      // Print to file
      outfile   << Tank_state[i].temperature << ", " 
                << Tank_state[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
                << Tank_state[i].discharge << ", " 
                << Tank_state[i].liquid << ", " 
                << Tank_state[i].vapor << ", " 
                << Tank_state[i].n_pressure << ", " 
                << Tank_state[i].liquid_density << ", " 
                << Tank_state[i].percent_discharge << std::endl;
    }
    std::cout << std::endl;
    outfile.close();
  }
  
  else
  {
    std::cout << "williamson code part 1 exited with code " << error_code_1 << std::endl;
  }
  
  
  
  // ************************************************************
  // Solve for pipe state
  // ************************************************************
  
  
  // Need to decide on the method for pipe input
  int tank_line = 16; // The line number in the tank state table
  
  int error_code_2 = williamson
  (
    p_agent,
    agent_type,
    Pipe_state,
    Tank_state[tank_line].n_pressure, 
    Tank_state[tank_line].temperature
  );
  
  
  if (error_code_2 == 0)
  {
    std::cout << "Pipe expansion state with a starting temperature of " << Tank_state[tank_line].temperature << " (F) " 
              << "and a pressure of " << Tank_state[tank_line].pressure - 14.7 << " (psig) " 
              << std::endl;
    std::cout << "temperature (F)" << ", " 
              << "pressure (psig)" << ", " 
              << "liquid (lb)" << ", " 
              << "vapor (lb)" << ", " 
              << "n_pressure (psi)" << ", " 
              << "density (lbs/cu.ft.)" << std::endl;
              
    std::string pipe_expansion_file;                             // The file to hold pipe state during expansion
    pipe_expansion_file = "Halon1301_pipe_expansion.csv";
    // pipe_expansion_file = "Novec1230_pipe_expansion.csv";
    std::ofstream outfile(pipe_expansion_file.c_str());
  
    for (int i = 0; i < Pipe_state.size(); i++) 
    {
      // Print to screen
      std::cout << Pipe_state[i].temperature << ", " 
                << Pipe_state[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
                << Pipe_state[i].liquid << ", " 
                << Pipe_state[i].vapor << ", " 
                << Pipe_state[i].n_pressure << ", " 
                << Pipe_state[i].density << std::endl;
      
      // Print to file
      outfile   << Pipe_state[i].temperature << ", " 
                << Pipe_state[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
                << Pipe_state[i].liquid << ", " 
                << Pipe_state[i].vapor << ", " 
                << Pipe_state[i].n_pressure << ", " 
                << Pipe_state[i].density << std::endl;
    }
    std::cout << std::endl;
    outfile.close();
  }
  else
  {
    std::cout << "williamson code part 2 exited with code " << error_code_2 << std::endl;
  }
  
  
  return 0;
}
