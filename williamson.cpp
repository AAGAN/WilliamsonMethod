#include "williamson.hpp"
#include <iostream>
#include <fstream>      // ofstream
#include <stdlib.h>     // abort
#include <cmath>        // abs
#include <algorithm>    // std::lower_bound



//! class default constructor
williamson::williamson()
{
  
}





//! class destructor
williamson::~williamson()
{
    
}
  




/**
 * It constructs an agent instance and passes its members to williamson's private members.
 * Users don't need to interact with the agent class.
 * Really the agent class might be unnecessary. The agent constructor can just be implemented here.
 */
williamson::williamson (std::string property_file_name, double molecular_weight_ratio, double coeff_dissol_expan)
{
  
  agent extinguishing_agent(property_file_name, molecular_weight_ratio, coeff_dissol_expan);    // agent instance
  
  // Save agent properties to williamson members
  temperature_ = extinguishing_agent.get_temperature();
  vapor_p_ = extinguishing_agent.get_vapor_p();
  liquid_spec_vol_ = extinguishing_agent.get_liquid_spec_vol();
  vapor_spec_vol_ = extinguishing_agent.get_vapor_spec_vol();
  liquid_enthal_ = extinguishing_agent.get_liquid_enthal();
  vapor_enthal_ = extinguishing_agent.get_vapor_enthal();
  liquid_entro_ = extinguishing_agent.get_liquid_entro();
  vapor_entro_ = extinguishing_agent.get_vapor_entro();
  c_henry_ = extinguishing_agent.get_c_henry();
  molecular_weight_ratio_ = extinguishing_agent.get_molecular_weight_ratio();
  coeff_dissol_expan_ = extinguishing_agent.get_coeff_dissol_expan();
  
  // Tank_state_ and Pipe_state_ are still empty vectors
  
}





// ************************************************************
// Williamson method implementation part 1: tank
// ************************************************************

/**
 * Takes initial storage conditions and calculates container state during discharge, saves in the Tank_state_ vector.
 * Takes in quantities in SI units, but calculations and results are saved in English units.
 * Assuming the property data file is in integer temperature and drops one degree F each line!!
 */
