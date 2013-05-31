## Drupal Convert File ##

A Drupal widget to automatically convert an uploaded file or image to
pdf or other sundry formats before being saved by drupal.

Two widgets will be installed, `Convert File` for file field types, and
`Convert Image` for image field types.

Editing the widget settings will show two new drop down options. The first
allows the selection of a provider that will deal with transformations, the
second will specify to what format the provider can change file/images to.

All the heavy lifting occurs in file upload validators while the file is still
located in the temporary apache upload directory, and before drupal has
formally saved the file.

### Default providers ###

Two default providers come with the module: `Convert File` and `Google Drive`.
Convert File will gzip an uploaded file, while Google Drive will convert the
file into pdf format. These providers are not active by default and must be
enabled via the modules page.

### Installation instructions with drush ###

Clone the repository into the sites/all/modules/custom directory:
1. `git clone --recursive git://github.com/delphian/drupal-convert-file.git`

Download the Google PHP SDK library 
(https://code.google.com/p/google-api-php-client/) into sites/all/libraries
directory:
1. `curl "http://google-api-php-client.googlecode.com/files/google-api-php-client-0.6.2.tar.gz" -O`
2. `tar -xvf  google-api-php-client-0.6.2.tar.gz`

Enable modules:
`drush en convertfile cf_googledrive cf_convertfile -y`

Create a new file field on a piece of content and attach the `Convert File`
widget:
1. Visit /admin/structure/types/manage/page/fields
2. Add new field: `test file`, Field type: `file`, Widget: `Convert File`, Click Save.
3. Click <b>Save field settings</b> on next page.
4. Near the middle of <b>Basic Page Settings</b>:
5. Add the `pdf` format to <b>Allowed file extensions</b>.
6. At the bottom of <b>Basic Page Settings</b>:
7. select `Convert File` from the <b>Convert using provider</b> dropdown.
8. Select `.pdf (application/pdf) from <b>Convert to format</b> dropdown.
9. Click <b>Save settings</b>

### Development with hooks and rules ###

A hook is provided for third party modules to declare themselves as a
provider for file conversions: `hook_convertfile_info()`.

The example info hook below registers a callback function that will handle
gzip conversions.

```php
/**
 * Implements hook_convertfile_info().
 *
 * Register our handler and formats with convertfile module.
 */
function cf_convertfile_convertfile_info() {
  return array(
    'cf_convertfile' => array(
      'name' => 'Convert File',
      'types' => array (
        'gz' => '.gz (application/x-gzip)',
      ),
    ),
  );
}
```

Now fields that use the file field widget `Convert File` will have an option
to select the provider as `Convert File` and a format of 
`.gz (application/x-gzip)`

When a file is uploaded into this field a custom #upload_validators callback
will be executed. This callback will invoke the rule event `File conversion has 
been requested`.

Each provider will declare its own rules actions, generally one for each
format provided, and provide default rules that listen for the file conversion
event.

```php
/**
 * Implements hook_rules_action_info().
 *
 * Register the convertfile gzip action with rules.
 */
function cf_convertfile_rules_action_info() {
  return array(
    'cf_convertfile_gzip' => array(
      'label' => t('Convert file to .gz (application/x-gzip) using Convert File'),
      'parameter' => array(
        'file' => array(
          'type' => 'file',
          'label' => t('File to convert'),
        ),
        'instance' => array(
          'type' => 'struct',
          'label' => t('Field instance data'),
        ),
      ),
      'group' => t('Convert File'),
      'base' => 'cf_convertfile_action_gzip',
    ),
  );
}
```

```php
/**
 * Rules action to gzip a file.
 *
 * @param stdClass $file
 *   File object that is being readied to be moved from temporary server
 *   upload bin.
 * @param array $instance
 *   Field instance that this file was submitted to.
 *
 * @return
 *   Sets new property of convertfile_error to TRUE on the $file object if an
 *   error was encountered, otherwise properties absense means success.
 *
 * @see cf_convertfile_rules_action_info()
 */
function cf_convertfile_action_gzip($file, $instance) {
  $success = FALSE;
  $settings = $instance['widget']['settings'];
  $dir = pathinfo($file->uri, PATHINFO_DIRNAME);
  $base = pathinfo($file->filename, PATHINFO_FILENAME);
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);

  if (($contents = file_get_contents($file->uri)) !== FALSE) {
    if ($fp = gzopen($file->uri, 'w9')) {
      if (gzwrite($fp, $contents)) {
        $ext_new = 'gz';
        $file->filename = $base . '.' . $ext . '.' . $ext_new;
        $file->filesize = filesize($file->uri);
        $file->filemime = file_get_mimetype($file->filename);
        $file->destination = $file->destination . '.' . $ext_new;
        $success = TRUE;
      }
      gzclose($fp);
    }
  }

  if (!$success) {
    $file->convertfile_error = TRUE;
  }
}
```

Listen for the convert file request event and execute our gzip action if
appropriate conditions are met:

```php
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
```

### Development with custom Form API element type ###

Convert file can be initiated while using the Form API by specifying the
custom element type `convertfile_file`

```php
$form['test_conversion'] = array(
  '#type' => 'convertfile_file',
  '#title' => 'Test file conversion',
  '#convertfile_provider' => 'cf_googledrive',
  '#convertfile_format' => 'pdf',
  '#description' => 'Test out the Form API element type convertfile_file by ' . 
    'specifying the googledrive provider and pdf format',
);
```

### Support, issues and questions ###

The latest relase can be cloned with: `git clone --recursive git://github.com/delphian/drupal-convert-file.git`

Please submit all issues to https://github.com/delphian/drupal-convert-file/issues

Feel free to contact me bryan.hazelbaker@gmail.com with any questions.
