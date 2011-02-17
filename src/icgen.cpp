
#include "icgen.hpp"

namespace classical {

using boost::any_cast;     

SingleGrid::SingleGrid(double mean, double var, int size): mean_(mean), var_(var), size_(size){
    start_ = mean_ - 4.*sqrt(var_);
    finish_ = mean_ + 4.*sqrt(var_);
    dx_ = (finish_ - start_)/((double) size_);
    current_ = start_ - 0.5*dx_;
    finish_ -= dx_;
}

std::pair<double,bool> SingleGrid::operator()(){
    current_ += dx_;
    if (current_  >= finish_){
        double old = current_;
        current_ = start_ - 0.5*dx_;
        return std::make_pair(old,true);
    } else {
        return std::make_pair(current_,false);
    }
}

SingleIC::SingleIC(){}

SingleIC::~SingleIC(){}

SingleIC::SingleIC(std::vector<double> mean, double var): var_(var), mean_(mean){}

SingleLinIC::SingleLinIC(){}

SingleLinIC::~SingleLinIC(){}

SingleLinIC::SingleLinIC(std::vector<double> means, double var, std::vector<int> sizes): SingleIC(means,var), sizes_(sizes) {

    (*this).size_ = (*this).mean_.size();
    for(int i=0;i<(*this).size_;i++){
    	grids_.push_back(SingleGrid(means[i],var,sizes[i]));
    	current_.push_back(std::make_pair(0.,false));
    }
    std::transform(grids_.begin()+1,grids_.end(),current_.begin()+1,boost::lambda::bind(&SingleGrid::operator(),boost::lambda::_1));
}

void SingleLinIC::RetVal(vTraj retVal){

	int i=0;
	bool exit = false;
	while (!exit && i<(*this).size_){

		if (!current_[i].second)
			exit = true;
		current_[i] = grids_[i]();
		++i;
	}
	std::transform(current_.begin(),current_.end(),(*retVal).begin(), boost::lambda::bind(&std::pair<double,bool>::first,boost::lambda::_1));

}

/*
void SingleLinIC::reset(int i){
    current_[i] = start_[i] - 0.5*dx_[i];
}
*/

SingleRandIC::SingleRandIC(){}

SingleRandIC::~SingleRandIC(){}

SingleRandIC::SingleRandIC(std::vector<double> means, double var): SingleIC(means, var){

    (*this).size_ = (*this).mean_.size();
    generator_.seed(static_cast<unsigned int>(std::time(0)));
    dist_ = boost::normal_distribution<>(0,(*this).var_);
    generators_ = boost::shared_ptr<boost::variate_generator<base_generator_type&, boost::normal_distribution<> > >( new boost::variate_generator<base_generator_type&, boost::normal_distribution<> >(generator_,dist_));

}

void SingleRandIC::RetVal(vTraj retVal){
    //TODO: turn this into an algorithm transform or so
    for (int i=0;i<(*this).size_;i++){
        (*retVal)[i] = (*generators_)()+(*this).mean_[i];
    }
}



icgenerator::icgenerator(){}

icgenerator::~icgenerator(){}


icgenerator::icgenerator(anyMap params){

    means_ = any_cast<std::vector<double> >(params["means"]);
    trajs_ = any_cast<std::vector<int> >(params["dims"]);
    tType_ = any_cast<int>(params["dist-type"]);
    variance_= any_cast<double>(params["variance"]);
    tnumb_ = std::accumulate(trajs_.begin(),trajs_.end(),1,std::multiplies<int>());
    tdim_ = trajs_.size();
    //lindone_ = false;
    j_=0;
    //tsing_.resize(trajs_.size(),0);


    try{
        single_ = any_cast<bool>(params["nobuild"]);
    } catch (boost::bad_any_cast &e) {
        single_ = false;
    }

    if (means_.size() != trajs_.size()){
        throw  std::range_error("the means and dims arrays are not the same size!");
    }

    (*this).singleCheck();
    if (!single_){
        initConditions_.resize(boost::extents[tdim_][tnumb_]);
    	(*this).montecarloFill();
//    	(*this).gen_weights(any_cast<boost::function<void (anyMap)> >params["weight_func"], params);
    }

}

void icgenerator::gen_weights(boost::function<void (anyMap)> wFunc, anyMap params){
    ;
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

/*
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
    (*this).montecarloFill();
}
*/

void icgenerator::ret_ics(boost::shared_ptr<ic_array> icPtr){
    *icPtr = initConditions_;
}

/*
void icgenerator::singleLinFill(vTraj ics){
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
    
    LOG(INFO) << "Building linear ICs";
	boost::progress_display show_progress(trajs_.size(), LOG(INFO));
    for (unsigned int i=0;i<trajs_.size();i++){
            SingleLinIC uni(means_,variance_,trajs_[i]);
        for (int j=0;j<trajs_[i];j++){
            initConditions_[i][j] = uni.RetVal();
        }
        ++show_progress;
    }
}
*/

void icgenerator::singleMCFill(vTraj ics){
    if (j_ < tnumb_){
        gens_->RetVal(ics);
        ++j_;
    } else {
        throw std::range_error("Reached the end of the trajectories");
    }

}

void icgenerator::montecarloFill(){

    //SingleRandIC uni(means_,variance_);
    vTraj rands = vTraj(new std::vector<double>(means_));
    LOG(INFO) << "Building " << tnumb_ << " monte-carlo ICs";
    //TODO: get progress display to go to LOG(INFO)?
    boost::progress_display show_progress(tnumb_, std::clog);

    for (int j=0;j<tnumb_;++j){
        gens_->RetVal(rands);
        for (int i=0;i<tdim_;++i){
            initConditions_[i][j] = (*rands)[i];
        }
        ++show_progress;
    }

}

/*
void icgenerator::linearFill(){

    SingleLinIC uni(means_,variance_);
    vTraj rands = vTraj(new std::vector<double>(means_));
    LOG(INFO) << "Building " << tnumb_ << " linear ICs";
    boost::progress_display show_progress(tnumb_, LOG(INFO));
    for (int j=0;j<tnumb_;j++){
            uni.RetVal(rands);
        for (int i=0;i<tdim_;i++){
            //std::cout << (*rands)[i] << std::endl;
            initConditions_[i][j] = (*rands)[i];
        }
        ++show_progress;
    }

}
*/

void icgenerator::singleCheck(){
	if (single_ == false){
        LOG(INFO) << "the icgenerator will build a full set of ICs and store them for later use";
        (*this).tTypeSwitch();
    } else {
    	LOG(INFO) << "The icgenerator is generating ICs on the fly and will not store them";
    	(*this).tTypeSwitch();
    }
}

void icgenerator::tTypeSwitch(){
    switch (tType_){
	            case 1:
	                gens_ = boost::shared_ptr<SingleRandIC>(new SingleRandIC(means_,variance_));
	                hpick = boost::bind(&icgenerator::singleMCRet,this,_1);
	                gpick = boost::bind(&icgenerator::singleMCFill,this,_1);
	                break;
	            case 2:
	                gens_ = boost::shared_ptr<SingleLinIC>(new SingleLinIC(means_,variance_,trajs_));
	                hpick = boost::bind(&icgenerator::singleMCRet,this,_1);
	                gpick = boost::bind(&icgenerator::singleMCFill,this,_1);
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
