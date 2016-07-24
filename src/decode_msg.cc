#include <v8.h>
#include <nan.h>
#include "cresper.h"
#include "decode_msg.h"

namespace cresper {

using namespace v8;
using std::string;

DecodeMsg _parse (DecodeMsg& msg) {
  size_t index = msg.message.find(CRLF, msg.index);
  if (index == string::npos)
    return DecodeMsg("", -1);

  return DecodeMsg(msg.message.substr(msg.index, index - msg.index),
    index + 2);
}

DecodeMsg _decodeString (DecodeMsg& msg) {
  GET_DECODED(msg)
  decoded.result = Nan::New<String>(decoded.message).ToLocalChecked();

  return decoded;
}

DecodeMsg _decodeError (DecodeMsg& msg) {
  GET_DECODED(msg)

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
}

DecodeMsg _decodtInt (DecodeMsg& msg) {
  GET_DECODED(msg)

  if (decoded.index == -1) {
    decoded.result = Nan::Undefined();
    return decoded;
  }

  decoded.result = Nan::New(std::stoi(decoded.message));
  return decoded;
}

DecodeMsg _decodeBulkString (DecodeMsg& msg) {
  GET_DECODED(msg)

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
}

DecodeMsg _decodeArray (DecodeMsg& msg) {
  GET_DECODED(msg)

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
  }

  Local<Array> result = Nan::New<Array>(arrLen);
  for (size_t i = 0; i < arrLen; i++) {
    DecodeMsg msgToDecode = DecodeMsg(msg.message, decoded.index);
    DecodeMsg _decoded = _decode(msgToDecode);
    Nan::Set(result, i, _decoded.result);
    decoded.index = _decoded.index;
  }

  return DecodeMsg(result);
}

DecodeMsg _decode (DecodeMsg& msg) {
  if (CHECK_MSG_PREFIX(msg, STRING_PREFIX)) // String
    return _decodeString(msg);
  else if (CHECK_MSG_PREFIX(msg, ERROR_PREFIX)) // Error
    return _decodeError(msg);
  else if (CHECK_MSG_PREFIX(msg, INT_PREFIX)) // Integer
    return _decodtInt(msg);
  else if (CHECK_MSG_PREFIX(msg, BULK_STRING_PREFIX)) // Bulk String
    return _decodeBulkString(msg);
  else if (CHECK_MSG_PREFIX(msg, ARRAY_PREFIX)) // Array
    return _decodeArray(msg);

  return Nan::Error(Nan::New("Missing prefix" + msg.message).ToLocalChecked());
}

}
