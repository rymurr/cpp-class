#include "pywrapper.hpp"

namespace classical{

int run_main(int argc, std::vector<std::string> argv, std::string fname){
    google::InitGoogleLogging(argv[0].c_str());

    pmap params;
    if (!params.read_params(fname, argc, argv)){return 0;};
    params.print();
    anyMap PrMap;
    params.map_out(PrMap);
    LOG(INFO) << "Parameters have been built, now proceeding with initial trajectory distribution";

    std::vector<field> field_vec;
    int num_fields = any_cast<int>(PrMap["nfield"]);
    for (int i = 0;i < num_fields ; i++) {
        field_vec.push_back(field(PrMap,i));
    }
    std::vector<double> dist_center;
    //find_center(params.map

    google::ShutdownGoogleLogging();
    return 1;
}

int pyrun_main(int argc, boost::python::list argv, std::string fname){

    std::size_t n = boost::python::len(argv);
    std::vector<std::string> av;
    for (int i = 0; i < n; i++) {
        av.push_back(boost::python::extract<std::string>(argv[i]));
    }

    if (!run_main(argc, av, fname)){return 0;}
    return 1;
}
}
