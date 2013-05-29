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
function cf_compress_convertfile_info() {
  $types = array();
  $types['cf_compress_gz'] = '.gz (application/x-gzip)';

  return array(
    'compress' => array(
      'name' => 'Compress',
      'callback' => NULL,
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
 * @see cf_compress_rules_action_info()
 */
function cf_compress_action_gzip($file, $instance) {
  $settings = $instance['widget']['settings'];
  $dir = pathinfo($file->uri, PATHINFO_DIRNAME);
  $base = pathinfo($file->filename, PATHINFO_FILENAME);
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);

  $contents = file_get_contents($file->uri);
  $fp = gzopen($file->uri, 'w9');
  gzwrite($fp, $contents);
  gzclose($fp);

  $ext_new = 'gz';
  $file->filename = $base . '.' . $ext . '.' . $ext_new;
  $file->filesize = filesize($file->uri);
  $file->filemime = file_get_mimetype($file->filename);
  $file->destination = $file->destination . '.' . $ext_new;
}

```

Please do not contribute any handlers that use a function callback. Rules is
the recommended method of doing conversions. Copy and modify the
cf_compress handler as a template for a new handler.
