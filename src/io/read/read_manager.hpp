//
//  read_manager.hpp
//  octopus
//
//  Created by Daniel Cooke on 14/02/2015.
//  Copyright (c) 2015 Oxford University. All rights reserved.
//

#ifndef __Octopus__read_manager__
#define __Octopus__read_manager__

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <cstddef>
#include <mutex>

#include <boost/filesystem.hpp>

#include <basics/contig_region.hpp>
#include <basics/genomic_region.hpp>
#include <containers/mappable_map.hpp>
#include <utils/hash_functions.hpp>

#include "read_reader.hpp"
#include "read_reader_impl.hpp"

namespace octopus {

class AlignedRead;

class ReadManager
{
public:
    using Path = boost::filesystem::path;
    
    using SampleName    = io::IReadReaderImpl::SampleName;
    using ReadContainer = io::IReadReaderImpl::ReadContainer;
    using SampleReadMap = io::IReadReaderImpl::SampleReadMap;
    
    ReadManager() = default;
    
    ReadManager(std::vector<Path> read_file_paths, unsigned max_open_files);
    
    ReadManager(std::initializer_list<Path> read_file_paths);
    
    ReadManager(const ReadManager&)            = delete;
    ReadManager& operator=(const ReadManager&) = delete;
    ReadManager(ReadManager&&);
    ReadManager& operator=(ReadManager&&)      = default;
    
    ~ReadManager() = default;
    
    friend void swap(ReadManager& lhs, ReadManager& rhs) noexcept;
    
    bool good() const noexcept;
    unsigned num_files() const noexcept;
    
    unsigned num_samples() const noexcept;
    const std::vector<SampleName>& samples() const;
    
    bool has_reads(const SampleName& sample,
                   const GenomicRegion& region) const;
    bool has_reads(const std::vector<SampleName>& samples,
                   const GenomicRegion& region) const;
    bool has_reads(const GenomicRegion& region) const;
    
    std::size_t count_reads(const SampleName& sample,
                            const GenomicRegion& region) const;
    std::size_t count_reads(const std::vector<SampleName>& samples,
                            const GenomicRegion& region) const;
    std::size_t count_reads(const GenomicRegion& region) const;
    
    GenomicRegion find_covered_subregion(const SampleName& sample,
                                         const GenomicRegion& region,
                                         std::size_t max_reads) const;
    GenomicRegion find_covered_subregion(const std::vector<SampleName>& samples,
                                         const GenomicRegion& region,
                                         std::size_t max_reads) const;
    GenomicRegion find_covered_subregion(const GenomicRegion& region,
                                         std::size_t max_reads) const;
    
    ReadContainer fetch_reads(const SampleName& sample,
                              const GenomicRegion& region) const;
    SampleReadMap fetch_reads(const std::vector<SampleName>& samples,
                              const GenomicRegion& region) const;
    SampleReadMap fetch_reads(const GenomicRegion& region) const;
    
private:
    using PathHash = octopus::utils::FilepathHash;
    
    struct FileSizeCompare
    {
        bool operator()(const Path& lhs, const Path& rhs) const;
    };
    
    using OpenReaderMap           = std::map<Path, io::ReadReader, FileSizeCompare>;
    using ClosedReaders           = std::unordered_set<Path, PathHash>;
    using SampleIdToReaderPathMap = std::unordered_map<SampleName, std::vector<Path>>;
    using ContigMap               = MappableMap<GenomicRegion::ContigName, ContigRegion>;
    using ReaderRegionsMap        = std::unordered_map<Path, ContigMap, PathHash>;
    
    unsigned max_open_files_ = 200;
    unsigned num_files_;
    
    mutable OpenReaderMap open_readers_;
    mutable ClosedReaders closed_readers_;
    
    SampleIdToReaderPathMap reader_paths_containing_sample_;
    
    ReaderRegionsMap possible_regions_in_readers_;
    
    std::vector<SampleName> samples_;
    
    mutable std::mutex mutex_;
    
    void setup_reader_samples_and_regions();
    void open_initial_files();
    
    io::ReadReader make_reader(const Path& reader_path) const;
    bool is_open(const Path& reader_path) const noexcept;
    std::vector<Path>::iterator partition_open(std::vector<Path>& reader_paths) const;
    unsigned num_open_readers() const noexcept;
    unsigned num_reader_spaces() const noexcept;
    void open_reader(const Path& reader_path) const;
    std::vector<Path>::iterator open_readers(std::vector<Path>::iterator first,
                                             std::vector<Path>::iterator last) const;
    void close_reader(const Path& reader_path) const;
    Path choose_reader_to_close() const;
    void close_readers(unsigned n) const;
    
    void add_possible_regions_to_reader_map(const Path& reader_path,
                                            const std::vector<GenomicRegion>& regions);
    void add_reader_to_sample_map(const Path& reader_path,
                                  const std::vector<SampleName>& samples_in_reader);
    bool could_reader_contain_region(const Path& reader_path,
                                     const GenomicRegion& region) const;
    
    std::vector<Path> get_reader_paths_containing_samples(const std::vector<SampleName>& sample) const;
    std::vector<Path> get_reader_paths_possibly_containing_region(const GenomicRegion& region) const;
    std::vector<Path> get_possible_reader_paths(const std::vector<SampleName>& samples,
                                                const GenomicRegion& region) const;
};

} // namespace octopus

#endif /* defined(__Octopus__read_manager__) */