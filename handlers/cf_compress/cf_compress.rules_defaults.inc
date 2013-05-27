<?php

function cf_compress_default_rules_configuration() {
  $configs = array();

  // Automkatically gzip all text files uploaded to 'convert_file' widgets.
  $rule = '{ "cf_compress_gzip_text" : {
    "LABEL" : "Compress text files uploaded",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "compress", "convertfile" ],
    "REQUIRES" : [ "convertfile", "cf_compress", "rules" ],
    "ON" : [ "convertfile_request" ],
    "IF" : [
      { "convertfile_condition_provider" : {
          "instance" : [ "instance" ],
          "provider" : { "value" : { "compress" : "compress" } }
        }
      },
      { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "txt" } }
    ],
    "DO" : [
      { "cf_compress_gzip" : { "file" : [ "file" ], "instance" : [ "instance" ] } },
      { "drupal_message" : { "message" : "Your file has been compressed to reduce disk usage." } }
    ]
  }
}';

  $configs['cf_compress_gzip_text'] = rules_import($rule);

  return $configs;
}