<?php

/**
 * @file
 * Rules related actions, events and conditions.
 */

/**
 * Implements hook_rules_action_info().
 */
function cf_googledrive_rules_action_info() {
  return array(
    'cf_googledrive_pdf' => array(
      'label' => t('Convert file to .pdf (application/pdf)'),
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
      'base' => 'cf_googledrive_action_pdf',
    ),
  );
}

/**
 * Rules action convert file to pdf format.
 *
 * @param stdClass $file
 *   File object that is being readied to be moved from temporary server
 *   upload bin.
 * @param array $instance
 *   Field instance that this file was submitted to.
 *
 * @return
 *   Sets new property of convertfile_error to a string containing an error
 *   message on the $file object if an error was encountered, otherwise
 *   properties absense means success.
 *
 * @see cf_convertfile_rules_action_info()
 */
function cf_googledrive_action_pdf($file, $instance) {
  $report = FALSE;
  $settings = $instance['widget']['settings'];
  $dir = pathinfo($file->uri, PATHINFO_DIRNAME);
  $base = pathinfo($file->filename, PATHINFO_FILENAME);
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);

  if (($library = libraries_load('google-api-php-client')) && !empty($library['loaded'])) {
    module_load_include('inc', 'cf_googledrive', 'cf_googledrive.api');
    if ($service = cf_googledrive_service(cf_googledrive_client())) {
      $g_file = cf_googledrive_insert_file($service, $file->filename, 'temporary', NULL, $file->filemime, $file->uri);
      if ($g_file) {
        if ($data = cf_googledrive_download_file($service, $g_file, 'application/pdf')) {
          $fp = fopen($file->uri, "wbx");
          if (fwrite($fp, $data)) {
            $ext_new = 'pdf';
            $file->filename = $base . '.' . $ext . '.' . $ext_new;
            $file->filesize = filesize($file->uri);
            $file->filemime = file_get_mimetype($file->filename);
            $file->destination = $file->destination . '.' . $ext_new;
            $report = TRUE;
          }
          else {
            $file->convertfile_error = 'Could not write file to disk.';
          }
        }
        else {
          $file->convertfile_error = 'Could not retrieve converted file.';
        }
        // Remove the file from Google Drive.
        if (!cf_googledrive_delete_file($service, $g_file->getId())) {
          watchdog('convertfile', 'Can not remove from google drive.', NULL, WATCHDOG_ERROR);
        }
      }
      else {
        $file->convertfile_error = 'Could not upload file.';
      }
    }
    else {
      $file->convertfile_error = 'Could not connect to service.';
    }
  } else {
    $file->convertfile_error = 'Could not find Google API PHP Client library.';
  }

  return $report;
}
