#ifndef CHORDNOVARW_SRC_INCLUDE_LOGGER_H_
#define CHORDNOVARW_SRC_INCLUDE_LOGGER_H_

#include <memory>
#include <string>
#include <stdexcept>
#include "log4cxx/consoleappender.h"
#include "log4cxx/patternlayout.h"
// #include "fmt/format.h"

namespace chordnovarw::logger {
void configure();
}

// Put this in cpp file if logging is required
#define LOGGER(name) static ::log4cxx::LoggerPtr logger = ::log4cxx::Logger::getLogger(name)

#define LOG_DEBUG(message) LOG4CXX_DEBUG(logger, message)
#define LOG_DEBUG_FORMAT(...) {  LOG4CXX_DEBUG_FMT(logger, __VA_ARGS__); }

#endif //CHORDNOVARW_SRC_INCLUDE_LOGGER_H_
