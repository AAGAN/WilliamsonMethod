#include "agent.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h> // atof or strtof

// using namespace std;


// class default constructor
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
}


// class destructor
agent::~agent()
{
    
}
  
  
// class constructor based on agent type
// Remember to use the namespace std:: !!
agent::agent(int i)
{
    
    if (i==1) // read in property data for Halon 1301
    {
        
        

    }
    
    else if (i==2) // read in property data for Novec 1230
    {
        
    }
    
    else // Neither Halon nor Novec. Bad input.
    {
        std::cout << "Wrong agent indicator!" << std::endl;
    }
        
        
    std::ifstream infile( "Halon1301_property.csv" ); // input data stream from file
    
    while (infile)
    {
        std::string input_line, one_number;
        getline(infile, input_line); // get the string of one line into input_line
        std::istringstream num_list(input_line); // a stream of this line
        
        // Push one line of data into each property vector
        getline(num_list, one_number, ',');
        // double current_num = atof(one_number.c_str()); // c_str() adds a terminating null-character at the end
        // double current_num = strtof(one_number.c_str(), NULL); // strtof does the same, but also capable of multiple floats in one string.
        temperature_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_p_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        liquid_spec_vol_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_spec_vol_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        liquid_enthal_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_enthal_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        liquid_entro_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        vapor_entro_.push_back(atof(one_number.c_str()));
        getline(num_list, one_number, ',');
        c_henry_.push_back(atof(one_number.c_str()));
        
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
