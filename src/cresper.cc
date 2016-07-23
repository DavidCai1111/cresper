#include <string>
#include <nan.h>
#include <v8.h>
#include <node.h>
#include "cresper.h"

namespace cresper {

using namespace v8;
using std::string;

NAN_MODULE_INIT(Cresper::Init) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Cresper").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(4);

  Nan::SetPrototypeMethod(tpl, "encodeString", encodeString);
  Nan::SetPrototypeMethod(tpl, "encodeInt", encodeInt);
  Nan::SetPrototypeMethod(tpl, "encodeError", encodeError);
  Nan::SetPrototypeMethod(tpl, "encodeNull", encodeNull);
  Nan::SetPrototypeMethod(tpl, "encodeBulkString", encodeBulkString);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Cresper").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Cresper::New) {
  Cresper* cresper = new Cresper();
  cresper->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(Cresper::encodeString) {
  Local<Value> stringToEncode = info[0];

  CHECK_ARG(stringToEncode->IsString(), "Argument should be a string");

  RETURN_STRING_BUFFER(info, STRING_PREFIX + string(*Nan::Utf8String(stringToEncode)) + CRLF)
}

NAN_METHOD(Cresper::encodeInt) {
  Local<Value> intToEncode = info[0];

  CHECK_ARG(intToEncode->IsNumber(), "Argument should be an integer");

  RETURN_STRING_BUFFER(info, INT_PREFIX + string(*Nan::Utf8String(intToEncode)) + CRLF)
}

NAN_METHOD(Cresper::encodeError) {
  Local<Value> errorToEncode = info[0];
  bool isError = Nan::Equals(Nan::ObjectProtoToString(errorToEncode->ToObject()).ToLocalChecked(),
    Nan::New("[object Error]").ToLocalChecked()).FromJust();

  CHECK_ARG(isError, "Argument should be an instance of Error");

  RETURN_STRING_BUFFER(info, ERROR_PREFIX + string(*Nan::Utf8String(errorToEncode)) + CRLF)
}

NAN_METHOD(Cresper::encodeBulkString) {
  Local<Value> stringToEncode = info[0];

  CHECK_ARG(stringToEncode->IsString(), "Argument should be a string");

  RETURN_STRING_BUFFER(info, BULK_STRING_PREFIX
    + stringToEncode->ToString()->Utf8Length() + CRLF
    + string(*Nan::Utf8String(stringToEncode)) + CRLF)
}

NAN_METHOD(Cresper::encodeNull) {
  RETURN_STRING_BUFFER(info, BULK_STRING_PREFIX + "-1" + CRLF)
}

NODE_MODULE(cresper, Cresper::Init)
}
