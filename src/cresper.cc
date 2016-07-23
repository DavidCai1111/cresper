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
  tpl->InstanceTemplate()->SetInternalFieldCount(9);

  Nan::SetPrototypeMethod(tpl, "encodeString", encodeString);
  Nan::SetPrototypeMethod(tpl, "encodeInt", encodeInt);
  Nan::SetPrototypeMethod(tpl, "encodeError", encodeError);
  Nan::SetPrototypeMethod(tpl, "encodeNull", encodeNull);
  Nan::SetPrototypeMethod(tpl, "encodeBulkString", encodeBulkString);
  Nan::SetPrototypeMethod(tpl, "encodeNullArray", encodeNullArray);
  Nan::SetPrototypeMethod(tpl, "encodeArray", encodeArray);
  Nan::SetPrototypeMethod(tpl, "encodeRequestArray", encodeRequestArray);

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

inline string Cresper::_encodeBulkString (const v8::Local<v8::Value>& stringToEncode) {
  return BULK_STRING_PREFIX
    + SSTR(stringToEncode->ToString()->Utf8Length()) + CRLF
    + string(*Nan::Utf8String(stringToEncode)) + CRLF;
}

NAN_METHOD(Cresper::encodeBulkString) {
  Local<Value> stringToEncode = info[0];

  CHECK_ARG(stringToEncode->IsString(), "Argument should be a string");

  string encoded = _encodeBulkString(stringToEncode);

  RETURN_STRING_BUFFER(info, encoded)
}

NAN_METHOD(Cresper::encodeNull) {
  RETURN_STRING_BUFFER(info, BULK_STRING_PREFIX + "-1" + CRLF)
}

NAN_METHOD(Cresper::encodeNullArray) {
  RETURN_STRING_BUFFER(info, ARRAY_PREFIX + "-1" + CRLF)
}

NAN_METHOD(Cresper::encodeArray) {
  CHECK_ARG(info[0]->IsArray(), "Argument should be an array");

  string encoded = _encodeArray(Local<Array>::Cast(info[0]));

  RETURN_STRING_BUFFER(info, encoded);
}

string Cresper::_encodeArray (const Local<Array>& arrayToEncode) {
  size_t len = arrayToEncode->Length();
  string encoded = ARRAY_PREFIX + SSTR(len) + CRLF;

  for (size_t i = 0; i < len; i++) {
    Local<Value> element = Nan::Get(arrayToEncode, i).ToLocalChecked();

    if (element->IsArray()) {
      encoded += _encodeArray(Local<Array>::Cast(element));
    } else {
      if (!element->IsUint8Array())
        Nan::ThrowTypeError("Each array element should be an instance of Buffer");

      encoded += string(*Nan::Utf8String(element->ToString()));
    }
  }

  return encoded;
}

NAN_METHOD(Cresper::encodeRequestArray) {
  CHECK_ARG(info[0]->IsArray(), "Argument should be an array");
  Local<Array> arrayToEncode = Local<Array>::Cast(info[0]);
  size_t len = arrayToEncode->Length();

  Local<Array> request = Nan::New<Array>(len);

  for (size_t i = 0; i < len; i++) {
    Nan::Set(request, i, MAKE_BUFFER(_encodeBulkString(arrayToEncode->Get(i)))
      .ToLocalChecked());
  }

  string encoded = _encodeArray(request);

  RETURN_STRING_BUFFER(info, encoded)
}

NODE_MODULE(cresper, Cresper::Init)
}
