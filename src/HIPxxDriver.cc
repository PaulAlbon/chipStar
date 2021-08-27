/**
 * @file HIPxxDriver.cc
 * @author Paulius Velesko (pvelesko@gmail.com)
 * @brief Definitions of extern declared functions and objects in HIPxxDriver.hh
 * Initializing the HIPxx runtime with backend selection through HIPXX_BE
 * environment variable.
 * @version 0.1
 * @date 2021-08-19
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "HIPxxDriver.hh"

#include <string>

#include "backend/backends.hh"

std::once_flag initialized;
HIPxxBackend* Backend;

std::string read_env_var(std::string ENV_VAR) {
  const char* ENV_VAR_IN = std::getenv(ENV_VAR.c_str());
  if (ENV_VAR_IN == nullptr) {
    return std::string();
  }

  return std::string(ENV_VAR_IN);
};

std::string read_backend_selection();

void read_env_vars(std::string& HIPxxPlatformStr,
                   std::string& HIPxxDeviceTypeStr,
                   std::string& HIPxxDeviceStr) {
  HIPxxPlatformStr = read_env_var("HIPXX_PLATFORM");
  if (HIPxxPlatformStr.size() == 0) HIPxxPlatformStr = "0";

  HIPxxDeviceTypeStr = read_env_var("HIPXX_DEVICE_TYPE");
  if (HIPxxDeviceTypeStr.size() == 0) HIPxxDeviceTypeStr = "default";

  HIPxxDeviceStr = read_env_var("HIPXX_DEVICE");
  if (HIPxxDeviceStr.size() == 0) HIPxxDeviceStr = "0";

  std::cout << "\n";
  std::cout << "HIPXX_PLATFORM=" << HIPxxPlatformStr << std::endl;
  std::cout << "HIPXX_DEVICE_TYPE=" << HIPxxDeviceTypeStr << std::endl;
  std::cout << "HIPXX_DEVICE=" << HIPxxDeviceStr << std::endl;
  std::cout << "\n";
};

void _initialize(std::string BE) {
  std::string HIPxxPlatformStr, HIPxxDeviceTypeStr, HIPxxDeviceStr;
  read_env_vars(HIPxxPlatformStr, HIPxxDeviceTypeStr, HIPxxDeviceStr);
  logDebug("HIPxxDriver Initialize");
  // Get the current Backend Env Var

  
  // If no BE is passed to init explicitly, read env var
  std::string HIPXX_BE;
  if (BE.size() == 0) {
    HIPXX_BE = read_env_var("HIPXX_BE");
  } else {
    HIPXX_BE = BE;
  }

  if (!HIPXX_BE.compare("OPENCL")) {
    Backend = new HIPxxBackendOpenCL();
  } else if (!HIPXX_BE.compare("LEVEL0")) {
    logCritical("LEVEL0 Backend not yet implemented");
    std::abort();
  } else if (!HIPXX_BE.compare("")) {
    logWarn("HIPXX_BE was not set. Defaulting to OPENCL");
    Backend = new HIPxxBackendOpenCL();
  } else {
    logCritical("Invalid Backend Selection");
    std::abort();
  }
  Backend->initialize(HIPxxPlatformStr, HIPxxDeviceTypeStr, HIPxxDeviceStr);
}

void initialize(std::string BE) {
  std::call_once(initialized, &_initialize, BE);
};
