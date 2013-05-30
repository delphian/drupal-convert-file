<?php

/**
 * @file
 * Provide default rules that are preconfigured.
 */

/**
 * Implements hook_default_rules_configuration().
 */

function convertfile_default_rules_configuration() {
  $configs = array();

  // Convert files to PDF success message.
  $rule = '{ "rules_cf_convert_to_pdf_success_message" : {
    "LABEL" : "CF convert to PDF success message",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "convertfile", "pdf", "success" ],
    "REQUIRES" : [ "convertfile", "rules" ],
    "ON" : [ "convertfile_success" ],
    "IF" : [
      { "NOT convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "pdf" } },
      { "convertfile_condition_format" : {
          "instance" : [ "instance" ],
          "format" : { "value" : { "pdf" : "pdf" } }
        }
      }
    ],
    "DO" : [
      { "drupal_message" : { "message" : "Your file has been automatically converted to the PDF format." } }
    ]
  }
}';

  $configs['rules_cf_convert_to_pdf_success_message'] = rules_import($rule);

  return $configs;
}
