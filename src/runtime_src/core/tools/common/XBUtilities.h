/**
 * Copyright (C) 2019-2022 Xilinx, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef __XBUtilities_h_
#define __XBUtilities_h_

// Include files
// Please keep these to the bare minimum
#include "core/common/device.h"
#include "core/common/query_requests.h"

#include <string>
#include <memory>
#include <map>
#include <iostream>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

namespace XBUtilities {
 
  bool can_proceed(bool force=false);
  void can_proceed_or_throw(const std::string& info, const std::string& error);

  void sudo_or_throw(const std::string& msg);
  void print_exception_and_throw_cancel(const xrt_core::error& e);
  void print_exception_and_throw_cancel(const std::runtime_error& e);


  void collect_devices( const std::set<std::string>  &_deviceBDFs,
                        bool _inUserDomain,
                        xrt_core::device_collection &_deviceCollection);

  boost::property_tree::ptree
  get_available_devices(bool inUserDomain);
  
   /**
   * get_axlf_section() - Get section from the file passed in
   *
   * filename: file containing the axlf section
   *
   * Return: pair of section data and size in bytes
   */
  std::vector<char>
  get_axlf_section(const std::string& filename, axlf_section_kind section);

  /**
   * get_uuids() - Get UUIDs from the axlf section
   *
   * dtbuf: axlf section to be parsed
   *
   * Return: list of UUIDs
   */
  std::vector<std::string> get_uuids(const void *dtbuf);

  xrt_core::query::reset_type str_to_reset_obj(const std::string& str);

  std::string
  get_xrt_pretty_version();

  /**
   * OEM ID is a unique number called as the 
   * Private Enterprise Number (PEN) maintained by IANA
   * 
   * Return: Manufacturer's name
   */
  std::string 
  parse_oem_id(const std::string& oemid);

  std::string 
  parse_clock_id(const std::string& id);


  std::string
  string_to_UUID(std::string str);

  uint64_t 
  string_to_bytes(std::string str);

  inline bool 
  is_power_of_2(const uint64_t x)
  {
    /*
    * Verify that the given value is greater than zero
    * and that only one bit is set in the given value
    * if only one bit is set this implies the given value is a power of 2
    */
    return (x != 0) && ((x & (x - 1)) == 0);
  }

 /*
  * xclbin locking
  */
  struct xclbin_lock
  {
    xclDeviceHandle m_handle;
    xuid_t m_uuid;

    xclbin_lock(std::shared_ptr<xrt_core::device> _dev)
      : m_handle(_dev->get_device_handle())
    {
      auto xclbinid = xrt_core::device_query<xrt_core::query::xclbin_uuid>(_dev);

      uuid_parse(xclbinid.c_str(), m_uuid);

      if (uuid_is_null(m_uuid))
        throw std::runtime_error("'uuid' invalid, please re-program xclbin.");

      if (xclOpenContext(m_handle, m_uuid, std::numeric_limits<unsigned int>::max(), true))
        throw std::runtime_error("'Failed to lock down xclbin");
    }

    ~xclbin_lock(){
      xclCloseContext(m_handle, m_uuid, std::numeric_limits<unsigned int>::max());
    }
  };

};

#endif

