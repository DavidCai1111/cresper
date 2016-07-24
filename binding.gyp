{
  "targets": [
    {
      "target_name": "cresper-binding",
      "sources": ["./src/cresper.cc"],
      "include_dirs" : ["<!(node -e \"require('nan')\")"],
      "cflags": [
        "-std=c++11",
        "-stdlib=libc++"
      ],
      "xcode_settings": {
        'OTHER_CFLAGS': [
          "-std=c++11",
          "-stdlib=libc++"
        ]
      },
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
          }
        }]
      ]
    }
  ]
}
