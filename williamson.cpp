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
 * 
 * Tank_state_ and Pipe_state_ are still empty vectors after this.
 */
williamson::williamson (std::string property_file_name, double molecular_weight_ratio, double coeff_dissol_expan)
{
  
  // Construct an agent instance
  agent extinguishing_agent(property_file_name, molecular_weight_ratio, coeff_dissol_expan);    
  
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
  specific_heat_ratio_ = extinguishing_agent.get_specific_heat_ratio();
  
  molecular_weight_ratio_ = extinguishing_agent.get_molecular_weight_ratio();
  coeff_dissol_expan_ = extinguishing_agent.get_coeff_dissol_expan();
  
  
  P_thres_ = 0.5;         // default pressure convergence threshold (psi)
  
  verbose = false;        // default verbose flag state
  
}





// ************************************************************
// Williamson method implementation part 1: tank
// ************************************************************

/**
 * Takes initial storage conditions and calculates container state during discharge. Saves in the Tank_state_ vector.
 * Takes in quantities in SI units, but calculations and results are saved in English units.
 * 
 * No requirement on the temperature increment of the property data file. It only needs to be in temperature-ascending order.
 * The temperature input also doesn't have to be in file.
 * Use the properties if storage temperature exists in file, otherwise linearly interpolate between two closest temperature data.
 * Aborts if storage temperature is outside available range. No extrapolation.
 */
