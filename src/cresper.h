#ifndef __CRSPER_H__
#define __CRSPER_H__

#include <string>
#include <nan.h>
#include <v8.h>
#include <node.h>

namespace cresper {

#define MAKE_BUFFER(string)                                   \
  (Nan::CopyBuffer((string).c_str(), (string).length()))

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

extern const std::string CRLF;
extern const std::string STRING_PREFIX;
extern const std::string INT_PREFIX;
extern const std::string ERROR_PREFIX;
extern const std::string BULK_STRING_PREFIX;
extern const std::string ARRAY_PREFIX;

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
  static std::string _encodeArray (const v8::Local<v8::Array>& arrayToEncode);
  static inline std::string _encodeBulkString (const v8::Local<v8::Value>& stringToEncode);
  NODE_FUNCTION(encodeString)
  NODE_FUNCTION(encodeError)
  NODE_FUNCTION(encodeInt)
  NODE_FUNCTION(encodeBulkString)
  NODE_FUNCTION(encodeNull)
  NODE_FUNCTION(encodeNullArray)
  NODE_FUNCTION(encodeArray)
  NODE_FUNCTION(encodeRequestArray)

  // Decoding functions
  // static v8::Local<v8::Value> _decode (DecodeMsg& msg);
  NODE_FUNCTION(decode)
};

}

#endif // !__CRSPER_H__
