#include "williamson.hpp"
#include <assert.h>       // assert function helps debugging
#include <iostream>
#include <fstream>
#include <vector>



//! Define the problem and call Williamson Method
/** 
Define agent file and initial storage conditions (filling density, temperature, nitrogen partial temperature).
Container state during discharge is run until the point when liquid is depleted or reaching the lowest temperature available in data, whichever comes first.
Pipe expansion state is run until the point when pressure becomes negative or reaching the lowest temperature available in data, whichever comes first.
*/

int main()
{
  
  // ************************************************************
  //! read in agent property data from files
  // ************************************************************
  // Use property data file "Halon1301_property.csv" for Halon 1301
  std::string property_file_name = "Halon1301_property.csv";
  // Use property data file "Novec1230_property.csv" for Novec 1230
  // std::string property_file_name = "Novec1230_property.csv";
        
        

  // ************************************************************
  //! Set molecular weight ratio and coefficient of dissolved volume expansion according to agent
  // ************************************************************
  double molecular_weight_ratio,      // molecular weight ratio of nitrogen vs agent
         coeff_dissol_expan;          // effect of dissolved nitrogen on the liquid volume
  
  // Halon
  molecular_weight_ratio = 0.188;
  coeff_dissol_expan = 0.053;
  // Novec
  // molecular_weight_ratio = 0.088636;
  // coeff_dissol_expan = 0.0429;
        
  
  
  // ************************************************************
  //! Initial storage conditions
  // ************************************************************
  // Halon
  double P = 401 * 6894.76;                       // Partial pressure of nitrogen (Pa)
  double T = (70 - 32)/(9/5.0) + 273.15;          // Storage temperature (Kelvin)
  double D = 70 * 16.0185;                        // Filling density of storage container (kg/(m^3))
  // Novec
  // double P = 1001 * 6894.76;                    // Partial pressure of nitrogen (Pa)
  // double T = (68 - 32)/(9/5.0) + 273.15;          // Storage temperature (Kelvin)
  // double D = 700;                              // Filling density of storage container (kg/(m^3))

  std::vector<tank_state> Tank_state;
  tank_state tank_state_snap;
  std::vector<pipe_state> Pipe_state;
  pipe_state pipe_state_snap;
  
  
  // create an instance of williamson class
  williamson test_case(property_file_name, molecular_weight_ratio, coeff_dissol_expan);
  
  
  
  // ************************************************************
  //! Solve for tank state
  // ************************************************************

  int error_code_1 = test_case.tank
  (
    P, 
    T, 
    D
  );
  // Tank_state_ has been updated after running williamson

  
  if (error_code_1 == 0)
  {
    // Print to screen
    test_case.print_tank_state_en();
              
    
    // Write to file
    std::string container_discharge_file;                             // The file to hold container state during discharge
    container_discharge_file = "Halon1301_container_discharge.csv";
    // container_discharge_file = "Novec1230_container_discharge.csv";
    test_case.write_tank_state_en(container_discharge_file);
    
    
    // Access the tank state vector
    Tank_state = test_case.get_tank_state_si();
    
    
    // Access the tank state structure at a time snapshot
    tank_state_snap = test_case.get_tank_state_si(T);
  }
  
  else
  {
    std::cout << "williamson code part 1 exited with code " << error_code_1 << std::endl;
  }
  
  
  
  // ************************************************************
  //! Solve for pipe state at a given discharging snapshot
  // ************************************************************
  
  int tank_line = 6; // The line number in the tank state table
  int error_code_2 = test_case.pipe
  (
    Tank_state[tank_line].n_pressure, 
    Tank_state[tank_line].temperature
  );
  
  
  if (error_code_2 == 0)
  {
    // Print to screen
    test_case.print_pipe_state_en();

              
    // Write to file
    std::string pipe_expansion_file;                             // The file to hold pipe state during expansion
    pipe_expansion_file = "Halon1301_pipe_expansion.csv";
    // pipe_expansion_file = "Novec1230_pipe_expansion.csv";
    test_case.write_pipe_state_en(pipe_expansion_file);
    
    
    // Access the pipe state vector
    Pipe_state = test_case.get_pipe_state_si();
    
    
    // Access the pipe state structure at a time snapshot
    pipe_state_snap = test_case.get_pipe_state_si(Tank_state[tank_line].temperature);
  }
  
  else
  {
    std::cout << "williamson code part 2 exited with code " << error_code_2 << std::endl;
  }
  
  
  return 0;
}
