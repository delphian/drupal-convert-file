<?php

/**
 * Implements hook_rules_action_info().
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
 *   Sets new property of $file->convertfile['error'] to TRUE if an
 *   error was encountered, otherwise properties absense means success.
 *
 * @todo use standardized error reporting.
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
    $file->convertfile['error'] = TRUE;
  }
}
