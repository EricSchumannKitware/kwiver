/*ckwg +29
 * Copyright 2018 by Kitware, Inc.
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

#include "burnout_image_enhancer.h"

#include <string>
#include <sstream>
#include <exception>

#include <arrows/vxl/image_container.h>

#include <vital/exceptions.h>

#include <video_transforms/video_enhancement_process.h>


namespace kwiver {
namespace arrows {
namespace burnout {


// ==================================================================================
class burnout_image_enhancer::priv
{
public:
  priv()
    : m_config_file( "burnout_enhancer.conf" )
    , m_process( "filter" )
  {}

  ~priv()
  {}

  // Items from the config
  std::string m_config_file;

  vidtk::video_enhancement_process< vxl_byte > m_process;
  vital::logger_handle_t m_logger;
};


// ==================================================================================
burnout_image_enhancer
::burnout_image_enhancer()
  : d( new priv() )
{
}


burnout_image_enhancer
::~burnout_image_enhancer()
{}


// ----------------------------------------------------------------------------------
vital::config_block_sptr
burnout_image_enhancer
::get_configuration() const
{
  // Get base config from base class
  vital::config_block_sptr config = vital::algorithm::get_configuration();

  config->set_value( "config_file", d->m_config_file,  "Name of config file." );

  return config;
}


// ----------------------------------------------------------------------------------
void
burnout_image_enhancer
::set_configuration( vital::config_block_sptr config_in )
{
  vital::config_block_sptr config = this->get_configuration();
  config->merge_config( config_in );

  d->m_config_file = config->get_value< std::string >( "config_file" );

  vidtk::config_block vidtk_config = d->m_process.params();
  vidtk_config.parse( d->m_config_file );

  if( !d->m_process.set_params( vidtk_config ) )
  {
    std::string reason = "Failed to set pipeline parameters";
    throw vital::algorithm_configuration_exception( type_name(), impl_name(), reason );
  }

  if( !d->m_process.initialize() )
  {
    std::string reason = "Failed to initialize pipeline";
    throw vital::algorithm_configuration_exception( type_name(), impl_name(), reason );
  }
}


// ----------------------------------------------------------------------------------
bool
burnout_image_enhancer
::check_configuration( vital::config_block_sptr config ) const
{
  std::string config_fn = config->get_value< std::string >( "config_file" );

  if( config_fn.empty() )
  {
    return false;
  }

  return true;
}


// ----------------------------------------------------------------------------------
vital::image_container_sptr
burnout_image_enhancer
::filter( vital::image_container_sptr image_data )
{
  // Convert inputs to burnout style inputs
  vil_image_view< vxl_byte > input_image;

  if( image_data )
  {
    input_image = vxl::image_container::vital_to_vxl( image_data->get_image() );
  }
  else
  {
    LOG_WARN( logger(), "Empty image received" );
    return vital::image_container_sptr();
  }

  // Process imagery
  d->m_process.set_source_image( input_image );

  if( !d->m_process.step() )
  {
    throw std::runtime_error( "Unable to step burnout filter process" );
  }

  // Return output in KWIVER wrapper
  return kwiver::vital::image_container_sptr(
    new arrows::vxl::image_container( d->m_process.copied_output_image() ) );
}


} } } // end namespace