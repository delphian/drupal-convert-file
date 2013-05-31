<?php

/**
 * Implements hook_default_rules_configuration().
 */
function cf_convertfile_default_rules_configuration() {
  $configs = array();

  // Automkatically gzip all text files uploaded to 'convert_file' widgets.
  $rule = '{ "cf_convertfile_gzip" : {
    "LABEL" : "CF Convert File convert to GZ",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "gzip", "convertfile", "gz" ],
    "REQUIRES" : [ "convertfile", "cf_convertfile", "rules" ],
    "ON" : [ "convertfile_request" ],
    "IF" : [
      { "convertfile_condition_provider" : {
          "instance" : [ "instance" ],
          "provider" : { "value" : { "cf_convertfile" : "cf_convertfile" } }
        }
      },
      { "convertfile_condition_format" : {
          "instance" : [ "instance" ],
          "format" : { "value" : { "gz" : "gz" } }
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