int williamson::tank (double P, double T, double D, double gas_specific_heat_ratio, double gas_molecular_weight)
{
  
  // Convert storage conditions from SI units to English units
  P = P / 6894.76;                        // convert from Pa to psi
  T = (T - 273.15)*(9/5.0) + 32;          // convert from kelvin to fahrenheit
  D = D / 16.0185;                        // convert from kg/(m^3) to lb/(ft^3)
  
  
  
  // Temperature pre-processing
  
  // If storage temperature is outside the temperature range of data file, abort the program.
  // Allow a small error 0.001 to deal with float number comparison issue: slight difference but essentially equal.
  if ((T < (temperature_.back() - 0.001)) || (T > (temperature_.front() + 0.001)))
  {
    std::cout << "Storage temperature is outside the available temperature data! Adapt input." << std::endl;
    return 1;
  }
  
  
  // Do a temperature search. Use a reverse iterator to find the block in data file that the storage temperature belongs to.
  std::vector<double>::reverse_iterator T_iter = std::lower_bound (temperature_.rbegin(), temperature_.rend(), T);
  int count_T = temperature_.rend() - T_iter - 1;
  
  // Linear interpolation only if temperature is in the middle of block. No need to interpolate if it's very close to boundaries.
  // 0.001 is the threshold for vicinity.
  if ( (std::abs(T - temperature_[count_T]) > 0.001) && (std::abs(T - temperature_[count_T+1]) > 0.001) )
  // if ( ((temperature_[count_T] - T) > 0.001) && ((T - temperature_[count_T+1]) > 0.001) )                      // This is the more rigorous condition, but using absolute value is just the same.
  {
    // print interpolation info if verbose
    if (verbose)
    {
      std::cout << "Interpolating and inserting temperature T=" << T << "(F) to line number " << count_T << " of the data." << std::endl << std::endl;
    }
    
    double linear_coeff = (T - temperature_[count_T+1]) / (temperature_[count_T] - temperature_[count_T+1]);      // coefficient for linear interpolation
    std::vector<double>::iterator it;         // vector iterator finds position in original data to insert interpolated properties at storage temperature so that temperature remains sorted
    it = temperature_.begin() + count_T + 1;
    temperature_.insert ( it , T );
    it = vapor_p_.begin() + count_T + 1;
    vapor_p_.insert ( it , ((linear_coeff * (vapor_p_[count_T] - vapor_p_[count_T+1])) + vapor_p_[count_T+1]) );
    it = liquid_spec_vol_.begin() + count_T + 1;
    liquid_spec_vol_.insert ( it , ((linear_coeff * (liquid_spec_vol_[count_T] - liquid_spec_vol_[count_T+1])) + liquid_spec_vol_[count_T+1]) );
    it = vapor_spec_vol_.begin() + count_T + 1;
    vapor_spec_vol_.insert ( it , ((linear_coeff * (vapor_spec_vol_[count_T] - vapor_spec_vol_[count_T+1])) + vapor_spec_vol_[count_T+1]) );
    it = liquid_enthal_.begin() + count_T + 1;
    liquid_enthal_.insert ( it , ((linear_coeff * (liquid_enthal_[count_T] - liquid_enthal_[count_T+1])) + liquid_enthal_[count_T+1]) );
    it = vapor_enthal_.begin() + count_T + 1;
    vapor_enthal_.insert ( it , ((linear_coeff * (vapor_enthal_[count_T] - vapor_enthal_[count_T+1])) + vapor_enthal_[count_T+1]) );
    it = liquid_entro_.begin() + count_T + 1;
    liquid_entro_.insert ( it , ((linear_coeff * (liquid_entro_[count_T] - liquid_entro_[count_T+1])) + liquid_entro_[count_T+1]) );
    it = vapor_entro_.begin() + count_T + 1;
    vapor_entro_.insert ( it , ((linear_coeff * (vapor_entro_[count_T] - vapor_entro_[count_T+1])) + vapor_entro_[count_T+1]) );
    it = c_henry_.begin() + count_T + 1;
    c_henry_.insert ( it , ((linear_coeff * (c_henry_[count_T] - c_henry_[count_T+1])) + c_henry_[count_T+1]) );
    count_T++;      // step forward
  }
  else if ( std::abs(T - temperature_[count_T+1]) <= 0.001 )        // No need to interpolate if it's close to the lower bound temperature, but necessary to step forward.
  {
    count_T++;
  }
  // Otherwise it's close to the upper bound. No need to do anything.
  
  
  
  // Initialize the tank state
  
  tank_state current_state,               // define a current tank state, in English units
             current_state_si;            // current state in SI units
             
  // initial tank state in English units
  current_state.temperature = temperature_[count_T];
  current_state.pressure = P + vapor_p_[count_T];
  current_state.discharge = 0.0;
  current_state.liquid = (1-D*vapor_spec_vol_[count_T]) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P/c_henry_[count_T]) - vapor_spec_vol_[count_T]);   // assuming unit volume
  current_state.vapor = D - current_state.liquid;
  current_state.n_pressure = P;
  current_state.liquid_density = (1+0.01*P/c_henry_[count_T]) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P/c_henry_[count_T]));
  current_state.percent_discharge = 0.0;
  
  // initial tank state in SI units
  current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;         // F to K
  current_state_si.pressure = current_state.pressure * 6894.76;                             // psi to Pa
  current_state_si.discharge = current_state.discharge * 0.453592;                          // lb to kg
  current_state_si.liquid = current_state.liquid * 0.453592;                                // lb to kg
  current_state_si.vapor = current_state.vapor * 0.453592;                                  // lb to kg
  current_state_si.n_pressure = current_state.n_pressure * 6894.76;                         // psi to Pa
  current_state_si.liquid_density = current_state.liquid_density * 16.0185;                 // lb/(ft^3) to kg/(m^3)
  current_state_si.percent_discharge = current_state.percent_discharge;                     // no need to convert for percentage
  
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
    
    double N, L1, L2, V2, P2, P3, E1, E5, E6, R1, R2, R3, A1, A2, A3;             // intermediate variables
    int count_iter = 1;                                                           // count iterations
    double P_inc = P_thres_/1.0;                                                  // pressure step increment when not converged
    
    N = current_state.liquid*0.01*current_state.n_pressure/c_henry_[count_T-1] + current_state.vapor*molecular_weight_ratio_*current_state.n_pressure/vapor_p_[count_T-1]; // weight of nitrogen in container
    P2 = current_state.n_pressure;        // assumed new nitrogen pressure after discharge
    P3 = P2 - 1;                          // random initialization for P3
    
    // Intermediate parameters
    E1 = (1 + coeff_dissol_expan_*current_state.n_pressure/c_henry_[count_T-1]) * liquid_entro_[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry_[count_T-1];
    R1 = (1 + molecular_weight_ratio_*current_state.n_pressure/vapor_p_[count_T-1]) / E6;
    A1 = R1*E1 - (1 + current_state.n_pressure/vapor_p_[count_T-1]) * vapor_entro_[count_T-1];
    
    // Converging pressure at one step
    while ( std::abs(P3 - P2) > P_thres_ )           // loop if pressure difference is still larger than threshold
    {
      E5 = (1 + coeff_dissol_expan_*P2/c_henry_[count_T]) * liquid_spec_vol_[count_T];
      R2 = (1 + 0.01*P2/c_henry_[count_T]) / E6;
      R3 = (1 + molecular_weight_ratio_*P2/vapor_p_[count_T]) / E6;
      A2 = R2*E1 - (1 + coeff_dissol_expan_*P2/c_henry_[count_T]) * liquid_entro_[count_T];
      A3 = R3*E1 - (1 + P2/vapor_p_[count_T]) * vapor_entro_[count_T];
      
      L2 = (current_state.vapor*A1*vapor_spec_vol_[count_T] - A3) / (A2*vapor_spec_vol_[count_T] - E5*A3);    // new liquid weight
      V2 = (1 - L2*E5) / vapor_spec_vol_[count_T];                                                            // new vapor weight
      L1 = current_state.liquid + current_state.vapor - L2 - V2;                                              // incremental liquid weight
      P3 = (N - L1*0.01*current_state.n_pressure/c_henry_[count_T-1]) / (L2*0.01/c_henry_[count_T] + V2*molecular_weight_ratio_/vapor_p_[count_T]); // calculated new nitrogen pressure after discharge
      
      // print convergence info if verbose
      if (verbose)
      {
        std::cout << "Tank iteration " << count_iter << " for temperature T=" << temperature_[count_T] << "(F): P2=" << P2 << "(psi), P3=" << P3 << "(psi)." << std::endl;
      }
    
      // Adapt P2 towards convergence
      if ( P3 < ( P2 - P_thres_ ) )        P2 = P2 - P_inc;
      else if ( P3 > ( P2 + P_thres_ ) )   P2 = P2 + P_inc;
      
      // If pressure drops to negative or too many iterations, result is not converging using current combination of threshold and increment.
      if ( (P2 < 0) || (P3 < 0) || (count_iter > 2*P/P_inc) )
      {
        std::cout << "Tank state is not converging. Try again with a larger criterion." << std::endl;
        std::cout << "This can be done by changing the input of the \"set_pressure_threshold\" member function." << std::endl;
        return 2;
      }
      
      count_iter++;
    }
    
    
    
    // Update current container state in English units
    P2 = (P2 + P3)/2.0;                   // Use the average of P2 and P3 as the converged pressure
    current_state.temperature = temperature_[count_T];
    current_state.pressure = P2 + vapor_p_[count_T];
    current_state.discharge = current_state.discharge + L1;
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.liquid_density = (1+0.01*P2/c_henry_[count_T]) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P2/c_henry_[count_T]));
    current_state.percent_discharge = current_state.discharge / D;
    
    // Update current container state in SI units
    current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
    current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
    current_state_si.discharge = current_state.discharge * 0.453592;                      // lb to kg
    current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
    current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
    current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
    current_state_si.liquid_density = current_state.liquid_density * 16.0185;             // lb/(ft^3) to kg/(m^3)
    current_state_si.percent_discharge = current_state.percent_discharge;                 // no need to convert for percentage
    
    
    // print converged current state to be saved if verbose
    if (verbose)
    {
      print_one_tank_state_en(current_state);
    }
    
    
    // Check whether there is still liquid agent in container
    if (L2 >= 0.0)                                    // Still liquid left. Save the current state.
    {
      Tank_state_en_.push_back(current_state);        // save in English units
      Tank_state_si_.push_back(current_state_si);     // save in si units
    }
    
    else                                              // Interpolate linearly between the two states before and after liquid depletion to find the exact depletion point.
    {
      // Update current state to the exact depletion point, in English units
      double linear_coeff = (0.0 - current_state.liquid) / (Tank_state_en_.back().liquid - current_state.liquid);      // coefficient for linear interpolation
      current_state.temperature = (linear_coeff * (Tank_state_en_.back().temperature - current_state.temperature)) + current_state.temperature;
      current_state.pressure = (linear_coeff * (Tank_state_en_.back().pressure - current_state.pressure)) + current_state.pressure;
      current_state.discharge = (linear_coeff * (Tank_state_en_.back().discharge - current_state.discharge)) + current_state.discharge;
      current_state.liquid = 0.0;
      current_state.vapor = (linear_coeff * (Tank_state_en_.back().vapor - current_state.vapor)) + current_state.vapor;
      current_state.n_pressure = (linear_coeff * (Tank_state_en_.back().n_pressure - current_state.n_pressure)) + current_state.n_pressure;
      current_state.liquid_density = (linear_coeff * (Tank_state_en_.back().liquid_density - current_state.liquid_density)) + current_state.liquid_density;
      current_state.percent_discharge = (linear_coeff * (Tank_state_en_.back().percent_discharge - current_state.percent_discharge)) + current_state.percent_discharge;
        
      // Update current container state in SI units
      current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
      current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
      current_state_si.discharge = current_state.discharge * 0.453592;                      // lb to kg
      current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
      current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
      current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
      current_state_si.liquid_density = current_state.liquid_density * 16.0185;             // lb/(ft^3) to kg/(m^3)
      current_state_si.percent_discharge = current_state.percent_discharge;                 // no need to convert for percentage
      
      // Save the interpolated depletion point
      Tank_state_en_.push_back(current_state);        // save in English units
      Tank_state_si_.push_back(current_state_si);     // save in si units
      
      std::cout << std::endl << "Liquid agent depletes at temperature " << current_state.temperature << "(F)" << std::endl << std::endl;
      
      
      
      // ************************************************************
      // After this point the container is left with gas mixture of N2 and agent, which obeys Ideal Gas Law.
      // ************************************************************
      
      // Save the exact depletion state as the initial ideal gas state
      Tank_ideal_gas_state_en_.push_back(current_state);        // save in English units
      Tank_ideal_gas_state_si_.push_back(current_state_si);     // save in si units
      
      int count_ideal_gas_T = count_T;                          // count temperature step in temperature_ list during ideal gas calculation
      double pressure_inert_gas_0,                              // partial pressure of inert gas inside tank before expansion in current step
             pressure_inert_gas_t,                              // partial pressure of inert gas inside tank after expansion in current step
             pressure_agent_0,                                  // partial pressure of agent vapor inside tank before expansion in current step
             pressure_agent_t,                                  // partial pressure of agent vapor inside tank after expansion in current step
             weight_inert_gas_t,                                // weight of inert gas inside tank after expansion in current step
             weight_agent_0,                                    // weight of agent vapor inside tank before expansion in current step
             weight_agent_t;                                    // weight of agent vapor inside tank after expansion in current step
      double R = 10.73;                                         // gas constant (psi*ft3/(lbmol*R)). Remember to change temperature from F to R when using this value!
      
      // Loop through the rest of temperatures in file for ideal gas calculation
      /*
        For each step inside loop, current_state represents the state before expansion, and count_ideal_gas_T points to the state after expansion to be calculated.
      */
      for ( ; count_ideal_gas_T < temperature_.size(); count_ideal_gas_T++)
      {
        // initialize states before current step expansion
        pressure_inert_gas_0 = current_state.n_pressure;
        pressure_agent_0 = current_state.pressure - current_state.n_pressure;
        weight_agent_0 = ( (pressure_agent_0*1) / (R * (459.67+current_state.temperature)) ) * (gas_molecular_weight/molecular_weight_ratio_);     // calculate agent vapor mole, then transfer to mass in g, then into lbs
        
        
        // Calculating ideal gas states after expansion
        pressure_inert_gas_t = pressure_inert_gas_0 * pow( (temperature_[count_ideal_gas_T] / current_state.temperature), (gas_specific_heat_ratio/(gas_specific_heat_ratio-1)) );
        current_state.n_pressure = pressure_inert_gas_t;
        pressure_agent_t = pressure_agent_0 * pow( (temperature_[count_ideal_gas_T] / current_state.temperature), (specific_heat_ratio_[count_ideal_gas_T]/(specific_heat_ratio_[count_ideal_gas_T]-1)) ); // use the specific heat ratio at the end of current step, just for ease.
        current_state.pressure = pressure_agent_t + pressure_inert_gas_t;
        
        current_state.temperature = temperature_[count_ideal_gas_T];
        current_state.liquid = 0.0;
        current_state.liquid_density = 0.0;
        
        weight_inert_gas_t = ( (pressure_inert_gas_t*1) / (R * (459.67+current_state.temperature)) ) * gas_molecular_weight;                       // calculate inert gas mole, then transfer to mass in g, then into lbs
        weight_agent_t = ( (pressure_agent_t*1) / (R * (459.67+current_state.temperature)) ) * (gas_molecular_weight/molecular_weight_ratio_);     // calculate agent vapor mole, then transfer to mass in g, then into lbs
        current_state.vapor = weight_agent_t;
        current_state.discharge = current_state.discharge + (weight_agent_0 - weight_agent_t);
        current_state.percent_discharge = current_state.discharge / D;
        
        
        // Update current container state in SI units
        current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
        current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
        current_state_si.discharge = current_state.discharge * 0.453592;                      // lb to kg
        current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
        current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
        current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
        current_state_si.liquid_density = current_state.liquid_density * 16.0185;             // lb/(ft^3) to kg/(m^3)
        current_state_si.percent_discharge = current_state.percent_discharge;                 // no need to convert for percentage
      
        // Save the current ideal gas state
        Tank_ideal_gas_state_en_.push_back(current_state);        // save in English units
        Tank_ideal_gas_state_si_.push_back(current_state_si);     // save in si units
      }
      
      
      std::cout << std::endl << "All temperatures in property file reached in tank state calculation with ideal gas after liquid depletes." << std::endl << std::endl;
      return 0;
    }
    
  }


  std::cout << std::endl << "All temperatures in property file reached in tank state calculation before liquid depletes." << std::endl << std::endl;
  return 0;
  
}





