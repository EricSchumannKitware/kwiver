/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Implementation of compute_association_matrix_from_features
 */

#include "compute_association_matrix_from_features.h"

#include <vital/algo/detected_object_filter.h>
#include <vital/types/object_track_set.h>
#include <vital/exceptions/algorithm.h>

#include <string>
#include <vector>
#include <atomic>
#include <algorithm>


namespace kwiver {
namespace arrows {
namespace core {

using namespace kwiver::vital;


/// Private implementation class
class compute_association_matrix_from_features::priv
{
public:
  /// Constructor
  priv()
    : m_logger( vital::get_logger( "arrows.core.compute_association_matrix_from_features" ))
  {
  }

  /// The feature matching algorithm to use
  vital::algo::detected_object_filter_sptr filter;

  /// Logger handle
  vital::logger_handle_t m_logger;
};


/// Constructor
compute_association_matrix_from_features
::compute_association_matrix_from_features()
  : d_( new priv )
{
}


/// Destructor
compute_association_matrix_from_features
::~compute_association_matrix_from_features() VITAL_NOTHROW
{
}


std::string
compute_association_matrix_from_features
::description() const
{
  return "Initializes new object tracks via simple thresholding";
}


/// Get this alg's \link vital::config_block configuration block \endlink
vital::config_block_sptr
compute_association_matrix_from_features
::get_configuration() const
{
  // get base config from base class
  vital::config_block_sptr config = algorithm::get_configuration();

  // Sub-algorithm implementation name + sub_config block
  // - Feature filter algorithm
  algo::detected_object_filter::get_nested_algo_configuration(
    "filter", config, d_->filter);

  return config;
}


/// Set this algo's properties via a config block
void
compute_association_matrix_from_features
::set_configuration( vital::config_block_sptr in_config )
{
  vital::config_block_sptr config = this->get_configuration();
  config->merge_config( in_config );

  algo::detected_object_filter::set_nested_algo_configuration( "filter",
    config, d_->filter );
}


bool
compute_association_matrix_from_features
::check_configuration(vital::config_block_sptr config) const
{
  return (
    algo::detected_object_filter::check_nested_algo_configuration( "filter", config )
  );
}


/// Compute an association matrix given detections and tracks
bool
compute_association_matrix_from_features
::compute(kwiver::vital::timestamp ts,
          kwiver::vital::image_container_sptr image,
          kwiver::vital::object_track_set_sptr tracks,
          kwiver::vital::detected_object_set_sptr detections,
          kwiver::vital::matrix_d& matrix,
          kwiver::vital::detected_object_set_sptr& considered) const
{
  considered = d_->filter->filter( detections );

  auto filtered_dets = considered->select();
  auto filtered_tracks = tracks->tracks();

  if( filtered_tracks.empty() || filtered_dets.empty() )
  {
    matrix = kwiver::vital::matrix_d();
  }
  else
  {
    matrix = kwiver::vital::matrix_d( filtered_tracks.size(), filtered_dets.size() );

    for( unsigned t = 0; t < filtered_tracks.size(); ++t )
    {
      for( unsigned d = 0; d < filtered_dets.size(); ++d )
      {
        matrix( t, d ) = 0.5;
      }
    }
  }

  considered = detections;
  return ( matrix.size() > 0 );
}


} // end namespace core
} // end namespace arrows
} // end namespace kwiver