int williamson::tank (double P, double T, double D)
{
  
  // pressure convergence threshold (psi)
  double P_thres = 0.05;
  
  // Convert storage conditions from SI units to English units
  P = P / 6894.76;                  // convert from Pa to psi
  T = (T - 273.15)*(9/5.0) + 32;    // convert from kelvin to fahrenheit
  D = D / 16.0185;                  // convert from kg/(m^3) to lb/(ft^3)
  
  
  // Find the integer temperature in data file closest to the storage temperature and starts from there.
  int count_T = round(temperature_[0] - T); // count the temperature step number
  // If data is not degree by degree in temperature then attempt to do a temperature search.
  // std::vector<double>::reverse_iterator T_iter = std::lower_bound (temperature.rbegin(), temperature.rend(), T);
  // int count_T = temperature.rend() - T_iter - 1;
  
  // If storage temperature is outside the temperature range of data file, abort the program.
  if ((T < temperature_.back()) || (T > (temperature_.front()+1)))
  {
    std::cout << "Storage temperature is outside the available temperature data!" << temperature_[0] << std::endl;
    abort();
  }
  
  
  // Initialize the tank state
  tank_state current_state,         // define a current tank state, in English units
             current_state_si;      // current state in SI units
             
  current_state.temperature = temperature_[count_T];
  current_state.pressure = P + vapor_p_[count_T];
  current_state.discharge = 0.0;
  current_state.liquid = (1-D*vapor_spec_vol_[count_T]) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P/c_henry_[count_T]) - vapor_spec_vol_[count_T]); // assuming unit volume!
  current_state.vapor = D - current_state.liquid;
  current_state.n_pressure = P;
  current_state.liquid_density = (1+0.01*P/c_henry_[count_T]) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P/c_henry_[count_T]));
  current_state.percent_discharge = 0.0;
  
  current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
  current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
  current_state_si.discharge = current_state.discharge * 0.453592;                      // lb to kg
  current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
  current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
  current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
  current_state_si.liquid_density = current_state.liquid_density * 16.0185;             // lb/(ft^3) to kg/(m^3)
  current_state_si.percent_discharge = current_state.percent_discharge;                 // no need to convert for percentage
  
  // Record the initial tank state
  Tank_state_en_.push_back(current_state);    
  Tank_state_si_.push_back(current_state_si);
  count_T++;


  // Loop through all temperatures
  /**
   * Solve for the discharged weight and pressure at each temperature step through the conservations of mass, volume and entropy.
   */
  for ( ; count_T < temperature_.size(); count_T++)
  {
    double N, L1, L2, V2, P2, P3, E1, E5, E6, R1, R2, R3, A1, A2, A3; // intermediate variables
    
    N = current_state.liquid*0.01*current_state.n_pressure/c_henry_[count_T-1] + current_state.vapor*molecular_weight_ratio_*current_state.n_pressure/vapor_p_[count_T-1]; // weight of nitrogen in container
    P2 = current_state.n_pressure - 1;    // assumed new nitrogen pressure after discharge
    P3 = P2 - 1;                          // random initialization for P3
    
    // Intermediate parameters
    E1 = (1 + coeff_dissol_expan_*current_state.n_pressure/c_henry_[count_T-1]) * liquid_entro_[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry_[count_T-1];
    R1 = (1 + molecular_weight_ratio_*current_state.n_pressure/vapor_p_[count_T-1]) / E6;
    A1 = R1*E1 - (1 + current_state.n_pressure/vapor_p_[count_T-1]) * vapor_entro_[count_T-1];
    
    // Converging pressure at one step
    while ( std::abs(P3 - P2) > P_thres )           // loop if pressure difference is larger than threshold
    {
      E5 = (1 + coeff_dissol_expan_*P2/c_henry_[count_T]) * liquid_spec_vol_[count_T];
      R2 = (1 + 0.01*P2/c_henry_[count_T]) / E6;
      R3 = (1 + molecular_weight_ratio_*P2/vapor_p_[count_T]) / E6;
      A2 = R2*E1 - (1 + coeff_dissol_expan_*P2/c_henry_[count_T]) * liquid_entro_[count_T];
      A3 = R3*E1 - (1 + P2/vapor_p_[count_T]) * vapor_entro_[count_T];
      
      L2 = (current_state.vapor*A1*vapor_spec_vol_[count_T] - A3) / (A2*vapor_spec_vol_[count_T] - E5*A3);  // new liquid weight
      V2 = (1 - L2*E5) / vapor_spec_vol_[count_T];                                                         // new vapor weight
      L1 = current_state.liquid + current_state.vapor - L2 - V2;                                          // incremental liquid weight
      P3 = (N - L1*0.01*current_state.n_pressure/c_henry_[count_T-1]) / (L2*0.01/c_henry_[count_T] + V2*molecular_weight_ratio_/vapor_p_[count_T]); // calculated new nitrogen pressure after discharge
      
      // Adapt P2 towards convergence
      // if (P3 < (P2-P_thres*5)) P2 = P2 - P_thres*10;
      // else if (P3 < (P2-P_thres)) P2 = P2 - P_thres;
      if (P3 < (P2-P_thres))        P2 = P2 - P_thres;
      else if (P3 > (P2+P_thres))   P2 = P2 + P_thres;
      // else ; // do nothing
    }
    
    
    // std::cout << P2 << ' ' << P3 << std::endl;
    // Update current container state
    current_state.temperature = temperature_[count_T];
    current_state.pressure = P2 + vapor_p_[count_T];
    current_state.discharge = current_state.discharge + L1;
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.liquid_density = (1+0.01*P2/c_henry_[count_T]) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P2/c_henry_[count_T]));
    current_state.percent_discharge = current_state.discharge / D;
    
    current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
    current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
    current_state_si.discharge = current_state.discharge * 0.453592;                      // lb to kg
    current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
    current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
    current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
    current_state_si.liquid_density = current_state.liquid_density * 16.0185;             // lb/(ft^3) to kg/(m^3)
    current_state_si.percent_discharge = current_state.percent_discharge;                 // no need to convert for percentage
    
    // Make sure there is still liquid agent in container
    // Tank_state_.push_back(current_state);
    if (L2 >= 0.0)    // Still liquid left. Save the current state.
    {
      Tank_state_en_.push_back(current_state);        // saved still in English units
      Tank_state_si_.push_back(current_state_si);     // save in si units
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
// Williamson method implementation part 2: pipe
// ************************************************************

/**
 * Takes tank state snapshot and calculates state along pipe due to pressure drop, saves in the Pipe_state_ vector.
 */
int williamson::pipe (double P, double T)
{
  
  // pressure convergence threshold (psi)
  double P_thres = 0.1;
  
  // Convert storage conditions from SI units to English units
  P = P / 6894.76;                  // convert from Pa to psi
  T = (T - 273.15)*(9/5.0) + 32;    // convert from kelvin to fahrenheit
  
  
  // Since the temperature from recession table comes from the data temperature and therefore exists, no need for temperature search and check and directly find its position.
  int count_T = round(temperature_[0] - T); // count the temperature step number
  
  
  // Initialize the pipe state
  pipe_state current_state,                                                                                 // define a current pipe state
             current_state_si;
  double N = 0.01*P/c_henry_[count_T];                                                                      // nitrogen weight does not change at each step
  
  current_state.temperature = temperature_[count_T];
  current_state.pressure = P + vapor_p_[count_T];
  current_state.liquid = 1;                                                                                 // expansion starts with unit mass of liquid agent and no vapor
  current_state.vapor = 0; 
  current_state.n_pressure = P;                                                                             // share tank nitrogen pressure at pipe beginning
  current_state.density = (1+N) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P/c_henry_[count_T]));  // initial density (only liquid exists)
  
  current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
  current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
  current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
  current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
  current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
  current_state_si.density = current_state.density * 16.0185;                           // lb/(ft^3) to kg/(m^3)
  
  // Record the initial pipe state
  Pipe_state_en_.push_back(current_state);  
  Pipe_state_si_.push_back(current_state_si);                                                                   
  count_T++;
  
  
  // Loop through all temperatures
  /**
   * Solve for the pressure and density at each temperature step through the conservations of mass and enthalpy.
   */
  for ( ; count_T < temperature_.size(); count_T++)
  {
    double L2, V2, P2, P3, E1, E2, E6, E7, E3, E4, E8, E9; // intermediate variables
    P2 = current_state.n_pressure - 1;    // assumed new nitrogen pressure after discharge
    P3 = P2 - 1;                          // random initialization for P3
  
    // Intermediate parameters
    E1 = (1 + coeff_dissol_expan_*current_state.n_pressure/c_henry_[count_T-1]) * liquid_enthal_[count_T-1];
    E2 = (1 + current_state.n_pressure/vapor_p_[count_T-1]) * vapor_enthal_[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry_[count_T-1];
    E7 = 1 + molecular_weight_ratio_*current_state.n_pressure/vapor_p_[count_T-1];
    
    
    // Converging pressure at one step
    while ( std::abs(P3 - P2) > P_thres )           // loop if pressure difference is larger than threshold
    {
      E3 = (1 + coeff_dissol_expan_*P2/c_henry_[count_T]) * liquid_enthal_[count_T];
      E4 = (1 + P2/vapor_p_[count_T]) * vapor_enthal_[count_T];
      E8 = 1 + 0.01*P2/c_henry_[count_T];
      E9 = 1 + molecular_weight_ratio_*P2/vapor_p_[count_T];
      
      L2 = (current_state.liquid*(E1*E9-E6*E4) + current_state.vapor*(E2*E9-E7*E4)) / (E3*E9-E8*E4);      // new liquid weight
      V2 = 1 - L2;                                                                                        // new vapor weight
      P3 = N / (L2*0.01/c_henry_[count_T] + V2*molecular_weight_ratio_/vapor_p_[count_T]);                // calculated new nitrogen pressure after expansion
  
      // Adapt P2 towards convergence
      if (P3 < (P2-P_thres))        P2 = P2 - P_thres;
      else if (P3 > (P2+P_thres))   P2 = P2 + P_thres;
    }
    
    
    // Update current pipe state
    current_state.temperature = temperature_[count_T];
    current_state.pressure = P2 + vapor_p_[count_T];
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.density = (1+N) / (L2*liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P2/c_henry_[count_T]) + V2*vapor_spec_vol_[count_T]);
    
    current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
    current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
    current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
    current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
    current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
    current_state_si.density = current_state.density * 16.0185;                           // lb/(ft^3) to kg/(m^3)
    
    // Make sure partial pressure of nitrogen is still positive in pipe
    // Pipe_state_.push_back(current_state);
    if (P2 >= 0.0)      // Still positive pressure. Store the current state.
    {
      Pipe_state_en_.push_back(current_state);
      Pipe_state_si_.push_back(current_state_si);
    }
    else
    {
      std::cout << std::endl << "Negative nitrogen partial pressure at temperature " << current_state.temperature << "(F)" << std::endl << std::endl;
      return 0;
    }
  
  }
  
  
  return 0;
  
}





