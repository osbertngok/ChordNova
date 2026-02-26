#include <log4cxx/log4cxx.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include "logger.h"

namespace chordnovarw::logger {
void configure() {
  log4cxx::LevelPtr loglevelptr = log4cxx::Level::getDebug();
  const char *outputformatstr = "%d %c [%p] [%F:%L %M] %m%n";

  log4cxx::LayoutPtr consolePatternLayout(new log4cxx::PatternLayout(LOG4CXX_STR(outputformatstr)));
  log4cxx::ConsoleAppenderPtr consoleAppender(new log4cxx::ConsoleAppender(consolePatternLayout, log4cxx::ConsoleAppender::getSystemErr()));

  log4cxx::LoggerPtr root(log4cxx::Logger::getRootLogger());
  root->setLevel(loglevelptr);
  root->removeAllAppenders();
  root->addAppender(consoleAppender);
}
}

