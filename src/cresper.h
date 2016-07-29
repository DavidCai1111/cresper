#ifndef __CRSPER_H__
#define __CRSPER_H__

#include <string>
#include <nan.h>
#include <v8.h>
#include <node.h>

namespace cresper {

#define MAKE_BULK_STRING(stringToEncode)                      \
  (BULK_STRING_PREFIX                                         \
    + std::to_string(stringToEncode->ToString()->Utf8Length())\
    + CRLF                                                    \
    + string(*Nan::Utf8String(stringToEncode)) + CRLF)

#define MAKE_BUFFER(string)                                   \
  (Nan::CopyBuffer((string).c_str(), (string).length()))

#define RETURN_STRING_BUFFER(info, string)                    \
  info.GetReturnValue()                                       \
    .Set(Nan::CopyBuffer((string).c_str(), (string).length()) \
    .ToLocalChecked());

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
  static NAN_METHOD(encodeString);
  static NAN_METHOD(encodeError);
  static NAN_METHOD(encodeInt);
  static NAN_METHOD(encodeBulkString);
  static NAN_METHOD(encodeNull);
  static NAN_METHOD(encodeNullArray);
  static NAN_METHOD(encodeArray);
  static NAN_METHOD(encodeRequestArray);

  // Decoding functions
  static NAN_METHOD(decode);
};

}

#endif // !__CRSPER_H__
