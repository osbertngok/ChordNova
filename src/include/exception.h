#ifndef CHORDNOVARW_SRC_INCLUDE_EXCEPTION_H_
#define CHORDNOVARW_SRC_INCLUDE_EXCEPTION_H_

#include <string>
#include <map>
#include <exception>
#include <utility>

#include "i18n.h"

namespace chordnovaexception {
using namespace chordnovalanguage;

enum class ChordNovaExceptionCode : uint16_t {
  CNEC_ResultNotFound = 0x01,
  CNEC_UnknownError = 0xff
};

class ChordNovaExceptionBase : public std::exception {
 public:
  ChordNovaExceptionBase(uint32_t code, std::string s) : _code(code), _s(std::move(s)) {
  }

  ~ChordNovaExceptionBase() noexcept override = default;

  char const *what() const noexcept override {
    return _s.c_str();
  }

  virtual const std::string &message() const {
    return _s;
  }

  virtual uint32_t GetCode() const {
    return _code;
  }

  virtual char const *GetCodeString(Language language) const = 0;

 protected:
  std::string _s;
  uint32_t _code;
};

class ChordNovaGenericException : public ChordNovaExceptionBase {

 public:

  explicit ChordNovaGenericException(const std::string& s) : ChordNovaExceptionBase(static_cast<uint32_t>(ChordNovaExceptionCode::CNEC_UnknownError), s) {}

  char const *GetCodeString(Language language) const override {
    switch (language) {
      case Language::Chinese:return "未知错误";
      case Language::English:
      default:return "Unknown Error";
    }
  }

};

}

#endif //CHORDNOVARW_SRC_INCLUDE_EXCEPTION_H_