//
//  pedigree_caller.cpp
//  Octopus
//
//  Created by Daniel Cooke on 30/10/2015.
//  Copyright © 2015 Oxford University. All rights reserved.
//

#include "pedigree_caller.hpp"

#include <utility>

#include "read_pipe.hpp"
#include "vcf_record.hpp"

namespace Octopus
{
    // public methods
    
    PedigreeVariantCaller::PedigreeVariantCaller(const ReferenceGenome& reference,
                                                 ReadPipe& read_pipe,
                                                 CandidateVariantGenerator&& candidate_generator,
                                                 unsigned ploidy,
                                                 SampleIdType mother, SampleIdType father,
                                                 double min_variant_posterior)
    :
    VariantCaller {reference, read_pipe, std::move(candidate_generator), RefCallType::None},
    ploidy_ {ploidy},
    mother_ {std::move(mother)},
    father_ {std::move(father)},
    min_variant_posterior_ {min_variant_posterior}
    {}
    
    // private methods
    
    std::string PedigreeVariantCaller::do_get_details() const
    {
        return "Pedigree caller. mother = " + mother_ + ", father = " + father_;
    }
    
    namespace {
        using GM = GenotypeModel::Pedigree;
        
    } // namespace
    
    std::vector<VcfRecord>
    PedigreeVariantCaller::call_variants(const GenomicRegion& region,
                                     const std::vector<Variant>& candidates,
                                     const ReadMap& reads) const
    {
        std::vector<VcfRecord> result {};
        return result;
    }

} // namespace Octopus