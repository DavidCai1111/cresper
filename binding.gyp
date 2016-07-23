{
  "targets": [
    {
      "target_name": "cresper-binding",
      "sources": ["./src/cresper.cc"],
      "include_dirs" : ["<!(node -e \"require('nan')\")"]
    }
  ]
}