// ************************************************************
// Williamson method implementation part 2: pipe
// ************************************************************

/**
 * Takes tank state snapshot as input and calculates state along pipe due to pressure drop, saves in the Pipe_state_ vector.
 * Takes in quantities in SI units, but calculations and results are saved in English units.
 * 
 */
int williamson::pipe (double P, double T)
{
  
  // Convert storage conditions from SI units to English units
  P = P / 6894.76;                        // convert from Pa to psi
  T = (T - 273.15)*(9/5.0) + 32;          // convert from kelvin to fahrenheit
  
  
  
  // Temperature pre-processing
  
  // If pipe starting temperature is outside the temperature range of data file, abort the program.
  // Allow a small error 0.001 to deal with float number comparison issue: slight difference but essentially equal.
  if ((T < (temperature_.back() - 0.001)) || (T > (temperature_.front() + 0.001)))
  {
    std::cout << "Pipe temperature is outside the available temperature data! Adapt input." << std::endl;
    return 1;
  }
  
  
  // Do a temperature search. Find the block in data file that the pipe temperature belongs to.
  std::vector<double>::reverse_iterator T_iter = std::lower_bound (temperature_.rbegin(), temperature_.rend(), T);
  int count_T = temperature_.rend() - T_iter - 1;
  
  
  // Linear interpolation only if temperature is in the middle of block. No need to interpolate if it's very close to boundaries.
  // Use 0.001 as the threshold for vicinity.
  if ( (std::abs(T - temperature_[count_T]) > 0.001) && (std::abs(T - temperature_[count_T+1]) > 0.001) )
  {
    // print interpolation info if verbose
    if (verbose)
    {
      std::cout << "Interpolating and inserting temperature T=" << T << "(F) to line number " << count_T << " of the data." << std::endl << std::endl;
    }
    
    double linear_coeff = (T - temperature_[count_T+1]) / (temperature_[count_T] - temperature_[count_T+1]);      // coefficient for linear interpolation
    std::vector<double>::iterator it;         // vector iterator finds position in original data to insert interpolated properties at pipe temperature so that temperature remains sorted
    it = temperature_.begin() + count_T + 1;
    temperature_.insert ( it , T );
    it = vapor_p_.begin() + count_T + 1;
    vapor_p_.insert ( it , ((linear_coeff * (vapor_p_[count_T] - vapor_p_[count_T+1])) + vapor_p_[count_T+1]) );
    it = liquid_spec_vol_.begin() + count_T + 1;
    liquid_spec_vol_.insert ( it , ((linear_coeff * (liquid_spec_vol_[count_T] - liquid_spec_vol_[count_T+1])) + liquid_spec_vol_[count_T+1]) );
    it = vapor_spec_vol_.begin() + count_T + 1;
    vapor_spec_vol_.insert ( it , ((linear_coeff * (vapor_spec_vol_[count_T] - vapor_spec_vol_[count_T+1])) + vapor_spec_vol_[count_T+1]) );
    it = liquid_enthal_.begin() + count_T + 1;
    liquid_enthal_.insert ( it , ((linear_coeff * (liquid_enthal_[count_T] - liquid_enthal_[count_T+1])) + liquid_enthal_[count_T+1]) );
    it = vapor_enthal_.begin() + count_T + 1;
    vapor_enthal_.insert ( it , ((linear_coeff * (vapor_enthal_[count_T] - vapor_enthal_[count_T+1])) + vapor_enthal_[count_T+1]) );
    it = liquid_entro_.begin() + count_T + 1;
    liquid_entro_.insert ( it , ((linear_coeff * (liquid_entro_[count_T] - liquid_entro_[count_T+1])) + liquid_entro_[count_T+1]) );
    it = vapor_entro_.begin() + count_T + 1;
    vapor_entro_.insert ( it , ((linear_coeff * (vapor_entro_[count_T] - vapor_entro_[count_T+1])) + vapor_entro_[count_T+1]) );
    it = c_henry_.begin() + count_T + 1;
    c_henry_.insert ( it , ((linear_coeff * (c_henry_[count_T] - c_henry_[count_T+1])) + c_henry_[count_T+1]) );
    count_T++;      // step forward
  }
  else if ( std::abs(T - temperature_[count_T+1]) <= 0.001 )        // No need to interpolate if it's close to the lower bound temperature, but necessary to step forward.
  {
    count_T++;
  }
  // Otherwise it's close to the upper bound. Just use the property at current step and no need to do anything.
  
  
  
  // Initialize the pipe state
  pipe_state current_state,                                                                                 // define a current pipe state, in English units
             current_state_si;                                                                              // current pipe state in SI units
  double N = 0.01*P/c_henry_[count_T];                                                                      // nitrogen weight does not change at each step, which is different downstream position of the pipe
  
  // initial pipe state in English units
  current_state.temperature = temperature_[count_T];
  current_state.pressure = P + vapor_p_[count_T];
  current_state.liquid = 1;                                                                                 // expansion starts with unit mass of liquid agent and no vapor
  current_state.vapor = 0; 
  current_state.n_pressure = P;                                                                             // share tank nitrogen pressure at pipe beginning
  current_state.density = (1+N) / (liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P/c_henry_[count_T]));  // initial density (only liquid exists)
  
  // initial pipe state in SI units
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
    
    double L2, V2, P2, P3, E1, E2, E6, E7, E3, E4, E8, E9;            // intermediate variables
    int count_iter = 1;                                               // count iterations
    double P_inc = P_thres_/2.5;                                      // pressure step increment when not converged
    
    P2 = current_state.n_pressure;        // assumed new nitrogen pressure after discharge
    P3 = P2 - 1;                          // random initialization for P3
  
    // Intermediate parameters
    E1 = (1 + coeff_dissol_expan_*current_state.n_pressure/c_henry_[count_T-1]) * liquid_enthal_[count_T-1];
    E2 = (1 + current_state.n_pressure/vapor_p_[count_T-1]) * vapor_enthal_[count_T-1];
    E6 = 1 + 0.01*current_state.n_pressure/c_henry_[count_T-1];
    E7 = 1 + molecular_weight_ratio_*current_state.n_pressure/vapor_p_[count_T-1];
    
    
    // Converging pressure at one step
    while ( std::abs(P3 - P2) > P_thres_ )           // loop if pressure difference is larger than threshold
    {
      E3 = (1 + coeff_dissol_expan_*P2/c_henry_[count_T]) * liquid_enthal_[count_T];
      E4 = (1 + P2/vapor_p_[count_T]) * vapor_enthal_[count_T];
      E8 = 1 + 0.01*P2/c_henry_[count_T];
      E9 = 1 + molecular_weight_ratio_*P2/vapor_p_[count_T];
      
      L2 = (current_state.liquid*(E1*E9-E6*E4) + current_state.vapor*(E2*E9-E7*E4)) / (E3*E9-E8*E4);      // new liquid weight
      V2 = 1 - L2;                                                                                        // new vapor weight
      P3 = N / (L2*0.01/c_henry_[count_T] + V2*molecular_weight_ratio_/vapor_p_[count_T]);                // calculated new nitrogen pressure after expansion
  
      // print convergence info if verbose
      if (verbose)
      {
        std::cout << "Pipe iteration " << count_iter << " for temperature T=" << temperature_[count_T] << "(F): P2=" << P2 << "(psi), P3=" << P3 << "(psi)." << std::endl;
      }
    
      // Adapt P2 towards convergence
      if ( P3 < (P2 - P_thres_) )        P2 = P2 - P_inc;
      else if ( P3 > (P2 + P_thres_) )   P2 = P2 + P_inc;
      
      // If pressure drops to negative or too many iterations, result is not converging using current combination of threshold and increment.
      if ( (P2 < 0) || (P3 < 0) || (count_iter > 2*P/P_inc) )
      {
        std::cout << "Pipe state is not converging. Try again with a larger criterion." << std::endl;
        std::cout << "This can be done by changing the input of the \"set_pressure_threshold\" member function." << std::endl;
        return 2;
      }
      
      count_iter++;
    }
    
    
    
    // Update current pipe state in English units
    P2 = (P2 + P3)/2.0;
    current_state.temperature = temperature_[count_T];
    current_state.pressure = P2 + vapor_p_[count_T];
    current_state.liquid = L2;
    current_state.vapor = V2;
    current_state.n_pressure = P2;
    current_state.density = (1+N) / (L2*liquid_spec_vol_[count_T]*(1+coeff_dissol_expan_*P2/c_henry_[count_T]) + V2*vapor_spec_vol_[count_T]);
    
    // Update current pipe state in SI units
    current_state_si.temperature = (current_state.temperature - 32)/(9/5.0) + 273.15;     // F to K
    current_state_si.pressure = current_state.pressure * 6894.76;                         // psi to Pa
    current_state_si.liquid = current_state.liquid * 0.453592;                            // lb to kg
    current_state_si.vapor = current_state.vapor * 0.453592;                              // lb to kg
    current_state_si.n_pressure = current_state.n_pressure * 6894.76;                     // psi to Pa
    current_state_si.density = current_state.density * 16.0185;                           // lb/(ft^3) to kg/(m^3)
    
    // print converged current state to be saved if verbose
    if (verbose)
    {
      print_one_pipe_state_en(current_state);
    }
    
    // Make sure partial pressure of nitrogen is still positive in pipe
    if (P2 >= 0.0)          // Still positive pressure. Store the current state.
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
  
  
  std::cout << std::endl << "All temperatures in property file reached in pipe state calculation." << std::endl << std::endl;
  return 0;
  
}





