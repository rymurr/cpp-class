
#include "icgen.hpp"

namespace classical {

using boost::any_cast;     

SingleIC::SingleIC(){}

SingleIC::~SingleIC(){}

SingleIC::SingleIC(double var, int size): size_(size), var_(var){}

SingleLinIC::SingleLinIC(){}

SingleLinIC::~SingleLinIC(){}

SingleLinIC::SingleLinIC(double mean, double var, int size): SingleIC(var,size), mean_(mean){

    start_ = mean_ - 4.*sqrt((*this).var_);
    finish_ = mean_ + 4.*sqrt((*this).var_);
    dx_ = (finish_-start_)/(*this).size_;
    current_ = start_ - 0.5*dx_;

}

double SingleLinIC::RetVal(){
    current_ += dx_;
    return current_;
}

void SingleLinIC::reset(){
    current_ = start_ - 0.5*dx_;
}

SingleRandIC::SingleRandIC(){}

SingleRandIC::~SingleRandIC(){}

SingleRandIC::SingleRandIC(std::vector<double> means, double var): SingleIC(var,1), means_(means){

    (*this).size_ = means_.size();

    generator_.seed(static_cast<unsigned int>(std::time(0)));
    dist_ = boost::normal_distribution<>(0,(*this).var_);
    generators_ = boost::shared_ptr<boost::variate_generator<base_generator_type&, boost::normal_distribution<> > >( new boost::variate_generator<base_generator_type&, boost::normal_distribution<> >(generator_,dist_));

}

void SingleRandIC::RetVal(vTraj retVal){
    for (int i=0;i<(*this).size_;i++){
        (*retVal)[i] = (*generators_)()+means_[i];
    }
}



icgenerator::icgenerator(){}

icgenerator::~icgenerator(){}


icgenerator::icgenerator(anyMap params){

    means_ = any_cast<std::vector<double> >(params["means"]);
    trajs_ = any_cast<std::vector<int> >(params["dims"]);
    tType_ = any_cast<int>(params["dist-type"]);
    variance_= any_cast<double>(params["variance"]);
    tnumb_ = 1;
    for_each(trajs_.begin(), trajs_.end(), tnumb_*=boost::lambda::_1);
    lindone_ = false;

    try{
        single_ = any_cast<bool>(params["nobuild"]);
    } catch (boost::bad_any_cast &e) {
        single_ = false;
    }

    if (means_.size() != trajs_.size()){
        throw  std::range_error("the means and dims arrays are not the same size!");
    }

    (*this).singleCheck();
    if (single_ == false){
    	(*this).gen_ics();
    }
    j_=0;
    tsing_.resize(trajs_.size(),0);
}

void icgenerator::get_ic(vTraj ics){
    if (single_ == true) {
        (*this).gpick(ics);
    } else {
        (*this).hpick(ics);
    }
}

void icgenerator::singleMCRet(vTraj ics){
    for (unsigned int i=0;i<trajs_.size();i++){
        (*ics)[i] = initConditions_[i][j_];
    }
    j_++;
}

void icgenerator::singleLinRet(vTraj ics){

    if (!lindone_){
        typedef ic_array::index index;
        ic_array::index_gen indices;

        for (index i=0;i<(int)trajs_.size();++i)
          sliceVec_.push_back(initConditions_[ indices[i][range(0,trajs_.size())] ]);

        for (unsigned int i=0;i<trajs_.size();++i){
          sliceIter_.push_back(sliceVec_[i].begin());
          (*ics)[i] = *sliceIter_[i];
        }
        lindone_=true;
        return;
    }


    ++sliceIter_[0];
    for (unsigned int j=0;j<trajs_.size()-1;++j){
      if (sliceIter_[j] == sliceVec_[j].end()){
        sliceIter_[j] = sliceVec_[j].begin();
        ++sliceIter_[j+1];
      }
      (*ics)[j] = *sliceIter_[j];
    }
    if (sliceIter_[sliceIter_.size()-1] == sliceVec_[sliceVec_.size()-1].end())
      throw std::range_error("Reached the end of the trajectories");
    (*ics)[trajs_.size()-1] = *sliceIter_[trajs_.size()-1];

}

void icgenerator::gen_ics(){
    (*this).fpick();
}

void icgenerator::ret_ics(boost::shared_ptr<ic_array> icPtr){
    *icPtr = initConditions_;
}

void icgenerator::singleMCFill(vTraj ics){
    if (tnumb_ > 0){
        gens_[0]->RetVal(ics); 
        tnumb_--;
    } else {
        throw std::range_error("Reached the end of the trajectories"); 
    }

}

void icgenerator::singleLinFill(vTraj ics){
    
    if (!lindone_){
        for (unsigned int i=0;i<gens_.size();i++){
            (*ics)[i] = gens_[i]->RetVal();
            tsing_[i]+=1;
        }
        lindone_=true;
        return;
    }

    if (tsing_[0] >= trajs_[0]){
        tsing_[0]=0;
        gens_[0]->reset();
        (*ics)[1] = gens_[1]->RetVal();
        tsing_[1]++;
    }

    for (unsigned int i=1;i<tsing_.size()-1;i++){
        if (tsing_[i] > trajs_[i]){
            tsing_[i]=1;
            gens_[i]->reset();
            (*ics)[i] = gens_[i]->RetVal();
            (*ics)[i+1] = gens_[i+1]->RetVal();
            tsing_[i+1]++;
        }

    }

    (*ics)[0] = gens_[0]->RetVal();
    tsing_[0]++;

    if (tsing_[tsing_.size()-1] > trajs_[trajs_.size()-1])
        throw std::range_error("Reached the end of the trajectories"); 
}

void icgenerator::linearFill(){
    
	boost::progress_display show_progress(trajs_.size(), std::clog);
    for (unsigned int i=0;i<trajs_.size();i++){
            SingleLinIC uni(means_[i],variance_,trajs_[i]);
        for (int j=0;j<trajs_[i];j++){
            initConditions_[i][j] = uni.RetVal();
        }
        ++show_progress;
    }
}

void icgenerator::montecarloFill(){

	int tot = std::accumulate(trajs_.begin(),trajs_.end(),1,std::multiplies<int>());

    SingleRandIC uni(means_,variance_);
    vTraj rands = vTraj(new std::vector<double>(means_));
    boost::progress_display show_progress(tot, std::clog);
    for (int j=0;j<tot;j++){
            uni.RetVal(rands);
        for (unsigned int i=0;i<trajs_.size();i++){
            initConditions_[i][j] = (*rands)[i];
        }
        ++show_progress;
    }

}

void icgenerator::singleCheck(){
	if (single_ == false){
        LOG(INFO) << "the icgenerator will build a full set of ICs and store them for later use";
        (*this).tTypeSwitchF();
    } else {
    	LOG(INFO) << "The icgenerator is generating ICs on the fly and will not store them";
    	(*this).tTypeSwitchT();
    }
}

void icgenerator::tTypeSwitchT(){
    switch (tType_){
        case 1:
            gens_.push_back(boost::shared_ptr<SingleRandIC>(new SingleRandIC(means_,variance_)));
            gpick = boost::bind(&icgenerator::singleMCFill,this,_1);
            break;
        case 2:
            for(unsigned int i=0;i<trajs_.size();i++){
                gens_.push_back(boost::shared_ptr<SingleLinIC>(new SingleLinIC(means_[i],variance_,trajs_[i])));
            }
            gpick = boost::bind(&icgenerator::singleLinFill,this,_1);
            break;
         case 3:
            LOG(FATAL) << "staged-linear is currently not implemented";
        default:
            LOG(FATAL) << "Bad choice for Initial conditions";
    }
}

void icgenerator::tTypeSwitchF(){
	switch (tType_){
	            case 1:
	                fpick = boost::bind(&icgenerator::montecarloFill,this);
	                hpick = boost::bind(&icgenerator::singleMCRet,this,_1);
	                int tot = std::accumulate(trajs_.begin(),trajs_.end(),1,std::multiplies<int>());
	                initConditions_.resize(boost::extents[trajs_.size()][tot]);
	                break;
	            case 2:
	                fpick = boost::bind(&icgenerator::linearFill,this);
	                hpick = boost::bind(&icgenerator::singleLinRet,this,_1);
	                int max = *(std::max_element(trajs_.begin(),trajs_.end()));
	                initConditions_.resize(boost::extents[trajs_.size()][max]);
	                break;
	            case 3:
	                LOG(FATAL) << "staged-linear is currently not implemented";
	            default:
	                LOG(FATAL) << "Bad choice for Initial conditions";
	        }
}

void icgenerator::save(std::string sType, std::string fName){

    std::string binary="binary", text="text";
    std::ofstream ofs(fName.c_str());

    if (sType.compare(binary) == 0){
        boost::archive::binary_oarchive oa(ofs);
        oa << *this;
    } else if (sType.compare(text) == 0) {
        boost::archive::text_oarchive oa(ofs);
        oa << *this;
    } else {
        std::string throwArg="archive type must be binary or text not" + sType;
        throw std::invalid_argument(throwArg);
    }

}

void icgenerator::load(std::string sType, std::string fName){

    std::string binary="binary", text="text";
    std::ifstream ifs(fName.c_str());

    if (sType.compare(binary) == 0){
        boost::archive::binary_iarchive ia(ifs);
        ia >> *this;
    } else if (sType.compare(text) == 0) {
        boost::archive::text_iarchive ia(ifs);
        ia >> *this;
    } else {
        std::string throwArg="archive type must be binary or text not" + sType;
        throw std::invalid_argument(throwArg);
    }
}

}
