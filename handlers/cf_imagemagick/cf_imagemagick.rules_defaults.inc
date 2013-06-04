<?php

/**
 * @file
 * Provide default rules that are preconfigured.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function cf_imagemagick_default_rules_configuration() {
  $configs = array();

  // Convert files to PDF.
  $rule = '{ "cf_imagemagick_convert_to_png" : {
    "LABEL" : "CF ImageMagick convert to PNG",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "convertfile", "imagemagick", "png" ],
    "REQUIRES" : [ "convertfile", "cf_imagemagick" ],
    "ON" : [ "convertfile_request" ],
    "IF" : [
      { "convertfile_condition_provider" : {
          "instance" : [ "instance" ],
          "provider" : { "value" : { "cf_imagemagick" : "cf_imagemagick" } }
        }
      },
      { "convertfile_condition_format" : {
          "instance" : [ "instance" ],
          "format" : { "value" : { "pdf" : "pdf" } }
        }
      },
      { "OR" : [
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "pdf" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "jpg" } },
        ]
      }
    ],
    "DO" : [
      { "cf_imagemagick_png" : { "file" : [ "file" ], "instance" : [ "instance" ] } }
    ]
  }
}';

  $configs['cf_imagemagick_png'] = rules_import($rule);

  return $configs;
}
