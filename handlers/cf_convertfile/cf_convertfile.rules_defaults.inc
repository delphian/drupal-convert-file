<?php

function cf_convertfile_default_rules_configuration() {
  $configs = array();

  // Automkatically gzip all text files uploaded to 'convert_file' widgets.
  $rule = '{ "cf_convertfile_gzip" : {
    "LABEL" : "Compress files upload with gzip",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "gzip", "convertfile" ],
    "REQUIRES" : [ "convertfile", "cf_convertfile", "rules" ],
    "ON" : [ "convertfile_request" ],
    "IF" : [
      { "convertfile_condition_provider" : {
          "instance" : [ "instance" ],
          "provider" : { "value" : { "compress" : "compress" } }
        }
      },
      { "convertfile_condition_format" : {
          "instance" : [ "instance" ],
          "format" : { "value" : { "cf_convertfile_gz" : "cf_convertfile_gz" } }
        }
      }
    ],
    "DO" : [
      { "cf_convertfile_gzip" : { "file" : [ "file" ], "instance" : [ "instance" ] } },
      { "drupal_message" : { "message" : "Your file has been compressed to reduce disk usage." } }
    ]
  }
}';

  $configs['cf_convertfile_gzip'] = rules_import($rule);

  return $configs;
}