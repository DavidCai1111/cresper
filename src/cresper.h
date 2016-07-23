#ifndef __CRSPER_H__
#define __CRSPER_H__

#include <string>
#include <nan.h>
#include <v8.h>
#include <node.h>

namespace cresper {

#define RETURN_STRING_BUFFER(info, string)                    \
  info.GetReturnValue()                                       \
    .Set(Nan::CopyBuffer((string).c_str(), (string).length()) \
    .ToLocalChecked());

#define NODE_FUNCTION(name)                                   \
  static void name (const Nan::FunctionCallbackInfo<v8::Value>& info);

#define CHECK_ARG(isInvalid, info)                            \
  if (!isInvalid) {                                           \
    Nan::ThrowTypeError(info);                                \
    return;                                                   \
  }

extern const std::string CRLF = "\r\n";
extern const std::string STRING_PREFIX = "+";
extern const std::string INT_PREFIX = ":";
extern const std::string ERROR_PREFIX = "-";
extern const std::string BULK_STRING_PREFIX = "$";

class Cresper : public Nan::ObjectWrap {
public:
  static void Init (v8::Local<v8::Object>);

private:
  ~Cresper() = default;

  static inline Nan::Persistent<v8::Function>& constructor () {
    static Nan::Persistent<v8::Function> crepser_contructor;
    return crepser_contructor;
  };
  static void New (const Nan::FunctionCallbackInfo<v8::Value>& info);

  // Encoding functions
  NODE_FUNCTION(encodeString)
  NODE_FUNCTION(encodeError)
  NODE_FUNCTION(encodeInt)
  NODE_FUNCTION(encodeBulkString)
  NODE_FUNCTION(encodeNull)
  NODE_FUNCTION(encodeNullArray)
  NODE_FUNCTION(encodeArray)
  NODE_FUNCTION(encodeRequestArray)
};

}

#endif // !__CRSPER_H__