int williamson::print_tank_state_en()
{
  
  std::cout << "Tank discharge states with a storage temperature of " << Tank_state_en_[0].temperature << " (F) ," 
            << "a pressure of " << Tank_state_en_[0].pressure << " (psia) " 
            << "and a filling density of " << (Tank_state_en_[0].liquid + Tank_state_en_[0].vapor) << " (lb/(ft^3))"
            << std::endl;
  
  std::cout << "temperature (F)" << ", " 
            << "pressure (psig)" << ", " 
            << "discharge (lb)" << ", " 
            << "liquid (lb)" << ", " 
            << "vapor (lb)" << ", " 
            << "n_pressure (psi)" << ", " 
            << "liquid_density (lbs/cu.ft.)" << ", " 
            << "percent_discharge" << std::endl;
            
  for (int i = 0; i < Tank_state_en_.size(); i++) 
  {
    std::cout << Tank_state_en_[i].temperature << ", " 
              << Tank_state_en_[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
              << Tank_state_en_[i].discharge << ", " 
              << Tank_state_en_[i].liquid << ", " 
              << Tank_state_en_[i].vapor << ", " 
              << Tank_state_en_[i].n_pressure << ", " 
              << Tank_state_en_[i].liquid_density << ", " 
              << Tank_state_en_[i].percent_discharge << std::endl;
  }
  
  std::cout << std::endl;
  
  return 0;
  
}





int williamson::print_tank_state_si()
{
  
  std::cout << "Tank discharge states with a storage temperature of " << Tank_state_si_[0].temperature << " (K) ," 
            << "a pressure of " << Tank_state_si_[0].pressure << " (Pa) " 
            << "and a filling density of " << (Tank_state_si_[0].liquid + Tank_state_si_[0].vapor)/0.0283168 << " (kg/(m^3))"
            << std::endl;
  
  std::cout << "temperature (K)" << ", " 
            << "pressure (Pa)" << ", " 
            << "discharge (kg)" << ", " 
            << "liquid (kg)" << ", " 
            << "vapor (kg)" << ", " 
            << "n_pressure (Pa)" << ", " 
            << "liquid_density (kg/(m^3))" << ", " 
            << "percent_discharge" << std::endl;
            
  for (int i = 0; i < Tank_state_si_.size(); i++) 
  {
    std::cout << Tank_state_si_[i].temperature << ", " 
              << Tank_state_si_[i].pressure << ", " 
              << Tank_state_si_[i].discharge << ", " 
              << Tank_state_si_[i].liquid << ", " 
              << Tank_state_si_[i].vapor << ", " 
              << Tank_state_si_[i].n_pressure << ", " 
              << Tank_state_si_[i].liquid_density << ", " 
              << Tank_state_si_[i].percent_discharge << std::endl;
  }
  
  std::cout << std::endl;
  
  return 0;
  
}





int williamson::print_pipe_state_en()
{
  
  std::cout << "Pipe expansion states with a starting temperature of " << Pipe_state_en_[0].temperature << " (F) " 
            << "and a pressure of " << Pipe_state_en_[0].pressure << " (psia) " 
            << std::endl;
    
  std::cout << "temperature (F)" << ", " 
            << "pressure (psig)" << ", " 
            << "liquid (lb)" << ", " 
            << "vapor (lb)" << ", " 
            << "n_pressure (psi)" << ", " 
            << "density (lbs/cu.ft.)" << std::endl;
              
  for (int i = 0; i < Pipe_state_en_.size(); i++) 
  {
    std::cout << Pipe_state_en_[i].temperature << ", " 
              << Pipe_state_en_[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
              << Pipe_state_en_[i].liquid << ", " 
              << Pipe_state_en_[i].vapor << ", " 
              << Pipe_state_en_[i].n_pressure << ", " 
              << Pipe_state_en_[i].density << std::endl;
  }
  
  std::cout << std::endl;
  
  return 0;
  
}





int williamson::print_pipe_state_si()
{
  
  std::cout << "Pipe expansion states with a starting temperature of " << Pipe_state_si_[0].temperature << " (K) " 
            << "and a pressure of " << Pipe_state_si_[0].pressure << " (Pa) " 
            << std::endl;
    
  std::cout << "temperature (K)" << ", " 
            << "pressure (Pa)" << ", " 
            << "liquid (kg)" << ", " 
            << "vapor (kg)" << ", " 
            << "n_pressure (Pa)" << ", " 
            << "density (kg/(m^3))" << std::endl;
              
  for (int i = 0; i < Pipe_state_si_.size(); i++) 
  {
    std::cout << Pipe_state_si_[i].temperature << ", " 
              << Pipe_state_si_[i].pressure << ", " 
              << Pipe_state_si_[i].liquid << ", " 
              << Pipe_state_si_[i].vapor << ", " 
              << Pipe_state_si_[i].n_pressure << ", " 
              << Pipe_state_si_[i].density << std::endl;
  }
  
  std::cout << std::endl;
  
  return 0;
  
}





int williamson::write_tank_state_en(std::string container_discharge_file)
{
  
  std::ofstream outfile(container_discharge_file.c_str());
  
  for (int i = 0; i < Tank_state_en_.size(); i++) 
  {
    outfile   << Tank_state_en_[i].temperature << ", " 
              << Tank_state_en_[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
              << Tank_state_en_[i].discharge << ", " 
              << Tank_state_en_[i].liquid << ", " 
              << Tank_state_en_[i].vapor << ", " 
              << Tank_state_en_[i].n_pressure << ", " 
              << Tank_state_en_[i].liquid_density << ", " 
              << Tank_state_en_[i].percent_discharge << std::endl;
  }
  outfile.close();
  
  return 0;
  
}





int williamson::write_tank_state_si(std::string container_discharge_file)
{
  
  std::ofstream outfile(container_discharge_file.c_str());
  
  for (int i = 0; i < Tank_state_si_.size(); i++) 
  {
    outfile   << Tank_state_si_[i].temperature << ", " 
              << Tank_state_si_[i].pressure << ", " 
              << Tank_state_si_[i].discharge << ", " 
              << Tank_state_si_[i].liquid << ", " 
              << Tank_state_si_[i].vapor << ", " 
              << Tank_state_si_[i].n_pressure << ", " 
              << Tank_state_si_[i].liquid_density << ", " 
              << Tank_state_si_[i].percent_discharge << std::endl;
  }
  outfile.close();
  
  return 0;
  
}





int williamson::write_pipe_state_en(std::string pipe_expansion_file)
{
  
  std::ofstream outfile(pipe_expansion_file.c_str());
  
  for (int i = 0; i < Pipe_state_en_.size(); i++) 
  {
    outfile   << Pipe_state_en_[i].temperature << ", " 
              << Pipe_state_en_[i].pressure - 14.7 << ", "   // transfer pressure from psi to psig
              << Pipe_state_en_[i].liquid << ", " 
              << Pipe_state_en_[i].vapor << ", " 
              << Pipe_state_en_[i].n_pressure << ", " 
              << Pipe_state_en_[i].density << std::endl;
  }
  outfile.close();
  
  return 0;
  
}





int williamson::write_pipe_state_si(std::string pipe_expansion_file)
{
  
  std::ofstream outfile(pipe_expansion_file.c_str());
  
  for (int i = 0; i < Pipe_state_si_.size(); i++) 
  {
    outfile   << Pipe_state_si_[i].temperature << ", " 
              << Pipe_state_si_[i].pressure << ", " 
              << Pipe_state_si_[i].liquid << ", " 
              << Pipe_state_si_[i].vapor << ", " 
              << Pipe_state_si_[i].n_pressure << ", " 
              << Pipe_state_si_[i].density << std::endl;
  }
  outfile.close();
  
  return 0;
  
}





tank_state williamson::get_tank_state_en(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  int count_T = round(Tank_state_en_[0].temperature - T);   // count the temperature step number
  
  // If storage temperature is outside the temperature range, abort the program.
  if ((T < Tank_state_en_.back().temperature) || (T > Tank_state_en_.front().temperature))
  {
    std::cout << "Storage temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  return Tank_state_en_[count_T];
  
}





tank_state williamson::get_tank_state_si(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  int count_T = round(Tank_state_en_[0].temperature - T);   // count the temperature step number
  
  // If storage temperature is outside the temperature range, abort the program.
  if ((T < Tank_state_en_.back().temperature) || (T > Tank_state_en_.front().temperature))
  {
    std::cout << "Storage temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  return Tank_state_si_[count_T];
  
}





pipe_state williamson::get_pipe_state_en(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  int count_T = round(Pipe_state_en_[0].temperature - T);   // count the temperature step number
  
  // If storage temperature is outside the temperature range, abort the program.
  if ((T < Pipe_state_en_.back().temperature) || (T > Pipe_state_en_.front().temperature))
  {
    std::cout << "Storage temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  return Pipe_state_en_[count_T];
  
}





pipe_state williamson::get_pipe_state_si(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  int count_T = round(Pipe_state_en_[0].temperature - T);   // count the temperature step number
  
  // If storage temperature is outside the temperature range, abort the program.
  if ((T < Pipe_state_en_.back().temperature) || (T > Pipe_state_en_.front().temperature))
  {
    std::cout << "Storage temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  return Pipe_state_si_[count_T];
  
}
  


