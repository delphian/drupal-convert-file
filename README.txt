A Drupal widget to automatically convert an uploaded file or image to
pdf or other sundry formats before being saved by drupal.

Two widgets will be installed, `Convert File` for file field types, and
`Convert Image` for image field types.

Editing the widget settings will show two new drop down options. The first
allows the selection of a provider that will deal with transformations, the
second will specify to what format the provider can change file/images to.

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
  $types = array(
    'cf_convertfile_gz' => '.gz (application/x-gzip)',
  );

  return array(
    'cf_convertfile' => array(
      'name' => 'Convert File',
      'types' => $types,
    ),
  );
}
```

A dirty implemenation is possible by using the callback variable and filling
it with the name of a function to do the conversion. This is not recommended,
however as an example the entire rules functionality could be bypassed by
calling the rules action function directly 
`'callback' => 'cf_compress_rules_action_info'`

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

Please do not contribute any handlers that use a function callback. Rules is
the recommended method of doing conversions. Copy and modify the
cf_compress handler as a template for a new handler.
