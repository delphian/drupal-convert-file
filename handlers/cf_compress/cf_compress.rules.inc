<?

/**
 * Implementation of hook_rules_event_info().
 * @ingroup rules
 */
function cf_compress_rules_event_info() {
  return array(
    'cf_compress_request' => array(
      'group' => 'Convert File',
      'label' => t('CF Handler Compress has been requested.'),
      'module' => 'cf_compress',
      'variables' => array(
        'file' => array('type' => 'file', 'label' => t('The file on which compression has been requested.')),
        'instance' => array('type' => 'struct', 'label' => t('Field instance that file is attached to.')),
      ),
    ),
  );
}

/**
 * Implements hook_rules_action_info().
 */
function cf_compress_rules_action_info() {
  return array(
    'cf_compress_gzip' => array(
      'label' => t('Convert file to .gz (application/x-gzip)'), 
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
      'base' => 'cf_compress_action_gzip',
    ),
  );
}

/**
 * Rules action to gzip a file.
 *
 * @see cf_compress_rules_action_info()
 */
function cf_compress_action_gzip($file, $instance) {
  $settings = $instance['widget']['settings'];
  $orig_file = $file;
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
