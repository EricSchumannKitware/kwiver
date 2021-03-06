/*ckwg +29
 * Copyright 2015-2016, 2019 by Kitware, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
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
 * \brief VXL fundamental matrix estimation algorithm (5 point alg)
 */

#ifndef KWIVER_ARROWS_VXL_ESTIMATE_FUNDAMENTAL_MATRIX_H_
#define KWIVER_ARROWS_VXL_ESTIMATE_FUNDAMENTAL_MATRIX_H_

#include <arrows/vxl/kwiver_algo_vxl_export.h>

#include <vital/types/camera_intrinsics.h>

#include <vital/algo/estimate_fundamental_matrix.h>

namespace kwiver {
namespace arrows {
namespace vxl {

/// A class that uses 5 pt algorithm to estimate an initial xform between 2 pt sets
class KWIVER_ALGO_VXL_EXPORT estimate_fundamental_matrix
  : public vital::algorithm_impl<estimate_fundamental_matrix, vital::algo::estimate_fundamental_matrix>
{
public:
  PLUGIN_INFO( "vxl",
               "Use VXL (vpgl) to estimate a fundamental matrix." )

  /// Constructor
  estimate_fundamental_matrix();

  /// Destructor
  virtual ~estimate_fundamental_matrix();

  /// Get this algorithm's \link vital::config_block configuration block \endlink
  virtual vital::config_block_sptr get_configuration() const;
  /// Set this algorithm's properties via a config block
  virtual void set_configuration(vital::config_block_sptr config);
  /// Check that the algorithm's currently configuration is valid
  virtual bool check_configuration(vital::config_block_sptr config) const;

  /// Estimate an fundamental matrix from corresponding points
  /**
   * \param [in]  pts1 the vector or corresponding points from the first image
   * \param [in]  pts2 the vector of corresponding points from the second image
   * \param [out] inliers for each point pair, the value is true if
   *                      this pair is an inlier to the estimate
   * \param [in]  inlier_scale error distance tolerated for matches to be inliers
   */
  virtual
  vital::fundamental_matrix_sptr
  estimate(const std::vector<vital::vector_2d>& pts1,
           const std::vector<vital::vector_2d>& pts2,
           std::vector<bool>& inliers,
           double inlier_scale = 1.0) const;
  using vital::algo::estimate_fundamental_matrix::estimate;

  /// Test corresponding points against a fundamental matrix and mark inliers
  /**
   * \param [in]  fm   the fundamental matrix
   * \param [in]  pts1 the vector or corresponding points from the first image
   * \param [in]  pts2 the vector of corresponding points from the second image
   * \param [out] inliers for each point pair, the value is true if
   *                      this pair is an inlier to the estimate
   * \param [in]  inlier_scale error distance tolerated for matches to be inliers
   */
  static void
  mark_inliers(vital::fundamental_matrix_sptr const& fm,
               std::vector<vital::vector_2d> const& pts1,
               std::vector<vital::vector_2d> const& pts2,
               std::vector<bool>& inliers,
               double inlier_scale = 1.0);
private:
  /// private implementation class
  class priv;
  const std::unique_ptr<priv> d_;
};


} // end namespace vxl
} // end namespace arrows
} // end namespace kwiver

#endif
