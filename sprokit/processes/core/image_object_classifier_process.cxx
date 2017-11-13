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

#include "image_object_classifier_process.h"
#include <arrows/ocv/image_container.h>

#include <vital/vital_foreach.h>
#include <vital/algo/image_object_detector.h>
#include <vital/util/wall_timer.h>

#include <sprokit/processes/kwiver_type_traits.h>
#include <sprokit/pipeline/process_exception.h>

namespace kwiver {

create_config_trait( detector, std::string, "", "Algorithm configuration subblock.\n\n"
  "Use 'detector:type' to select desired detector implementation.");


//----------------------------------------------------------------
// Private implementation class
class image_object_classifier_process::priv
{
public:
  priv();
  ~priv();

  kwiver::vital::wall_timer m_detect_timer;
  kwiver::vital::logger_handle_t m_logger;
  vital::algo::image_object_detector_sptr m_detector;

  // ------------------------------------------------------------------
  /**
   * @brief Classify regions defined within the provided detected object set.
   *
   * This method
   */
  void
  classify( const vital::image_container_sptr &src_image,
            const vital::detected_object_set_sptr &dets_in_sptr,
            vital::detected_object_set_sptr &dets_out_sptr )
  {
    kwiver::vital::wall_timer timer;
    cv::Mat cv_src = arrows::ocv::image_container::vital_to_ocv( src_image->get_image() );

    vital::detected_object::vector_t dets_in = dets_in_sptr->select();
    vital::detected_object::vector_t dets_out;

    // Add in the detections that defined the ROI
    //dets_out.insert( dets_out.end(), dets_in.begin(), dets_in.end() );
    
    // Define the bound box representing the entire image.
    cv::Size s = cv_src.size();
    vital::bounding_box_d img( vital::bounding_box_d::vector_type( 0, 0 ),
                               vital::bounding_box_d::vector_type( s.width, s.height ) );

    kwiver::vital::image_container_sptr windowed_image;
    VITAL_FOREACH( vital::detected_object_sptr det, dets_in )
    {
      timer.start();
      vital::bounding_box_d bbox = det->bounding_box();
      
      // Clip bounding box to the image
      bbox = intersection( img, bbox );

      if( bbox.height() <= 0 || bbox.width() <= 0 )
      {
        continue;
      }
      
      int x = bbox.upper_left()[0];
      int y = bbox.upper_left()[1];
      int w = bbox.width();
      int h = bbox.height();
      
      LOG_TRACE( m_logger, "Processing ROI window with upper left coordinates (" +
                 std::to_string(x) + "," + std::to_string(y) + ") of size (" +
                 std::to_string(w) + "x" + std::to_string(h) + ")" );

      // Make CV rect for bbox
      cv::Rect roi( x, y, w, h );
      
      // Detect within the region of interest.
      windowed_image = vital::image_container_sptr( new arrows::ocv::image_container( cv_src(roi) ) );
      timer.stop();
      LOG_TRACE( m_logger, "Time to create windowed vital image: " << timer.elapsed() );
      
      vital::detected_object::vector_t dets = m_detector->detect( windowed_image )->select();
      
      // Add detections set to the output detection set
      dets_out.insert( dets_out.end(), dets.begin(), dets.end() );
    } // end foreach

    dets_out_sptr = std::make_shared<vital::detected_object_set>(dets_out);
  }

}; // end priv class


// ==================================================================
image_object_classifier_process::
image_object_classifier_process( kwiver::vital::config_block_sptr const& config )
  : process( config ),
    d( new image_object_classifier_process::priv )
{
  // Attach our logger name to process logger
  attach_logger( kwiver::vital::get_logger( name() ) ); // could use a better approach
  d->m_logger = logger();

  make_ports();
  make_config();
}


image_object_classifier_process::
~image_object_classifier_process()
{
}


// ------------------------------------------------------------------
void
image_object_classifier_process::
_configure()
{
  vital::config_block_sptr algo_config = get_config();

  // Check config so it will give run-time diagnostic of config problems
  if ( ! vital::algo::image_object_detector::check_nested_algo_configuration( "detector", algo_config ) )
  {
    throw sprokit::invalid_configuration_exception( name(), "Configuration check failed." );
  }

  vital::algo::image_object_detector::set_nested_algo_configuration( "detector", algo_config, d->m_detector );
  if ( ! d->m_detector )
  {
    throw sprokit::invalid_configuration_exception( name(), "Unable to create detector" );
  }
}


// ------------------------------------------------------------------
void
image_object_classifier_process::
_step()
{
  LOG_TRACE( logger(), "Starting process" );
  vital::image_container_sptr src_image = grab_from_port_using_trait( image );
  vital::detected_object_set_sptr dets_in = grab_from_port_using_trait( detected_object_set );
  d->m_detect_timer.start();

  // Get detections from detector on image
  vital::detected_object_set_sptr dets_out;
  d->classify( src_image, dets_in, dets_out );

  d->m_detect_timer.stop();

  double elapsed_time = d->m_detect_timer.elapsed();
  LOG_DEBUG( logger(), "Elapsed time detecting objects: " << elapsed_time );

  push_to_port_using_trait( detection_time, elapsed_time);
  push_to_port_using_trait( detected_object_set, dets_out );
}


// ------------------------------------------------------------------
void
image_object_classifier_process::
make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t required;
  sprokit::process::port_flags_t optional;

  required.insert( flag_required );

  // -- input --
  declare_input_port_using_trait( image, required );
  declare_input_port_using_trait( detected_object_set, required );

  // -- output --
  declare_output_port_using_trait( detected_object_set, optional );
  declare_output_port_using_trait( detection_time, optional );
}


// ------------------------------------------------------------------
void
image_object_classifier_process::
make_config()
{
  declare_config_using_trait( detector );
}


// ================================================================
image_object_classifier_process::priv
::priv()
{
}


image_object_classifier_process::priv
::~priv()
{
}

} // end namespace kwiver
