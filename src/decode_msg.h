#ifndef __DECODE_MSG_H__
#define __DECODE_MSG_H__

#include <string>
#include <v8.h>

namespace cresper {

#define CHECK_MSG_PREFIX(msg, prefix)                         \
  msg.message[msg.index] == *prefix.c_str()

#define GET_DECODED(msg)                                      \
  DecodeMsg msgToDecode(msg.message, msg.index + 1);          \
  DecodeMsg decoded = _parse(msgToDecode);

struct DecodeMsg {
  DecodeMsg (std::string _message, int _index = 0) : message(_message), index(_index) {};
  DecodeMsg (v8::Local<v8::Value> _result, int _index = 0) : result(_result), index(_index) {};

  v8::Local<v8::Value> result;
  std::string message;
  int index;
};

DecodeMsg _decode (DecodeMsg& msg);
DecodeMsg _parse (DecodeMsg& msg);
DecodeMsg _decodeString (DecodeMsg& msg);
DecodeMsg _decodeError (DecodeMsg& msg);
DecodeMsg _decodtInt (DecodeMsg& msg);
DecodeMsg _decodeBulkString (DecodeMsg& msg);
DecodeMsg _decodeArray (DecodeMsg& msg);

}

#endif // !__DECODE_MSG_H__

