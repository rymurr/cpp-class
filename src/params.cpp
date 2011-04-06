/*
 * params2.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: ryanmurray
 */
#include "params.hpp"
namespace classical {

state_param::state_param(std::string desc, std::string varName, int defVal, std::map<int,std::string> input):param(desc, varName){
    typedef std::map<int,std::string> valMap;
    legalVals = boost::shared_ptr<valMap>(new valMap(input));
    defaultValue = boost::shared_ptr<int>(new int(defVal));
    std::stringstream out;
    out << "\n\t";
    std::for_each((*legalVals).begin(), (*legalVals).end(),
            out << boost::lambda::bind(&valMap::value_type::first,boost::lambda::_1) 
            << ". " 
            << boost::lambda::bind(&valMap::value_type::second,boost::lambda::_1) + "\n\t");    
    *description += out.str();

                  
}

boost::any state_param::verify(){
    if (!actualValue)
        actualValue = defaultValue;
    std::map<int,std::string>::iterator it;
    it = (*legalVals).find(*actualValue);
    if (it != (*legalVals).end()){
        ident = boost::shared_ptr<std::string>(new std::string(it->second));
        LOG(INFO) << "For " << *name << " option " << *actualValue<< " is being used, which is " + *ident << std::endl;
    }
    else {
        throw invalid_input() << err_info(boost::lexical_cast<std::string>(*actualValue) + " does not have a valid meaning. Possible values are:\n" + *description);
    }
    return boost::any(*actualValue);
}

void state_param::print(){
    std::cout << *description << " " 
        << *name << " " 
        << *defaultValue << " " 
        << *actualValue << " " 
        << *ident << std::endl;
}

void state_param::set(boost::any act){
    actualValue = boost::shared_ptr<int>(new int(boost::any_cast<int>(act)));
}

void file_param::print(){
    std::cout << *description << " "
        << *name << " "
        << *defaultValue << " "
        << *actualValue << " "
        << std::endl;
}

boost::any file_param::verify(){
    if (!actualValue)
        actualValue = defaultValue;
    fs::path filename(*actualValue);
    fs::path file = filename.parent_path();

    if (file.empty() || (fs::exists(file) && fs::is_directory(file))){
        LOG(INFO) << "The path " + *actualValue + " is valid for " + *name;
    } else {
        throw invalid_input() << err_info(*actualValue + " is not a valid file name");
    }
    return boost::any(*actualValue);
}

void file_param::set(boost::any act){
    actualValue = boost::shared_ptr<std::string>(new std::string(boost::any_cast<std::string>(act)));
}

}
