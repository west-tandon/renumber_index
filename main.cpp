#include "reorder.hpp"
#include "util.hpp"

#include <cilk/cilk_api.h>

#include <fstream>

int main(int argc, char** argv)
{
    if (argc < 4) {
        fprintf(stderr,
            "%s <ds2i_prefix> <ds2i_out_prefix> <id_mapping> [<num threads>]\n",
            argv[0]);
        return EXIT_FAILURE;
    }
    std::string ds2i_prefix = argv[1];
    std::string ds2i_out_prefix = argv[2];
    std::string mapping = argv[3];
    if (argc == 5) {
        int threads = atoi(argv[4]);
        __cilkrts_set_param("nworkers", std::to_string(threads).c_str());
    }

    std::vector<uint32_t> ordering;
    std::ifstream in(mapping);
    std::string id;
    while (in >> id) { ordering.push_back(std::stoi(id)); }

    auto invidx = read_ds2i_files(ds2i_prefix);
    auto reordered_invidx = reorder(invidx, ordering);

    {
        timer t("write ds2i files");
        write_ds2i_files(reordered_invidx, ds2i_out_prefix);
    }

    return EXIT_SUCCESS;
}
