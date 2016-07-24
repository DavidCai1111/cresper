# cresper
[![js-standard-style](https://img.shields.io/badge/code%20style-standard-brightgreen.svg)](http://standardjs.com/)

A parser for RESP (REdis Serialization Protocol) which purely using C++ addon.

## Install

```sh
npm install cresper
```

## Usage

```js
'use strict'
const Resper = require('cresper')

let resper = new Resper()

resper.decode(resper.encodeArray([
  resper.encodeInt(1),
  resper.encodeString('str'),
  resper.encodeNullArray(),
  resper.encodeError(new Error('heheda'))
]))

resper.encodeRequestArray(['LLEN', 'mylist'])
```

## API

### Class: Resper

### Method: encodeString(str)

Encode `str` to RESP buffer.

### Method: encodeError(err)

Encode `err` to RESP buffer.

### Method: encodeInt(int)

Encode `int` to RESP buffer.

### Method: encodeBulkString(bulk)

Encode `bluk` to RESP buffer, `bluk` should be a String.

### Method: encodeNull()

Get the RESP Null buffer.

### Method: encodeNullArray()

Get the RESP NullArray buffer.

### Method: encodeArray(arr)

Encode `arr` to RESP buffer, each element in `arr` should be an instance of buffer.

### Method: encodeRequestArray(requestArr)

Encode `requestArr` to RESP request buffer, each element in `requestArr` should be a string.

```
resper.encodeRequestArray(['LLEN', 'mylist'])
```

### Method: decode(encodedBuffer)

Decode RESP buffer to real value.

```js
Resper.decode(Resper.encodeInt(998)) // 998
```
