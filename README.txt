A Drupal widget to automatically convert the files or images submitted to it
from one format to another before saving.

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
 */
function convertfile_convertfile_info() {
  return array(
    'convertfile' => array(
      'name' => 'Convert File Module',
      'callback' => 'convertfile_do_conversion',
      'types' => array(
        'gz' => '.gz (application/x-gzip)',
      ),
    ),
  );
}
```

The example callback below will automatically gzip all files:

```php
/**
 * Convert our files.
 *
 * @todo we are messing with the destination file name, this could crash into
 * an existing filename that was not checked (with the new extension).
 */
function convertfile_do_conversion($file, $instance) {
  $settings = $instance['widget']['settings'];
  $dir = pathinfo($file->uri, PATHINFO_DIRNAME);
  $base = pathinfo($file->filename, PATHINFO_FILENAME);
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);

  // Gzip the file.
  if ($settings['convertfile_format'] == 'gz' && $ext != 'gz') {
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

  return array();
}
```
