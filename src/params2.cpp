/*
 * params2.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: ryanmurray
 */
#include "params2.hpp"

state_param::state_param(std::string desc, std::string varName, int defVal, std::map<int,std::string> input): param<int>(desc,varName,defVal){
    typedef std::map<int,std::string> valMap;
    legalVals = boost::shared_ptr<valMap>(new valMap(input));
    std::stringstream out;
    out << "\n\t";
    std::for_each((*legalVals).begin(), (*legalVals).end(),
            out << bind(&valMap::value_type::first,_1) 
            << ". " 
            << bind(&valMap::value_type::second,_1) + "\n\t");    
    *description += out.str();

                  
}

void state_param::verify(){
    std::map<int,std::string>::iterator it;
    it = (*legalVals).find(*actualValue);
    if (it != (*legalVals).end()){
        ident = boost::shared_ptr<std::string>(new std::string(it->second));
        std::cout << "For " << *name << " option " << *actualValue<< " is being used, which is " + *ident << std::endl;
    }
    else {
        throw std::invalid_argument(boost::lexical_cast<std::string>(*actualValue) + " does not have a valid meaning. Possible values are:\n" + *description);
    }
}

void state_param::print(){
    std::cout << *description << " " 
        << *name << " " 
        << *defaultValue << " " 
        << *actualValue << " " 
        << *ident << std::endl;
}

