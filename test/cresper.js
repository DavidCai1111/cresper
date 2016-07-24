'use strict'
/* global describe, it */
const should = require('should')
const Cresper = require('../lib/cresper')
const CRLF = '\r\n'

describe('Cresper test', () => {
  let cresper = new Cresper()

  it('Should throw when type of param is wrong', () => {
    (() => cresper.encodeString(1)).should.throw(/should be a string/);

    (() => cresper.encodeError(1)).should.throw(/should be an instance of Error/);

    (() => cresper.encodeInt('jaja')).should.throw(/should be an integer/);

    (() => cresper.encodeBulkString(1)).should.throw(/should be a string/);

    (() => cresper.encodeArray('jaja')).should.throw(/should be an array/);

    (() => cresper.encodeArray(['jaja'])).should.throw(/should be an instance of Buffer/);

    (() => cresper.decode(['jaja'])).should.throw(/should be an instance of Buffer/)
  })

  it('Should decode right', () => {
    cresper.decode(cresper.encodeString('jaja')).should.eql('jaja')

    let error = cresper.decode(cresper.encodeError(new TypeError('heheda')))
    error.name.should.eql('TypeError:')
    error.message.should.eql('heheda')

    cresper.decode(cresper.encodeInt(998)).should.eql(998)
    cresper.decode(cresper.encodeBulkString('bulkjaja')).should.eql('bulkjaja')
    should(cresper.decode(cresper.encodeNull())).be.null()
    let _err = new Error('heheda')
    _err.name = 'Error:'
    cresper.decode(cresper.encodeArray([
      cresper.encodeInt(1),
      cresper.encodeString('str'),
      cresper.encodeNullArray(),
      cresper.encodeError(new Error('heheda'))
    ])).should.eql([1, 'str', null, _err])
  })

  it('Should encode string right', () => {
    cresper.encodeString('jaja').toString().should.eql(`+jaja${CRLF}`)
  })

  it('Should encode error right', () => {
    cresper.encodeError(new TypeError('jaja')).toString().should.eql(`-TypeError: jaja${CRLF}`)
  })

  it('Should encode integer right', () => {
    cresper.encodeInt(998).toString().should.eql(`:998${CRLF}`)
  })

  it('Should encode bulkStr right', () => {
    cresper.encodeBulkString('jaja').toString().should.eql(`$4${CRLF}jaja${CRLF}`)
  })

  it('Should encode null right', () => {
    cresper.encodeNull().toString().should.eql(`$-1${CRLF}`)
  })

  it('Should encode nullArray right', () => {
    cresper.encodeNullArray().toString().should.eql(`*-1${CRLF}`)
  })

  it('Should encode array right', () => {
    cresper.encodeArray([
      cresper.encodeInt(1),
      cresper.encodeString('str'),
      cresper.encodeNullArray(),
      cresper.encodeError(new Error('heheda'))
    ]).toString().should.eql(`*4${CRLF}:1${CRLF}+str${CRLF}*-1${CRLF}-Error: heheda${CRLF}`)

    cresper.encodeArray([
      cresper.encodeInt(1),
      cresper.encodeString('str'),
      [
        cresper.encodeNullArray(),
        cresper.encodeError(new Error('heheda'))
      ]
    ]).toString().should.eql(`*3${CRLF}:1${CRLF}+str${CRLF}*2${CRLF}*-1${CRLF}-Error: heheda${CRLF}`)
  })

  it('Should encode request right', () => {
    cresper.encodeRequestArray(['LLEN', 'mylist']).toString().should.eql(`*2${CRLF}$4${CRLF}LLEN${CRLF}$6${CRLF}mylist${CRLF}`)
  })
})
