/*ckwg +29
 * Copyright 2011-2017, 2020 by Kitware, Inc.
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

#ifndef SPROKIT_PROCESSES_EXAMPLES_PRINT_NUMBER_PROCESS_H
#define SPROKIT_PROCESSES_EXAMPLES_PRINT_NUMBER_PROCESS_H

#include "processes_examples_export.h"

#include <sprokit/pipeline/process.h>

/**
 * \file print_number_process.h
 *
 * \brief Declaration of the number printer process.
 */

namespace sprokit {

/**
 * \class print_number_process
 *
 * \brief A process for printing numbers to a file.
 *
 * \process Prints numbers to a file.
 *
 * \iports
 *
 * \iport{number} The source of numbers to print.
 *
 * \configs
 *
 * \config{output} Where to output the numbers.
 *
 * \reqs
 *
 * \req The \port{number} port must be connected.
 *
 * \ingroup examples
 */
class PROCESSES_EXAMPLES_NO_EXPORT print_number_process
  : public process
{
public:
  PLUGIN_INFO( "print_number",
               "Print numbers to a file" );
  /**
   * \brief Constructor.
   *
   * \param config The configuration for the process.
   */
  print_number_process(kwiver::vital::config_block_sptr const& config);

  /**
   * \brief Destructor.
   */
  ~print_number_process();

protected:
  /**
   * \brief Configure the process.
   */
  void _configure() override;

  /**
   * \brief Reset the process.
   */
  void _reset() override;

  /**
   * \brief Step the process.
   */
  void _step() override;

private:
  class priv;
  std::unique_ptr<priv> d;
};

}

#endif // SPROKIT_PROCESSES_EXAMPLES_PRINT_NUMBER_PROCESS_H
