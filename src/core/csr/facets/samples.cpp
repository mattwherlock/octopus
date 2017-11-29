// Copyright (c) 2017 Daniel Cooke
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#include "samples.hpp"

#include <utility>

namespace octopus { namespace csr {

const std::string Samples::name_ {"Samples"};

Samples::Samples(std::vector<std::string> samples) : samples_ {std::move(samples)} {}

Facet::ResultType Samples::do_get() const
{
    return samples_;
}

} // namespace csr
} // namespace octopus