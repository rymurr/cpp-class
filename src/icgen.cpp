
#include "icgen.hpp"

namespace classical {

using boost::any_cast;

icgenerator::icgenerator(anyMap &params, anyMap wParams){

    means_ = any_cast<std::vector<double> >(params["means"]);
    trajs_ = any_cast<std::vector<int> >(params["dims"]);
    tType_ = any_cast<int>(params["dist-type"]);
    variance_= any_cast<double>(params["variance"]);
    tnumb_ = std::accumulate(trajs_.begin(),trajs_.end(),1,std::multiplies<int>());
    tdim_ = trajs_.size();
    j_=0;
    k_=0;


    try{
        single_ = any_cast<bool>(params["nobuild"]);
    } catch (boost::bad_any_cast &e) {
        single_ = false;
    }

    if (means_.size() != trajs_.size()){
        throw  icgen_input_error() << err_info("the means and dims arrays are not the same size!") << dims_info(std::make_pair(means_.size(),trajs_.size()));
    }

    this->singleCheck();
    if (wParams.empty()){
        LOG(WARNING) << "The weight generator is using the general parameter set to build. This could possibly slow down the program and puts i"
                << " stronger set of restrictions on what can be in the parameter set. This is fine, but consider using a specialized weight generation parameter set";
        this->setWeights(params);
    } else {
        LOG(WARNING) << "The weight generator is using the custom parameter set supplied by the caller.";
        this->setWeights(wParams);
    }
    if (!single_){
    	this->genICs();
    }

}

/*
void icgenerator::genWeights(){
    ;
}
*/

void icgenerator::genICs(){

    vTraj rands = vTraj(new std::vector<double>(means_));
    LOG(INFO) << "Building " << tnumb_ << " ICs";
    //TODO: get progress display to go to LOG(INFO)? also, wrap in a #ifdef or something
    boost::progress_display show_progress(tnumb_, std::clog);

    //TODO: this can probably be openmp'ed or another thread type?
    for (int j=0;j<tnumb_;++j){
        icGens_->operator()(rands);
        weights_[j] = wGens_->operator()(rands);
        std::transform(rands->begin(),rands->end(),initConditions_[j].begin(),boost::lambda::_1);
        ++show_progress;
    }

}

void icgenerator::singleCheck(){
	if (!single_){
        DLOG(INFO) << "Dims are: " << tdim_ << " by " << tnumb_;
	    initConditions_.resize(boost::extents[tnumb_][tdim_]);
	    weights_.resize(tnumb_);
        LOG(INFO) << "the icgenerator will build a full set of ICs and store them for later use";
        LOG(INFO) << "the icgenerator will build a full set of weights and store them for later use";
    } else {
    	LOG(INFO) << "The icgenerator is generating ICs on the fly and will not store them";
    	LOG(INFO) << "The icgenerator is generating weights on the fly and will not store them";
    }
	switch (tType_){
        case 1:
            LOG(INFO) << "icgenerator is building ICs in monte-carlo mode. With " << tdim_ << " dimensions and " << tnumb_ << " trajectories.";
            icGens_ = boost::shared_ptr<SingleRandIC>(new SingleRandIC(means_,variance_));
            break;
        case 2:
            LOG(INFO) << "icgenerator is building ICs in linear mode. With " << tdim_ << " dimensions and " << tnumb_ << " trajectories.";
            icGens_ = boost::shared_ptr<SingleLinIC>(new SingleLinIC(means_,variance_,trajs_));
            break;
        case 3:
            LOG(FATAL) << "staged-linear is currently not implemented";
            break;
        default:
            LOG(FATAL) << "Bad choice for Initial conditions";
	}
}

void icgenerator::save(std::string sType, std::string fName){

    std::string binary="binary", text="text";
    std::ofstream ofs(fName.c_str());

    LOG(INFO) << "Saving icgenerator in " + sType + " format. Archive is called " + fName;
    if (sType.compare(binary) == 0){
        boost::archive::binary_oarchive oa(ofs);
        oa << *this;
    } else if (sType.compare(text) == 0) {
        boost::archive::text_oarchive oa(ofs);
        oa << *this;
    } else {
        std::string throwArg="archive type must be binary or text not" + sType;
        throw archive_error() << err_info(throwArg);
    }

}

void icgenerator::load(std::string sType, std::string fName, anyMap params){

    std::string binary="binary", text="text";
    std::ifstream ifs(fName.c_str());

    LOG(INFO) << "Loading icgenerator from " + sType + " format. Archive is called " + fName;
    try{
        if (sType.compare(binary) == 0){
            boost::archive::binary_iarchive ia(ifs);
            ia >> *this;
        } else if (sType.compare(text) == 0) {
            boost::archive::text_iarchive ia(ifs);
            ia >> *this;
        } else {
            std::string throwArg="archive type must be binary or text not" + sType;
            throw archive_error() << err_info(throwArg);
        }
    } catch (std::exception &e){
        LOG(ERROR) << "Binary is of wrong type, " << e.what();
        throw;
    }

    if (!params.empty()){
        this->setWeights(params);
        LOG(INFO) << "weights are being rebuilt from parameter set";
    } else {
        if (single_){
            LOG(WARNING) << "weights are not able to be fully rebuilt, you must call setWeights separately or the program will halt.";
        } else {
            LOG(WARNING) << "weights are not being fully rebuilt, since they were stored in the archive this should be ok.";
        }
    }
}

}
