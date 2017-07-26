#include "agent.hpp"
#include <iostream>
#include <fstream>      // stream from file
#include <sstream>      // istringstream
#include <string>
#include <stdlib.h>     // atof or strtof

// using namespace std;
// Remember to add the namespace std:: after commenting this out !!



//! class default constructor
agent::agent()
{
    // temperature_.push_back(0.0);
    // vapor_p_.push_back(0.0);
    // liquid_spec_vol_.push_back(0.0);
    // vapor_spec_vol_.push_back(0.0);
    // liquid_enthal_.push_back(0.0);
    // vapor_enthal_.push_back(0.0);
    // liquid_entro_.push_back(0.0);
    // vapor_entro_.push_back(0.0);
    // c_henry_.push_back(0.0);
    // molecular_weight_ratio_ = 0.0;
    // coeff_dissol_expan_ = 0.0;
}



//! class destructor
agent::~agent()
{
    
}
  
  
  
// Agent class constructor
// No longer use agent indicator. Agent is specified through the property file.
agent::agent(std::string property_file_name, double molecular_weight_ratio, double coeff_dissol_expan)
{
    
    // Assign the two numerical properties
    molecular_weight_ratio_ = molecular_weight_ratio;
    coeff_dissol_expan_ = coeff_dissol_expan;
    

    // Read property file
    std::ifstream infile(property_file_name.c_str());           // input data stream from file
        
        
    // Property file empty or not exist. Abort the program.
    if(!infile.good()) 
    {
        std::cout << "Error with the file! Nonexistent, empty, or else." << std::endl;
        abort();
        // exit (EXIT_FAILURE);
    }
        
        
    // Keep reading the file line by line until the end
    while (infile.good())                                       // "!infile.eof()" works too, but only "infile" would read an extra empty line at the end.
    {
        std::string input_line, one_number;
        getline(infile, input_line);                            // get the string of one line into input_line
        std::istringstream num_list(input_line);                // a stream of this line
        
        // Input one line of data into each property vector.
        // Instead of pushing back, use insert to the front since temperature data in file is in an ascending order.
        // After inserting these vectors hold properties in a temperature-descending order.
            
        getline(num_list, one_number, ',');
        temperature_.insert(temperature_.begin(), atof(one_number.c_str()));
        // double current_num = atof(one_number.c_str());               // c_str() adds a terminating null-character at the end
        // double current_num = strtof(one_number.c_str(), NULL);       // strtof does the same, but also capable of multiple floats in one string.
        // temperature_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_p_.insert(vapor_p_.begin(), atof(one_number.c_str()));
        // vapor_p_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        liquid_spec_vol_.insert(liquid_spec_vol_.begin(), atof(one_number.c_str()));
        // liquid_spec_vol_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_spec_vol_.insert(vapor_spec_vol_.begin(), atof(one_number.c_str()));
        // vapor_spec_vol_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        liquid_enthal_.insert(liquid_enthal_.begin(), atof(one_number.c_str()));
        // liquid_enthal_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_enthal_.insert(vapor_enthal_.begin(), atof(one_number.c_str()));
        // vapor_enthal_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        liquid_entro_.insert(liquid_entro_.begin(), atof(one_number.c_str()));
        // liquid_entro_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_entro_.insert(vapor_entro_.begin(), atof(one_number.c_str()));
        // vapor_entro_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        c_henry_.insert(c_henry_.begin(), atof(one_number.c_str()));
        // c_henry_.push_back(atof(one_number.c_str()));
        // The linear relation between Henry's constant and temperature for Novec is already appended in its data file.
        // c_henry_.insert(c_henry_.begin(), 0.9286*temperature_.front() + 3494.6);
        // c_henry_.insert(c_henry_.begin(), 3600);
        
        // Expect 9 numbers from each line
        if (!num_list.eof())
        {
            std::cout << "Unexpected: didn't reach end of line!" << std::endl;
        }
    }
    
    
    // Expect to reach end of file
    if (!infile.eof())
    {
        std::cout << "Unexpected: didn't reach end of file!" << std::endl;
    }

    
}
