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
#include <algorithm>
#include <sstream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include "customGlog.hpp"

#include "exceptions.hpp"
namespace classical{

#define foreach     BOOST_FOREACH
#define BOOST_FILESYSTEM_VERSION 3

using boost::any_cast;
namespace po = boost::program_options;


namespace fs = boost::filesystem;

typedef std::map<std::string,boost::any> anyMap;

class param {
    protected:
        boost::shared_ptr<std::string> description, name;

    public:

        param(std::string desc, std::string nameVal){
            description = boost::shared_ptr<std::string>(new std::string(desc));
            name = boost::shared_ptr<std::string>(new std::string(nameVal));
        };

        virtual ~param(){};

        boost::any virtual verify(){return boost::any(0);};

		void virtual print(){};

        void virtual set(boost::any){};
        
        virtual std::string getName(){return *name;};

        virtual std::string getDesc(){return *description;};

        void virtual genParam(po::options_description&){};


};

template <class S>
class run_param: public param {
    private:
        boost::shared_ptr<S> actualValue;
        boost::shared_ptr<S> defaultValue;
		boost::shared_ptr<S> max,min;

    public:
		run_param(std::string desc, std::string varName, S defVal, S maximum, S minimum):param(desc, varName){
	    defaultValue = boost::shared_ptr<S>(new S(defVal));
        max = boost::shared_ptr<S>(new S(maximum));
        min = boost::shared_ptr<S>(new S(minimum));
        }

		boost::any verify(){
            if (*actualValue >= *min && *actualValue <= *max){
                LOG(INFO) << *name << " is within the allowed range" << std::endl;
            }
            else {
                throw std::invalid_argument(*name + " is not in the valid range.\n");
            }
            return boost::any(*actualValue);
        }

		void print(){
            std::cout << *description << " "
                << *name << " "
                << *defaultValue << " "
                << *actualValue << " "
                << *max << " " 
                << *min << std::endl;
        }

		void set(boost::any act){
		    actualValue = boost::shared_ptr<S>(new S(boost::any_cast<S>(act)));
		}

        void virtual genParam(po::options_description &desc){
            desc.add_options()
                (((*this).name)->c_str(), po::value<S>()->default_value(*defaultValue),((*this).description)->c_str());
        }
};


class state_param: public param {
    private:
        boost::shared_ptr<int> actualValue;
        boost::shared_ptr<int> defaultValue;
        boost::shared_ptr<std::map<int,std::string> > legalVals;
        boost::shared_ptr<std::string> ident;

    public:
        state_param(std::string, std::string, int, std::map<int,std::string>);

        void virtual genParam(po::options_description &desc){
            desc.add_options()
                (((*this).name)->c_str(), po::value<int>()->default_value(*defaultValue),((*this).description)->c_str());
        }

		boost::any verify();

		void print();

		void set(boost::any);
};

class file_param: public param {
    private:
        boost::shared_ptr<std::string> actualValue;
        boost::shared_ptr<std::string> defaultValue;

	public:
		file_param(std::string desc, std::string varName, std::string defVal):param(desc,varName){
	        defaultValue = boost::shared_ptr<std::string>(new std::string(defVal));
		};

        void virtual genParam(po::options_description &desc){
            desc.add_options()
                (((*this).name)->c_str(), po::value<std::string>()->default_value(*defaultValue),((*this).description)->c_str());
        }

		boost::any verify();

		void print();

		void set(boost::any);
};

template <class R>
class list_param: public param {
    private:
        boost::shared_ptr<std::string> actualValue;
        boost::shared_ptr<std::string> defaultValue;
        boost::shared_ptr<int> size;
        boost::shared_ptr<std::vector<R> > pArray;

    public:
        list_param(std::string desc, std::string varName, std::string defVal, boost::shared_ptr<int> sz):param(desc,varName){
            defaultValue = boost::shared_ptr<std::string>(new std::string(defVal));
            size =sz;
        }

        void virtual genParam(po::options_description &desc){
            desc.add_options()
                (((*this).name)->c_str(), po::value<std::string>()->default_value(*defaultValue),((*this).description)->c_str());
        }

        boost::any verify();

		void print(){
            std::cout << *description << " " 
                << *name << " " 
                << *defaultValue << " "
                << *actualValue << " "
                << *size << " ";
            foreach(R i, *pArray){
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }

        list_param(const list_param &a){
            description = a.description;
            name = a.name;
            actualValue = a.actualValue;
            defaultValue = a.defaultValue;
            size = a.size;
            pArray = a.pArray;
        }

        void set(boost::any act){
            actualValue = boost::shared_ptr<std::string>(new std::string(boost::any_cast<std::string>(act)));
        }
};

template <class R>
boost::any list_param<R>::verify(){

    using namespace boost;
    typedef std::vector< std::string > split_vector_type;
    split_vector_type splitStr;
    split(splitStr, *actualValue, is_any_of(","), token_compress_on);

    if ((int)splitStr.size() > *size){
        LOG(WARNING) <<  "\n***WARNING! The number of elements for the list given for " + *name + " exceeds the number of dimensions given. The list is being truncated to " + boost::lexical_cast<std::string>(*size) + ".***\n" << std::endl;
    }
    if ((int)splitStr.size() < *size){
        throw std::invalid_argument("The number of elements for the list given for " + *name + " is too short, at least " + boost::lexical_cast<std::string>(*size) + " are needed.");
    }

    pArray = boost::shared_ptr<std::vector<R> >(new std::vector<R>);
    for(int i=0; i<*size; i++)
    {
        (*pArray).push_back(boost::lexical_cast<R>(splitStr[i]));
    }
    return boost::any(*pArray);
}


}
#endif


