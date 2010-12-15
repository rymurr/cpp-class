/*
 * params2.hpp
 *
 *  Created on: Dec 14, 2010
 *      Author: ryanmurray
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>


//#include <boost/assign/list_inserter.hpp>
//#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
//#include <boost/bimap.hpp>

//#include "input_func.hpp"

#define foreach     BOOST_FOREACH

using boost::any_cast;

typedef std::map<std::string,boost::any> anyMap;
//typedef boost::bimap< int, std::string > bm_type;

template <class T>
class param {
	protected:
		std::string description, category, name;
		T defaultValue;

	public:
		param(std::string desc, std::string cat, std::string varName, T defVal){
			description = desc;
			category = cat;
			name = varName;
			defaultValue = defVal;
		}

		void virtual print();
};

template <class T>
void param<T>::print(){
	std::cout << description << category << name << defaultValue <<std::endl;
}

class run_param: public param<double> {
	private:
		double actual,max,min;

	public:
		run_param(std::string desc, std::string cat, std::string varName, double defVal, double act, double maximum, double minimum): param<double>(desc,cat,varName,defVal){
			actual = act;
			max = maximum;
			min = minimum;
		}

		void verify(){
			if (actual >= min && actual <= max){
				std::cout << name << " is within the allowed range" << std::endl;
			}
			else {
				throw std::invalid_argument(name + " is not in the valid range.\n");
			}
		}

		void print(){
			std::cout << description << category << name << defaultValue << actual << max << min << std::endl;
		}
};

class state_param: public param<int> {
	private:
	    std::map<int,std::string> legalVals;
	    int actual;
	    std::string ident;

	public:
		state_param(std::string desc, std::string cat, std::string varName, int defVal, int act, std::map<int,std::string> input): param<int>(desc,cat,varName,defVal){
			actual = act;
			legalVals = input;
		}

		void verify(){
			std::map<int,std::string>::iterator it;
			it = legalVals.find(actual);
			ident = it->second;
			if (it != legalVals.end()){
				std::cout << "For " << name << " option " << actual << " is being used, which is " + ident << std::endl;
			}
			else {
				throw std::invalid_argument(boost::lexical_cast<std::string>(actual) + " does not have a valid meaning.\n");
			}
		}

		void print(){
			std::cout << description << category << name << defaultValue << actual << ident << std::endl;
		}
};
#endif
