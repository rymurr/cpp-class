/*
 * params2.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: ryanmurray
 */
#include "params2.hpp"

state_param::state_param(std::string desc, std::string cat, std::string varName, int defVal, int act, std::map<int,std::string> input): param<int>(desc,cat,varName,defVal){
    actual = act;
    legalVals = input;
    typedef std::map<int,std::string> valMap;
    std::stringstream out;
    out << "\n\t";
    std::for_each(legalVals.begin(), legalVals.end(),
            out << bind(&valMap::value_type::first,_1) 
            << ". " 
            << bind(&valMap::value_type::second,_1) + "\n\t");    
    description += out.str();

                  
}

void state_param::verify(){
    std::map<int,std::string>::iterator it;
    it = legalVals.find(actual);
    if (it != legalVals.end()){
        ident = it->second;
        std::cout << "For " << name << " option " << actual << " is being used, which is " + ident << std::endl;
    }
    else {
        throw std::invalid_argument(boost::lexical_cast<std::string>(actual) + " does not have a valid meaning. Possible values are:\n" + description);
    }
}

void state_param::print(){
    std::cout << description << " " 
        << category << " " 
        << name << " " 
        << defaultValue << " " 
        << actual << " " 
        << ident << std::endl;
}

