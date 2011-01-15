
#include "icgen.hpp"

using boost::any_cast;     

icgenerator::icgenerator(){}

icgenerator::~icgenerator(){}

icgenerator::icgenerator(anyMap params){

    means_ = any_cast<std::vector<double> >(params["means"]);
    trajs_ = any_cast<std::vector<int> >(params["dims"]);
    tType_ = any_cast<int>(params["dist-type"]);
    variance_= any_cast<double>(params["variance"]);
    tnumb_ = 1;
    for_each(trajs_.begin(), trajs_.end(), tnumb_*=boost::lambda::_1);

    try{
        single_ = any_cast<bool>(params["nobuild"]);
    } catch (boost::bad_any_cast &e) {
        single_ = false;
    }

    if (means_.size() != trajs_.size()){
        throw  std::range_error("the means and dims arrays are not the same size!");
    }

    if (single_ == false){
        LOG(INFO) << "the icgenerator will build a full set of ICs and store them for later use";
        switch (tType_){
            case 1:
                fpick = boost::bind(&icgenerator::montecarlo,this);
                break;
            case 2:
                fpick = boost::bind(&icgenerator::linear,this);
                break;
            case 3:
                LOG(FATAL) << "staged-linear is currently not implemented";
            default:
                LOG(FATAL) << "Bad choice for Initial conditions";
        }
    } else {
        LOG(INFO) << "The icgenerator is generating ICs on the fly and will not store them";
        switch (tType_){
            case 1:
                gens_.push_back(boost::shared_ptr<SingleRandIC>(new SingleRandIC(means_,variance_)));
                gpick = boost::bind(&icgenerator::singlemc,this,_1);
                break;
            case 2:
                for(int i=0;i<trajs_.size();i++){
                    gens_.push_back(boost::shared_ptr<SingleLinIC>(new SingleLinIC(means_[i],variance_,trajs_[i])));
                    tsing_.push_back(0);
                }
                lindone_ = false;
                gpick = boost::bind(&icgenerator::singlelin,this,_1);
                break;
             case 3:
                LOG(FATAL) << "staged-linear is currently not implemented";
            default:
                LOG(FATAL) << "Bad choice for Initial conditions";
        }
    }
}

void icgenerator::get_ic(vTraj ics){
    (*this).gpick(ics);
}

void icgenerator::gen_ics(){
    (*this).fpick();
}

void icgenerator::ret_ics(boost::shared_ptr<ic_array> icPtr){
    *icPtr = initConditions_;
}

void icgenerator::singlemc(vTraj ics){
    if (tnumb_ > 0){
        gens_[0]->RetVal(ics); 
        tnumb_--;
    } else {
        throw std::range_error("Reached the end of the trajectories"); 
    }

}

void icgenerator::singlelin(vTraj ics){
    
    if (!lindone_){
//        transform(gens_.begin(),gens_.end(),ics->begin(),boost::bind<double>(&SingleLinIC::RetVal,boost::lambda::ll_reinterpret_cast<SingleLinIC*>(boost::lambda::_1)));
//        for_each(gens_.begin(),gens_.end(),boost::bind(&SingleLinIC::RetVal,boost::lambda::ll_dynamic_cast<boost::shared_ptr<SingleLinIC> >(boost::lambda::_1)));
        for (int i=0;i<gens_.size();i++){
            (*ics)[i] = gens_[i]->RetVal();
            tsing_[i]+=1;
        }
    //    tsing_[0]++;
        lindone_=true;
        return;
        //TRYING TO GET THIS TO WORK AS A ONE LINER!
    }

    if (tsing_[0] >= trajs_[0]){
        tsing_[0]=0;
        gens_[0]->reset();
        (*ics)[1] = gens_[1]->RetVal();
        tsing_[1]++;
    }

    for (int i=1;i<tsing_.size()-1;i++){
        if (tsing_[i] > trajs_[i]){
            tsing_[i]=1;
            gens_[i]->reset();
            (*ics)[i] = gens_[i]->RetVal();
            (*ics)[i+1] = gens_[i+1]->RetVal();
            tsing_[i+1]++;
//            return;
        }// else 

    }

    (*ics)[0] = gens_[0]->RetVal();
    tsing_[0]++;
//VERY IMPORTANT!!! IS ALL OF THIS THREAD SAFE???? Shouldnt need to be, see notes.

    if (tsing_[tsing_.size()-1] > trajs_[trajs_.size()-1])
        throw std::range_error("Reached the end of the trajectories"); 
}

void icgenerator::linear(){
    int max = *(std::max_element(trajs_.begin(),trajs_.end()));
    initConditions_.resize(boost::extents[trajs_.size()][max]);    
    
    for (int i=0;i<trajs_.size();i++){
            SingleLinIC uni(means_[i],variance_,trajs_[i]);
        for (int j=0;j<trajs_[i];j++){
            initConditions_[i][j] = uni.RetVal();
        }
    }
}

void icgenerator::montecarlo(){

    int tot = 1;
    std::for_each(trajs_.begin(),trajs_.end(),tot*=boost::lambda::_1);
    initConditions_.resize(boost::extents[trajs_.size()][tot]);

    /*
    base_generator_type generator(static_cast<unsigned int>(std::time(0)));
    boost::normal_distribution<> uni_dist(0,variance_);
    boost::variate_generator<base_generator_type&, boost::normal_distribution<> > uni(generator, uni_dist);
    */
    //double trajs = std::accumulate(trajs_.begin(),trajs_.end(),0);
    SingleRandIC uni(means_,variance_);
    vTraj rands = vTraj(new std::vector<double>(means_));
    for (int j=0;j<tot;j++){
            uni.RetVal(rands);
        for (int i=0;i<trajs_.size();i++){
            initConditions_[i][j] = (*rands)[i];
        }
    }

}

SingleRandIC::SingleRandIC(){}

SingleRandIC::~SingleRandIC(){}

SingleRandIC::SingleRandIC(std::vector<double> means, double var): means_(means), SingleIC(var,1){

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

SingleLinIC::SingleLinIC(){}

SingleLinIC::~SingleLinIC(){}

SingleLinIC::SingleLinIC(double mean, double var, int size): mean_(mean), SingleIC(var,size){

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

SingleIC::SingleIC(){}

SingleIC::~SingleIC(){}

SingleIC::SingleIC(double var, int size): size_(size), var_(var){}
