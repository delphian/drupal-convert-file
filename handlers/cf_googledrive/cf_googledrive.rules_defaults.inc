<?php

/**
 * @file
 * Provide default rules that are preconfigured.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function cf_googledrive_default_rules_configuration() {
  $configs = array();

  // Convert files to PDF.
  $rule = '{ "rules_cf_google_drive_convert_to_pdf" : {
    "LABEL" : "CF Google Drive convert to PDF",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "convertfile", "drive", "google", "pdf" ],
    "REQUIRES" : [ "convertfile", "cf_googledrive" ],
    "ON" : [ "convertfile_request" ],
    "IF" : [
      { "convertfile_condition_provider" : {
          "instance" : [ "instance" ],
          "provider" : { "value" : { "cf_googledrive" : "cf_googledrive" } }
        }
      },
      { "convertfile_condition_format" : {
          "instance" : [ "instance" ],
          "format" : { "value" : { "pdf" : "pdf" } }
        }
      },
      { "OR" : [
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "txt" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "docx" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "doc" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "xls" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "odt" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "xlsx" } }
        ]
      }
    ],
    "DO" : [
      { "cf_googledrive_pdf" : { "file" : [ "file" ], "instance" : [ "instance" ] } }
    ]
  }
}';

  $configs['cf_googledrive_pdf'] = rules_import($rule);

  // Convert files to TXT.
  $rule = '{ "rules_cf_google_drive_convert_to_txt" : {
    "LABEL" : "CF Google Drive convert to TXT",
    "PLUGIN" : "reaction rule",
    "TAGS" : [ "convertfile", "drive", "google", "txt" ],
    "REQUIRES" : [ "convertfile", "cf_googledrive" ],
    "ON" : [ "convertfile_request" ],
    "IF" : [
      { "convertfile_condition_provider" : {
          "instance" : [ "instance" ],
          "provider" : { "value" : { "cf_googledrive" : "cf_googledrive" } }
        }
      },
      { "convertfile_condition_format" : {
          "instance" : [ "instance" ],
          "format" : { "value" : { "txt" : "txt" } }
        }
      },
      { "OR" : [
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "pdf" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "docx" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "doc" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "xls" } },
          { "convertfile_condition_file_extension" : { "file" : [ "file" ], "extension" : "xlsx" } }
        ]
      }
    ],
    "DO" : [
      { "cf_googledrive_txt" : { "file" : [ "file" ], "instance" : [ "instance" ] } }
    ]
  }
}';

  $configs['cf_googledrive_txt'] = rules_import($rule);

  return $configs;
}
