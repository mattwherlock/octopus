//
//  variant_call.hpp
//  Octopus
//
//  Created by Daniel Cooke on 21/04/2016.
//  Copyright © 2016 Oxford University. All rights reserved.
//

#ifndef call_hpp
#define call_hpp

#include <vector>
#include <unordered_map>
#include <utility>
#include <iterator>

#include <boost/optional.hpp>

#include "common.hpp"
#include "genomic_region.hpp"
#include "mappable.hpp"
#include "allele.hpp"
#include "genotype.hpp"
#include "vcf_record.hpp"
#include "reference_genome.hpp"

namespace Octopus
{
    class Call : public Mappable<Allele>
    {
    public:
        Call() = delete;
        
        explicit Call(double quality);
        template <typename T> explicit Call(T&& genotype_calls, double quality);
        
        virtual ~Call() = default;
        
        Call(const Call&)            = default;
        Call& operator=(const Call&) = default;
        Call(Call&&)                 = default;
        Call& operator=(Call&&)      = default;
        
        struct PhaseCall
        {
            PhaseCall() = delete;
            template <typename R> PhaseCall(R&& region, double score);
            
            GenomicRegion region;
            double score;
        };
        
        struct GenotypeCall
        {
            GenotypeCall() = delete;
            template <typename G> GenotypeCall(G&& genotype, double posterior);
            template <typename G, typename P> GenotypeCall(G&& genotype, double posterior, P&& phase);
            
            Genotype<Allele> genotype;
            double posterior;
            boost::optional<PhaseCall> phase;
        };
        
        double get_quality() const noexcept;
        
        const GenotypeCall& get_genotype_call(const SampleIdType& sample) const;
        
        bool is_phased(const SampleIdType& sample) const;
        bool all_phased() const noexcept;
        void set_phase(const SampleIdType& sample, PhaseCall phase);
        
        virtual const GenomicRegion& get_region() const noexcept = 0;
        virtual const Allele& get_reference() const noexcept = 0;
        
        virtual void parsimonise(char dummy_base) {};
        virtual void parsimonise(const ReferenceGenome& reference) {};
        virtual void decorate(VcfRecord::Builder& record) const = 0;
        
    protected:
        std::unordered_map<SampleIdType, GenotypeCall> genotype_calls_;
        
        double quality_;
    };
    
    template <typename T>
    Call::Call(T&& genotype_calls, double quality)
    : genotype_calls_ {std::begin(genotype_calls), std::end(genotype_calls)}, quality_ {quality}
    {}
    
    template <typename R>
    Call::PhaseCall::PhaseCall(R&& region, double score)
    : region {std::forward<R>(region)}, score {score}
    {}
    
    template <typename G>
    Call::GenotypeCall::GenotypeCall(G&& genotype, double posterior)
    : genotype {std::forward<G>(genotype)}, posterior {posterior}, phase {}
    {}
    
    template <typename G, typename P>
    Call::GenotypeCall::GenotypeCall(G&& genotype, double posterior, P&& phase)
    : genotype {std::forward<G>(genotype)}, posterior {posterior}, phase {std::forward<P>(phase)}
    {}
} // namespace Octopus

#endif /* call_hpp */