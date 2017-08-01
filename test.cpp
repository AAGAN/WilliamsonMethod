#include "williamson.hpp"
#include <assert.h>       // assert function helps debugging
#include <iostream>
#include <fstream>
#include <vector>



//! Define the problem and call Williamson Method

/** 
Define agent file and initial storage conditions (filling density, temperature, nitrogen partial temperature) in SI units.
Container state during discharge is run until the point when liquid is depleted or reaching the lowest temperature available in data, whichever comes first.
Pipe expansion state is run until the point when pressure becomes negative or reaching the lowest temperature available in data, whichever comes first.

Program would also terminate if not converging. Prompting user to try other combinations of pressure convergence threshold and increment.
*/


int main()
{
  
  // ************************************************************
  //! Read in agent property data from files
  // ************************************************************
  
  // Use property data file "Halon1301_property.csv" for Halon 1301
  // std::string property_file_name = "Halon1301_property.csv";
  
  // Use property data file "Novec1230_property.csv" for Novec 1230
  std::string property_file_name = "Novec1230_property.csv";
        
        

  // ****************************************************************************************************
  //! Set molecular weight ratio and coefficient of dissolved volume expansion according to agent
  // ****************************************************************************************************
  
  double molecular_weight_ratio,      // molecular weight ratio of inert gas vs agent
         coeff_dissol_expan;          // effect of dissolved inert gas on the liquid volume
  
  
  // Halon
  // molecular_weight_ratio = 0.188;
  // coeff_dissol_expan = 0.053;
  
  // Novec
  molecular_weight_ratio = 0.088636;
  coeff_dissol_expan = 0.0429;
        
  
  
  // ************************************************************
  //! Initial storage conditions
  // ************************************************************
  
  // Halon
  // double P = 401 * 6894.76;                       // Partial pressure of nitrogen (Pa)
  // double T = (70 - 32)/(9/5.0) + 273.15;          // Storage temperature (Kelvin)
  // double D = 70 * 16.0185;                        // Filling density of storage container (kg/(m^3))
  
  // Novec
  double P = 1001 * 6894.76;                    // Partial pressure of nitrogen (Pa)
  double T = (68 - 32)/(9/5.0) + 273.15;          // Storage temperature (Kelvin)
  double D = 700;                              // Filling density of storage container (kg/(m^3))

  
  
  // ************************************************************
  //! Variables to store tank state and pipe state
  // ************************************************************
  
  /**
  // Structure for the state of tank during discharge.
  struct tank_state
  {
    double temperature;         //!< container temperature
    double pressure;            //!< container pressure
    double discharge;           //!< discharged liquid mass
    double liquid;              //!< liquid mass in container
    double vapor;               //!< vapor mass in container
    double n_pressure;          //!< partial pressure of inert gas (N2)
    double liquid_density;      //!< density of liquid mixture (with inert gas solute)
    double percent_discharge;   //!< percentage of discharged agent: mass of discharged agent over initial filled agent
  };

  // Structure for the state of the pipe.
  struct pipe_state
  {
    double temperature;         //!< pipe temperature
    double pressure;            //!< pipe pressure
    double liquid;              //!< liquid agent mass in pipe
    double vapor;               //!< vapor agent mass in pipe
    double n_pressure;          //!< partial pressure of inert gas (N2) in pipe
    double density;             //!< total density of mixture (agent + inert gas) of liquid and vapor together
  };
  */

  // State containers, for test here.
  std::vector<tank_state> Tank_state;
  tank_state tank_state_snap;
  std::vector<pipe_state> Pipe_state;
  pipe_state pipe_state_snap;
  
  
  
  // ************************************************************
  //! Create an instance of williamson class
  //  Refer to williamson.hpp for all available class methods
  // ************************************************************
  
  williamson test_case(property_file_name, molecular_weight_ratio, coeff_dissol_expan);
  
  
  // Turn on verbose for more information is desired when running
  test_case.verbose_on();
  
  
  // Set pressure threshold to a desired value
  test_case.set_pressure_threshold(0.3);
  
  
  
  
  
  // ************************************************************
  //! Solve for tank state
  // ************************************************************

  int error_code_1 = test_case.tank
  (
    P, 
    T, 
    D
  );
  // Tank state in test_case has been updated after running williamson

  
  // If tank calculation succeeds
  if (error_code_1 == 0)
  {
    // Print vector to screen
    test_case.print_tank_state_en();
    // test_case.print_tank_state_si();
              
    
    // Write to file
    std::string container_discharge_file;                             // The file to hold container state during discharge
    // container_discharge_file = "Halon1301_container_discharge.csv";
    container_discharge_file = "Novec1230_container_discharge.csv";
    test_case.write_tank_state_en(container_discharge_file);
    // test_case.write_tank_state_si(container_discharge_file);
    
    
    // Access the tank state vector
    // Tank_state = test_case.get_tank_state_en();
    Tank_state = test_case.get_tank_state_si();
    
    
    // Access and print the tank state structure at the given temperature or percentage of discharge
    // tank_state_snap = test_case.get_tank_state_from_T_en(T);
    tank_state_snap = test_case.get_tank_state_from_percentdischarge_si(0.5);
    print_one_tank_state_si(tank_state_snap);
    // tank_state_snap = test_case.get_tank_state_from_T_si(T);
    // print_tank_state_si(tank_state_snap);
  }
  
  else
  {
    std::cout << "williamson code tank part exited with code " << error_code_1 << std::endl;
  }
  
  
  
  
  
  // ************************************************************
  //! Solve for pipe state at a given discharging snapshot
  // ************************************************************
  
  // Turn off verbose when not wanting information to overflow
  test_case.verbose_off();
  
  
  // int tank_line = 16;                        // Choose the line number in the tank state table as the starting pipe condition. Use for testing.
  int error_code_2 = test_case.pipe
  (
    tank_state_snap.n_pressure,
    tank_state_snap.temperature
    // Tank_state[tank_line].n_pressure, 
    // Tank_state[tank_line].temperature
  );
  
  
  // If pipe calculation succeeds
  if (error_code_2 == 0)
  {
    // Print vector to screen
    test_case.print_pipe_state_en();
    // test_case.print_pipe_state_si();

              
    // Write to file
    std::string pipe_expansion_file;                             // The file to hold pipe state during expansion
    // pipe_expansion_file = "Halon1301_pipe_expansion.csv";
    pipe_expansion_file = "Novec1230_pipe_expansion.csv";
    test_case.write_pipe_state_en(pipe_expansion_file);
    // test_case.write_pipe_state_si(pipe_expansion_file);
    
    
    // Access the pipe state vector
    // Pipe_state = test_case.get_pipe_state_en();
    Pipe_state = test_case.get_pipe_state_si();
    
    
    // Access and print the pipe state structure at the given temperature
    int pipe_line = 6;                        // Access the line number in the pipe state table
    pipe_state_snap = test_case.get_pipe_state_en(Pipe_state[pipe_line].temperature);
    print_one_pipe_state_en(pipe_state_snap);
    pipe_state_snap = test_case.get_pipe_state_si(Pipe_state[pipe_line].temperature);
    print_one_pipe_state_si(pipe_state_snap);
  }
  
  else
  {
    std::cout << "williamson code pipe part exited with code " << error_code_2 << std::endl;
  }
  
  
  
  return 0;
}

