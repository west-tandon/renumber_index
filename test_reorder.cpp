#include "reorder.hpp"
#include "util.hpp"

#include <cilk/cilk_api.h>

#include <fstream>

int main(int argc, char** argv)
{
    if (argc < 4) {
        fprintf(stderr,
            "%s <original> <reordered> <id_mapping>\n",
            argv[0]);
        return EXIT_FAILURE;
    }
    std::string original_prefix = argv[1];
    std::string reordered_prefix = argv[2];
    std::string mapping = argv[3];

    std::vector<uint32_t> ordering;
    std::ifstream in(mapping);
    std::string id;
    while (in >> id) { ordering.push_back(std::stoi(id)); }

    auto original = read_ds2i_files(original_prefix);
    auto reordered = read_ds2i_files(reordered_prefix);

    assert(original.num_docs == reordered.num_docs);
    assert(original.freqs.size() == reordered.freqs.size());
    assert(original.docids.size() == reordered.docids.size());
    assert(original.freqs.size() == reordered.freqs.size());
    assert(original.doc_lengths.size() == reordered.doc_lengths.size());

    for (size_t idx = 0; idx < original.doc_lengths.size(); idx++) {
        if (original.doc_lengths[idx] != reordered.doc_lengths[ordering[idx]])
        {
            std::ostringstream msg;
            msg << "doc_lengths at " << idx << ": "
                << original.doc_lengths[idx] << " != "
                << reordered.doc_lengths[ordering[idx]]
                << std::endl;
            throw std::runtime_error(msg.str());
        }
    }

    for (uint32_t term_id = 0; term_id < original.docids.size(); term_id++) {
        std::unordered_map<uint32_t, uint32_t> reordered_postings;
        for (uint32_t idx = 0; idx < reordered.docids[term_id].size(); idx++)
        {
            auto doc = reordered.docids[term_id][idx];
            auto freq = reordered.freqs[term_id][idx];
            reordered_postings[doc] = freq;
        }
        for (uint32_t idx = 0; idx < original.docids[term_id].size(); idx++)
        {
            auto doc = original.docids[term_id][idx];
            auto mapped_doc = ordering[doc];
            if (reordered_postings[mapped_doc] != original.freqs[term_id][idx])
            {
                std::ostringstream msg;
                msg << "freq for term " << term_id << " and doc "
                    << doc << " (mapped " << mapped_doc << "): "
                    << reordered_postings[mapped_doc] << " != "
                    << original.freqs[term_id][idx]
                    << std::endl;
                throw std::runtime_error(msg.str());
            }
        }
    }

    return EXIT_SUCCESS;
}