/**
 * Loop through tank state discharge table (English units) and print each component in the vector.
 */
int williamson::print_tank_state_en()
{
  
  std::cout << "Tank discharge states with a storage temperature of " << Tank_state_en_[0].temperature << " (F), " 
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





/**
 * Loop through tank state discharge table (SI units) and print each component in the vector.
 */
int williamson::print_tank_state_si()
{
  
  std::cout << "Tank discharge states with a storage temperature of " << Tank_state_si_[0].temperature << " (K), " 
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





/**
 * Loop through pipe state expansion table (English units) and print each component in the vector.
 */
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





/**
 * Loop through pipe state expansion table (SI units) and print each component in the vector.
 */
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





/**
 * Loop through tank state discharge table (English units) and write each component in the vector to file.
 */
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





/**
 * Loop through tank state discharge table (SI units) and write each component in the vector to file.
 */
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





/**
 * Loop through pipe state expansion table (English units) and write each component in the vector to file.
 */
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





/**
 * Loop through pipe state expansion table (SI units) and write each component in the vector to file.
 */
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





/**
 * Interpolate at the given temperature according to tank state discharge table (English units).
 * Temperature input in SI units (K).
 */
tank_state williamson::get_tank_state_from_T_en(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  
  // If the quote temperature is outside the tank state temperature range, abort the program.
  if ((T < (Tank_state_en_.back().temperature - 0.001)) || (T > (Tank_state_en_.front().temperature + 0.001)))
  {
    std::cout << "Quote temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  
  // The temperature is inside range
  tank_state quote_state;
  
  // Use the lowest temperature tank state
  if ( std::abs(T - Tank_state_en_.back().temperature) <= 0.001 )
  {
    quote_state = Tank_state_en_.back();
  }
  
  // Use the highest temperature tank state
  else if ( std::abs(T - Tank_state_en_.front().temperature) <= 0.001 )
  {
    quote_state = Tank_state_en_.front();
  }
  
  // Find the block that the quote temperature belongs to and interpolate linearly
  // It's fine to interpolate even if temperature is close to bounds
  else
  {
    for (int i = 0; i < (Tank_state_en_.size()-1); i++)     // loop through all blocks
    {
      if ((T <= Tank_state_en_[i].temperature) && (T >= Tank_state_en_[i+1].temperature))
      {
        double linear_coeff = (T - Tank_state_en_[i+1].temperature) / (Tank_state_en_[i].temperature - Tank_state_en_[i+1].temperature);      // coefficient for linear interpolation
        quote_state.temperature = T;
        quote_state.pressure = (linear_coeff * (Tank_state_en_[i].pressure - Tank_state_en_[i+1].pressure)) + Tank_state_en_[i+1].pressure;
        quote_state.discharge = (linear_coeff * (Tank_state_en_[i].discharge - Tank_state_en_[i+1].discharge)) + Tank_state_en_[i+1].discharge;
        quote_state.liquid = (linear_coeff * (Tank_state_en_[i].liquid - Tank_state_en_[i+1].liquid)) + Tank_state_en_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Tank_state_en_[i].vapor - Tank_state_en_[i+1].vapor)) + Tank_state_en_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Tank_state_en_[i].n_pressure - Tank_state_en_[i+1].n_pressure)) + Tank_state_en_[i+1].n_pressure;
        quote_state.liquid_density = (linear_coeff * (Tank_state_en_[i].liquid_density - Tank_state_en_[i+1].liquid_density)) + Tank_state_en_[i+1].liquid_density;
        quote_state.percent_discharge = (linear_coeff * (Tank_state_en_[i].percent_discharge - Tank_state_en_[i+1].percent_discharge)) + Tank_state_en_[i+1].percent_discharge;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given temperature according to tank state discharge table (SI units).
 * Temperature input in SI units (K).
 */
tank_state williamson::get_tank_state_from_T_si(double T)
{
  
  // If the quote temperature is outside the tank state temperature range, abort the program.
  if ((T < (Tank_state_si_.back().temperature - 0.001)) || (T > (Tank_state_si_.front().temperature + 0.001)))
  {
    std::cout << "Quote temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  
  // The temperature is inside range
  tank_state quote_state;
  
  // Use the lowest temperature tank state
  if ( std::abs(T - Tank_state_si_.back().temperature) <= 0.001 )
  {
    quote_state = Tank_state_si_.back();
  }
  
  // Use the highest temperature tank state
  else if ( std::abs(T - Tank_state_si_.front().temperature) <= 0.001 )
  {
    quote_state = Tank_state_si_.front();
  }
  
  // Find the block that the quote temperature belongs to and interpolate linearly
  // It's fine to interpolate even if temperature is close to bounds
  else
  {
    for (int i = 0; i < (Tank_state_si_.size()-1); i++)     // loop through all blocks
    {
      if ((T <= Tank_state_si_[i].temperature) && (T >= Tank_state_si_[i+1].temperature))
      {
        double linear_coeff = (T - Tank_state_si_[i+1].temperature) / (Tank_state_si_[i].temperature - Tank_state_si_[i+1].temperature);      // coefficient for linear interpolation
        quote_state.temperature = T;
        quote_state.pressure = (linear_coeff * (Tank_state_si_[i].pressure - Tank_state_si_[i+1].pressure)) + Tank_state_si_[i+1].pressure;
        quote_state.discharge = (linear_coeff * (Tank_state_si_[i].discharge - Tank_state_si_[i+1].discharge)) + Tank_state_si_[i+1].discharge;
        quote_state.liquid = (linear_coeff * (Tank_state_si_[i].liquid - Tank_state_si_[i+1].liquid)) + Tank_state_si_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Tank_state_si_[i].vapor - Tank_state_si_[i+1].vapor)) + Tank_state_si_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Tank_state_si_[i].n_pressure - Tank_state_si_[i+1].n_pressure)) + Tank_state_si_[i+1].n_pressure;
        quote_state.liquid_density = (linear_coeff * (Tank_state_si_[i].liquid_density - Tank_state_si_[i+1].liquid_density)) + Tank_state_si_[i+1].liquid_density;
        quote_state.percent_discharge = (linear_coeff * (Tank_state_si_[i].percent_discharge - Tank_state_si_[i+1].percent_discharge)) + Tank_state_si_[i+1].percent_discharge;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given temperature according to tank ideal gas state table after liquid depletion (English units).
 * Temperature input in SI units (K).
 */
tank_state williamson::get_tank_ideal_gas_state_from_T_en(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  
  // If the quote temperature is outside the tank state temperature range, abort the program.
  if ((T < (Tank_ideal_gas_state_en_.back().temperature - 0.001)) || (T > (Tank_ideal_gas_state_en_.front().temperature + 0.001)))
  {
    std::cout << "Quote temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  
  // The temperature is inside range
  tank_state quote_state;
  
  // Use the lowest temperature tank state
  if ( std::abs(T - Tank_ideal_gas_state_en_.back().temperature) <= 0.001 )
  {
    quote_state = Tank_ideal_gas_state_en_.back();
  }
  
  // Use the highest temperature tank state
  else if ( std::abs(T - Tank_ideal_gas_state_en_.front().temperature) <= 0.001 )
  {
    quote_state = Tank_ideal_gas_state_en_.front();
  }
  
  // Find the block that the quote temperature belongs to and interpolate linearly
  // It's fine to interpolate even if temperature is close to bounds
  else
  {
    for (int i = 0; i < (Tank_ideal_gas_state_en_.size()-1); i++)     // loop through all blocks
    {
      if ((T <= Tank_ideal_gas_state_en_[i].temperature) && (T >= Tank_ideal_gas_state_en_[i+1].temperature))
      {
        double linear_coeff = (T - Tank_ideal_gas_state_en_[i+1].temperature) / (Tank_ideal_gas_state_en_[i].temperature - Tank_ideal_gas_state_en_[i+1].temperature);      // coefficient for linear interpolation
        quote_state.temperature = T;
        quote_state.pressure = (linear_coeff * (Tank_ideal_gas_state_en_[i].pressure - Tank_ideal_gas_state_en_[i+1].pressure)) + Tank_ideal_gas_state_en_[i+1].pressure;
        quote_state.discharge = (linear_coeff * (Tank_ideal_gas_state_en_[i].discharge - Tank_ideal_gas_state_en_[i+1].discharge)) + Tank_ideal_gas_state_en_[i+1].discharge;
        quote_state.liquid = (linear_coeff * (Tank_ideal_gas_state_en_[i].liquid - Tank_ideal_gas_state_en_[i+1].liquid)) + Tank_ideal_gas_state_en_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Tank_ideal_gas_state_en_[i].vapor - Tank_ideal_gas_state_en_[i+1].vapor)) + Tank_ideal_gas_state_en_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Tank_ideal_gas_state_en_[i].n_pressure - Tank_ideal_gas_state_en_[i+1].n_pressure)) + Tank_ideal_gas_state_en_[i+1].n_pressure;
        quote_state.liquid_density = (linear_coeff * (Tank_ideal_gas_state_en_[i].liquid_density - Tank_ideal_gas_state_en_[i+1].liquid_density)) + Tank_ideal_gas_state_en_[i+1].liquid_density;
        quote_state.percent_discharge = (linear_coeff * (Tank_ideal_gas_state_en_[i].percent_discharge - Tank_ideal_gas_state_en_[i+1].percent_discharge)) + Tank_ideal_gas_state_en_[i+1].percent_discharge;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given temperature according to tank state discharge table (SI units).
 * Temperature input in SI units (K).
 */
tank_state williamson::get_tank_ideal_gas_state_from_T_si(double T)
{
  
  // If the quote temperature is outside the tank state temperature range, abort the program.
  if ((T < (Tank_ideal_gas_state_si_.back().temperature - 0.001)) || (T > (Tank_ideal_gas_state_si_.front().temperature + 0.001)))
  {
    std::cout << "Quote temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  
  // The temperature is inside range
  tank_state quote_state;
  
  // Use the lowest temperature tank state
  if ( std::abs(T - Tank_ideal_gas_state_si_.back().temperature) <= 0.001 )
  {
    quote_state = Tank_ideal_gas_state_si_.back();
  }
  
  // Use the highest temperature tank state
  else if ( std::abs(T - Tank_ideal_gas_state_si_.front().temperature) <= 0.001 )
  {
    quote_state = Tank_ideal_gas_state_si_.front();
  }
  
  // Find the block that the quote temperature belongs to and interpolate linearly
  // It's fine to interpolate even if temperature is close to bounds
  else
  {
    for (int i = 0; i < (Tank_ideal_gas_state_si_.size()-1); i++)     // loop through all blocks
    {
      if ((T <= Tank_ideal_gas_state_si_[i].temperature) && (T >= Tank_ideal_gas_state_si_[i+1].temperature))
      {
        double linear_coeff = (T - Tank_ideal_gas_state_si_[i+1].temperature) / (Tank_ideal_gas_state_si_[i].temperature - Tank_ideal_gas_state_si_[i+1].temperature);      // coefficient for linear interpolation
        quote_state.temperature = T;
        quote_state.pressure = (linear_coeff * (Tank_ideal_gas_state_si_[i].pressure - Tank_ideal_gas_state_si_[i+1].pressure)) + Tank_ideal_gas_state_si_[i+1].pressure;
        quote_state.discharge = (linear_coeff * (Tank_ideal_gas_state_si_[i].discharge - Tank_ideal_gas_state_si_[i+1].discharge)) + Tank_ideal_gas_state_si_[i+1].discharge;
        quote_state.liquid = (linear_coeff * (Tank_ideal_gas_state_si_[i].liquid - Tank_ideal_gas_state_si_[i+1].liquid)) + Tank_ideal_gas_state_si_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Tank_ideal_gas_state_si_[i].vapor - Tank_ideal_gas_state_si_[i+1].vapor)) + Tank_ideal_gas_state_si_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Tank_ideal_gas_state_si_[i].n_pressure - Tank_ideal_gas_state_si_[i+1].n_pressure)) + Tank_ideal_gas_state_si_[i+1].n_pressure;
        quote_state.liquid_density = (linear_coeff * (Tank_ideal_gas_state_si_[i].liquid_density - Tank_ideal_gas_state_si_[i+1].liquid_density)) + Tank_ideal_gas_state_si_[i+1].liquid_density;
        quote_state.percent_discharge = (linear_coeff * (Tank_ideal_gas_state_si_[i].percent_discharge - Tank_ideal_gas_state_si_[i+1].percent_discharge)) + Tank_ideal_gas_state_si_[i+1].percent_discharge;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given discharge state according to tank state discharge table (English units).
 */
tank_state williamson::get_tank_state_from_percentdischarge_en(double percentdischarge)
{
  
  // If the quote discharge state is outside the range of discharge in tank state recession table, abort the program.
  // Allow a small error 1e-7 to deal with float number comparison issue: slight difference but essentially equal.
  if ((percentdischarge < (Tank_state_en_.front().percent_discharge - 1e-7)) || (percentdischarge > (Tank_state_en_.back().percent_discharge + 1e-7)))
  {
    std::cout << "Quote percentage of discharge is outside the tank state recession table!" << std::endl;
    abort();
  }
  
  
  // The discharge state is inside range
  tank_state quote_state;
  
  // Use the lowest discharge tank state
  if ( std::abs(percentdischarge - Tank_state_en_.front().percent_discharge) <= 1e-7 )
  {
    quote_state = Tank_state_en_.front();
  }
  
  // Use the highest discharge tank state
  else if ( std::abs(percentdischarge - Tank_state_en_.back().percent_discharge) <= 1e-7 )
  {
    quote_state = Tank_state_en_.back();
  }
  
  // Find the block that the quote discharge belongs to and interpolate linearly
  // It's fine to interpolate even if discharge is close to bounds
  else
  {
    for (int i = 0; i < (Tank_state_en_.size()-1); i++)     // loop through all blocks
    {
      if ((percentdischarge >= Tank_state_en_[i].percent_discharge) && (percentdischarge <= Tank_state_en_[i+1].percent_discharge))
      {
        double linear_coeff = (percentdischarge - Tank_state_en_[i+1].percent_discharge) / (Tank_state_en_[i].percent_discharge - Tank_state_en_[i+1].percent_discharge);      // coefficient for linear interpolation
        quote_state.temperature = (linear_coeff * (Tank_state_en_[i].temperature - Tank_state_en_[i+1].temperature)) + Tank_state_en_[i+1].temperature;
        quote_state.pressure = (linear_coeff * (Tank_state_en_[i].pressure - Tank_state_en_[i+1].pressure)) + Tank_state_en_[i+1].pressure;
        quote_state.discharge = (linear_coeff * (Tank_state_en_[i].discharge - Tank_state_en_[i+1].discharge)) + Tank_state_en_[i+1].discharge;
        quote_state.liquid = (linear_coeff * (Tank_state_en_[i].liquid - Tank_state_en_[i+1].liquid)) + Tank_state_en_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Tank_state_en_[i].vapor - Tank_state_en_[i+1].vapor)) + Tank_state_en_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Tank_state_en_[i].n_pressure - Tank_state_en_[i+1].n_pressure)) + Tank_state_en_[i+1].n_pressure;
        quote_state.liquid_density = (linear_coeff * (Tank_state_en_[i].liquid_density - Tank_state_en_[i+1].liquid_density)) + Tank_state_en_[i+1].liquid_density;
        quote_state.percent_discharge = percentdischarge;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given discharge state according to tank state discharge table (SI units).
 */
tank_state williamson::get_tank_state_from_percentdischarge_si(double percentdischarge)
{
  
  // If the quote discharge state is outside the range of discharge in tank state recession table, abort the program.
  // Allow a small error 1e-7 to deal with float number comparison issue: slight difference but essentially equal.
  if ((percentdischarge < (Tank_state_si_.front().percent_discharge - 1e-7)) || (percentdischarge > (Tank_state_si_.back().percent_discharge + 1e-7)))
  {
    std::cout << "Quote percentage of discharge is outside the tank state recession table!" << std::endl;
    abort();
  }
  
  
  // The discharge state is inside range
  tank_state quote_state;
  
  // Use the lowest discharge tank state
  if ( std::abs(percentdischarge - Tank_state_si_.front().percent_discharge) <= 1e-7 )
  {
    quote_state = Tank_state_si_.front();
  }
  
  // Use the highest discharge tank state
  else if ( std::abs(percentdischarge - Tank_state_si_.back().percent_discharge) <= 1e-7 )
  {
    quote_state = Tank_state_si_.back();
  }
  
  // Find the block that the quote discharge belongs to and interpolate linearly
  // It's fine to interpolate even if discharge is close to bounds
  else
  {
    for (int i = 0; i < (Tank_state_si_.size()-1); i++)     // loop through all blocks
    {
      if ((percentdischarge >= Tank_state_si_[i].percent_discharge) && (percentdischarge <= Tank_state_si_[i+1].percent_discharge))
      {
        double linear_coeff = (percentdischarge - Tank_state_si_[i+1].percent_discharge) / (Tank_state_si_[i].percent_discharge - Tank_state_si_[i+1].percent_discharge);      // coefficient for linear interpolation
        quote_state.temperature = (linear_coeff * (Tank_state_si_[i].temperature - Tank_state_si_[i+1].temperature)) + Tank_state_si_[i+1].temperature;
        quote_state.pressure = (linear_coeff * (Tank_state_si_[i].pressure - Tank_state_si_[i+1].pressure)) + Tank_state_si_[i+1].pressure;
        quote_state.discharge = (linear_coeff * (Tank_state_si_[i].discharge - Tank_state_si_[i+1].discharge)) + Tank_state_si_[i+1].discharge;
        quote_state.liquid = (linear_coeff * (Tank_state_si_[i].liquid - Tank_state_si_[i+1].liquid)) + Tank_state_si_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Tank_state_si_[i].vapor - Tank_state_si_[i+1].vapor)) + Tank_state_si_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Tank_state_si_[i].n_pressure - Tank_state_si_[i+1].n_pressure)) + Tank_state_si_[i+1].n_pressure;
        quote_state.liquid_density = (linear_coeff * (Tank_state_si_[i].liquid_density - Tank_state_si_[i+1].liquid_density)) + Tank_state_si_[i+1].liquid_density;
        quote_state.percent_discharge = percentdischarge;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given temperature according to pipe state expansion table (English units).
 * Temperature input in SI units (K).
 */
pipe_state williamson::get_pipe_state_en(double T)
{
  
  T = (T - 273.15)*(9/5.0) + 32;                            // convert from kelvin to fahrenheit
  
  // If the quote temperature is outside the pipe state temperature range, abort the program.
  if ((T < (Pipe_state_en_.back().temperature - 0.001)) || (T > (Pipe_state_en_.front().temperature + 0.001)))
  {
    std::cout << "Quote temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  
  // The temperature is inside range
  pipe_state quote_state;
  
  // Use the lowest temperature pipe state
  if ( std::abs(T - Pipe_state_en_.back().temperature) <= 0.001 )
  {
    quote_state = Pipe_state_en_.back();
  }
  
  // Use the highest temperature pipe state
  else if ( std::abs(T - Pipe_state_en_.front().temperature) <= 0.001 )
  {
    quote_state = Pipe_state_en_.front();
  }
  
  // Find the block that the quote temperature belongs to and interpolate linearly
  // It's fine to interpolate even if temperature is close to bounds
  else
  {
    for (int i = 0; i < (Pipe_state_en_.size()-1); i++)     // loop through all blocks
    {
      if ((T <= Pipe_state_en_[i].temperature) && (T >= Pipe_state_en_[i+1].temperature))
      {
        double linear_coeff = (T - Pipe_state_en_[i+1].temperature) / (Pipe_state_en_[i].temperature - Pipe_state_en_[i+1].temperature);      // coefficient for linear interpolation
        quote_state.temperature = T;
        quote_state.pressure = (linear_coeff * (Pipe_state_en_[i].pressure - Pipe_state_en_[i+1].pressure)) + Pipe_state_en_[i+1].pressure;
        quote_state.liquid = (linear_coeff * (Pipe_state_en_[i].liquid - Pipe_state_en_[i+1].liquid)) + Pipe_state_en_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Pipe_state_en_[i].vapor - Pipe_state_en_[i+1].vapor)) + Pipe_state_en_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Pipe_state_en_[i].n_pressure - Pipe_state_en_[i+1].n_pressure)) + Pipe_state_en_[i+1].n_pressure;
        quote_state.density = (linear_coeff * (Pipe_state_en_[i].density - Pipe_state_en_[i+1].density)) + Pipe_state_en_[i+1].density;
        break;
      }
    }
  }
  
  return quote_state;
  
}





/**
 * Interpolate at the given temperature according to pipe state expansion table (SI units).
 * Temperature input in SI units (K).
 */
pipe_state williamson::get_pipe_state_si(double T)
{
  
  // T comes in K

  // If the quote temperature is outside the pipe state temperature range, abort the program.
  if ((T < (Pipe_state_si_.back().temperature - 0.001)) || (T > (Pipe_state_si_.front().temperature + 0.001)))
  {
    std::cout << "Quote temperature is outside the available temperature data!" << std::endl;
    abort();
  }
  
  
  // The temperature is inside range
  pipe_state quote_state;
  
  // Use the lowest temperature pipe state
  if ( std::abs(T - Pipe_state_si_.back().temperature) <= 0.001 )
  {
    quote_state = Pipe_state_si_.back();
  }
  
  // Use the highest temperature pipe state
  else if ( std::abs(T - Pipe_state_si_.front().temperature) <= 0.001 )
  {
    quote_state = Pipe_state_si_.front();
  }
  
  // Find the block that the quote temperature belongs to and interpolate linearly
  // It's fine to interpolate even if temperature is close to bounds
  else
  {
    for (int i = 0; i < (Pipe_state_si_.size()-1); i++)     // loop through all blocks
    {
      if ((T <= Pipe_state_si_[i].temperature) && (T >= Pipe_state_si_[i+1].temperature))
      {
        double linear_coeff = (T - Pipe_state_si_[i+1].temperature) / (Pipe_state_si_[i].temperature - Pipe_state_si_[i+1].temperature);      // coefficient for linear interpolation
        quote_state.temperature = T;
        quote_state.pressure = (linear_coeff * (Pipe_state_si_[i].pressure - Pipe_state_si_[i+1].pressure)) + Pipe_state_si_[i+1].pressure;
        quote_state.liquid = (linear_coeff * (Pipe_state_si_[i].liquid - Pipe_state_si_[i+1].liquid)) + Pipe_state_si_[i+1].liquid;
        quote_state.vapor = (linear_coeff * (Pipe_state_si_[i].vapor - Pipe_state_si_[i+1].vapor)) + Pipe_state_si_[i+1].vapor;
        quote_state.n_pressure = (linear_coeff * (Pipe_state_si_[i].n_pressure - Pipe_state_si_[i+1].n_pressure)) + Pipe_state_si_[i+1].n_pressure;
        quote_state.density = (linear_coeff * (Pipe_state_si_[i].density - Pipe_state_si_[i+1].density)) + Pipe_state_si_[i+1].density;
        break;
      }
    }
  }
  
  return quote_state;
  
}




  
// *******************************************************
// Several stand along functions
// *******************************************************


/**
 * The tank state needs to be given in English units.
 */
void print_one_tank_state_en(tank_state quote_state)
{
  
  std::cout << "The quoted tank state has"
            << std::endl;
  
  std::cout << "temperature (F)" << ", " 
            << "pressure (psig)" << ", " 
            << "discharge (lb)" << ", " 
            << "liquid (lb)" << ", " 
            << "vapor (lb)" << ", " 
            << "n_pressure (psi)" << ", " 
            << "liquid_density (lbs/cu.ft.)" << ", " 
            << "percent_discharge" << std::endl;
            
  std::cout << quote_state.temperature << ", " 
            << quote_state.pressure - 14.7 << ", "   // transfer pressure from psi to psig
            << quote_state.discharge << ", " 
            << quote_state.liquid << ", " 
            << quote_state.vapor << ", " 
            << quote_state.n_pressure << ", " 
            << quote_state.liquid_density << ", " 
            << quote_state.percent_discharge << std::endl;
  
  std::cout << std::endl;
  
  return;
  
}
  


/**
 * The tank state needs to be given in SI units.
 */
void print_one_tank_state_si(tank_state quote_state)
{
  
  std::cout << "The quoted tank state has"
            << std::endl;
  
  std::cout << "temperature (K)" << ", " 
            << "pressure (Pa)" << ", " 
            << "discharge (kg)" << ", " 
            << "liquid (kg)" << ", " 
            << "vapor (kg)" << ", " 
            << "n_pressure (Pa)" << ", " 
            << "liquid_density (kg/(m^3))" << ", " 
            << "percent_discharge" << std::endl;
            
  std::cout << quote_state.temperature << ", " 
            << quote_state.pressure << ", "
            << quote_state.discharge << ", " 
            << quote_state.liquid << ", " 
            << quote_state.vapor << ", " 
            << quote_state.n_pressure << ", " 
            << quote_state.liquid_density << ", " 
            << quote_state.percent_discharge << std::endl;
  
  std::cout << std::endl;
  
  return;
  
}



/**
 * The pipe state needs to be given in English units.
 */
void print_one_pipe_state_en(pipe_state quote_state)
{
  
  std::cout << "The quoted pipe state has"
            << std::endl;
  
  std::cout << "temperature (F)" << ", " 
            << "pressure (psig)" << ", " 
            << "liquid (lb)" << ", " 
            << "vapor (lb)" << ", " 
            << "n_pressure (psi)" << ", " 
            << "density (lbs/cu.ft.)" << std::endl;
            
  std::cout << quote_state.temperature << ", " 
            << quote_state.pressure - 14.7 << ", "   // transfer pressure from psi to psig
            << quote_state.liquid << ", " 
            << quote_state.vapor << ", " 
            << quote_state.n_pressure << ", " 
            << quote_state.density << std::endl;
  
  std::cout << std::endl;
  
  return;
  
}
  


/**
 * The pipe state needs to be given in SI units.
 */
void print_one_pipe_state_si(pipe_state quote_state)
{
  
  std::cout << "The quoted pipe state has"
            << std::endl;
  
  std::cout << "temperature (K)" << ", " 
            << "pressure (Pa)" << ", " 
            << "liquid (kg)" << ", " 
            << "vapor (kg)" << ", " 
            << "n_pressure (Pa)" << ", " 
            << "density (kg/(m^3))" << std::endl;
              
  std::cout << quote_state.temperature << ", " 
            << quote_state.pressure << ", " 
            << quote_state.liquid << ", " 
            << quote_state.vapor << ", " 
            << quote_state.n_pressure << ", " 
            << quote_state.density << std::endl;
            
  std::cout << std::endl;
  
  return;
  
}


