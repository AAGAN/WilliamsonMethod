# WilliamsonMethod
c++ implementation of Williamson's method


Use g++ (GNU c++ compiler) instead of gcc (GNU c or c++) when linking files. Otherwise main function doesn't recognize the functions defined in other cpps.

Agent class defined. Each physical property is a vector read in from corresponding agent property file.

The output of the first part of the Williamson method would be a vector of tank_state structures.

The calculated pressure recession should match Figure H.1(b) of NFPA 12A for Halon 1301
