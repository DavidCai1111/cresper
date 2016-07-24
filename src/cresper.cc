#include <stdexcept>
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
  Nan::SetPrototypeMethod(tpl, "decode", decode);

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
  const bool isError = Nan::Equals(Nan::ObjectProtoToString(errorToEncode->ToObject()).ToLocalChecked(),
    Nan::New("[object Error]").ToLocalChecked()).FromJust();

  CHECK_ARG(isError, "Argument should be an instance of Error");

  RETURN_STRING_BUFFER(info, ERROR_PREFIX + string(*Nan::Utf8String(errorToEncode)) + CRLF)
}

inline string Cresper::_encodeBulkString (const v8::Local<v8::Value>& stringToEncode) {
  return BULK_STRING_PREFIX
    + std::to_string(stringToEncode->ToString()->Utf8Length()) + CRLF
    + string(*Nan::Utf8String(stringToEncode)) + CRLF;
}

NAN_METHOD(Cresper::encodeBulkString) {
  Local<Value> stringToEncode = info[0];

  CHECK_ARG(stringToEncode->IsString(), "Argument should be a string");

  const string encoded = _encodeBulkString(stringToEncode);

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

  const string encoded = _encodeArray(Local<Array>::Cast(info[0]));

  RETURN_STRING_BUFFER(info, encoded);
}

string Cresper::_encodeArray (const Local<Array>& arrayToEncode) {
  const size_t len = arrayToEncode->Length();
  string encoded = ARRAY_PREFIX + std::to_string(len) + CRLF;

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
  const size_t len = arrayToEncode->Length();

  Local<Array> request = Nan::New<Array>(len);

  for (size_t i = 0; i < len; i++) {
    Nan::Set(request, i, MAKE_BUFFER(_encodeBulkString(arrayToEncode->Get(i)))
      .ToLocalChecked());
  }

  const string encoded = _encodeArray(request);

  RETURN_STRING_BUFFER(info, encoded)
}

DecodeMsg _parse (DecodeMsg& msg) {
  size_t index = msg.message.find(CRLF, msg.index);
  if (index == string::npos)
    return DecodeMsg("", -1);

  return DecodeMsg(msg.message.substr(msg.index, index - msg.index),
    index + 2);
}

DecodeMsg _decode (DecodeMsg& msg) {
  if (CHECK_MSG_PREFIX(msg, STRING_PREFIX)) { // String
    DecodeMsg msgToDecode(msg.message, msg.index + 1);
    DecodeMsg decoded = _parse(msgToDecode);
    decoded.result = Nan::New(decoded.message).ToLocalChecked();

    return decoded;
  } else if (CHECK_MSG_PREFIX(msg, ERROR_PREFIX)) { // Error
    DecodeMsg msgToDecode(msg.message, msg.index + 1);
    DecodeMsg decoded = _parse(msgToDecode);

    if (decoded.index == -1) {
      decoded.result = Nan::Undefined();
      return decoded;
    }

    size_t spaceIndex = decoded.message.find(" ");
    if (spaceIndex != string::npos) {
      Local<Value> error = Nan::Error(decoded.message.substr(spaceIndex + 1).c_str());
      Nan::Set(error->ToObject(), Nan::New("name").ToLocalChecked(),
        Nan::New(decoded.message.substr(0, spaceIndex)).ToLocalChecked());

      decoded.result = error;
    } else {
      decoded.result = Nan::Error(decoded.message.c_str());
    }

    return decoded;
  } else if (CHECK_MSG_PREFIX(msg, INT_PREFIX)) { // Integer
    DecodeMsg msgToDecode(msg.message, msg.index + 1);
    DecodeMsg decoded = _parse(msgToDecode);

    if (decoded.index == -1) {
      decoded.result = Nan::Undefined();
      return decoded;
    }

    decoded.result = Nan::New(std::stoi(decoded.message));
    return decoded;
  } else if (CHECK_MSG_PREFIX(msg, BULK_STRING_PREFIX)) { // Bulk String
    DecodeMsg msgToDecode(msg.message, msg.index + 1);
    DecodeMsg decoded = _parse(msgToDecode);

    if (decoded.index == -1) {
      decoded.result = Nan::Undefined();
      return decoded;
    }

    int bulkStartIndex = decoded.index;
    int bulkLen;

    try {
      bulkLen = std::stoi(decoded.message);
    } catch (const std::invalid_argument& err) {
      Nan::ThrowError(Nan::New("Invaild bulk string length").ToLocalChecked());
    }

    int bulkEndIndex = bulkStartIndex + bulkLen;
    decoded.index = decoded.index + bulkLen + 2;

    if (bulkLen == -1 || msg.message.length() < bulkEndIndex + CRLF.length()) {
      decoded.result = Nan::Null();
    } else {
      decoded.result = Nan::New(msg.message.substr(bulkStartIndex, bulkEndIndex - bulkStartIndex)).ToLocalChecked();
    }

    return decoded;
  } else if (CHECK_MSG_PREFIX(msg, ARRAY_PREFIX)) { // Array
    DecodeMsg msgToDecode(msg.message, msg.index + 1);
    DecodeMsg decoded = _parse(msgToDecode);

    if (decoded.index == -1) {
      decoded.result = Nan::Undefined();
      return decoded;
    }

    size_t arrLen;
    try {
      arrLen = std::stoi(decoded.message);
    } catch (const std::invalid_argument& err) {
      decoded.result = Nan::Null();
      return decoded;
    }

    if (static_cast<int>(arrLen) == -1) {
      decoded.result = Nan::Null();
      return decoded;
    } else if (static_cast<int>(arrLen) < -1) {
      Nan::ThrowError(Nan::New("Invaild array length").ToLocalChecked());
    } else {
      Local<Array> result = Nan::New<Array>(arrLen);
      for (size_t i = 0; i < arrLen; i++) {
        DecodeMsg msgToDecode = DecodeMsg(msg.message, decoded.index);
        DecodeMsg _decoded = _decode(msgToDecode);
        Nan::Set(result, i, _decoded.result);
        decoded.index = _decoded.index;
      }
      return DecodeMsg(result);
    }
  }

  return Nan::Error(Nan::New("Missing prefix" + msg.message).ToLocalChecked());
}

NAN_METHOD(Cresper::decode) {
  CHECK_ARG(info[0]->IsUint8Array(), "Argument should be an instance of Buffer");

  DecodeMsg msg(string(*Nan::Utf8String(info[0]->ToString())));
  Local<Value> decoded = _decode(msg).result;

  return info.GetReturnValue()
    .Set(decoded);
}

NODE_MODULE(cresper, Cresper::Init)
}
