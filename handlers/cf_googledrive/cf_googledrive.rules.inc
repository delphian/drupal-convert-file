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
      'label' => t('Convert file to .pdf (application/pdf) using Google Drive'),
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
    'cf_googledrive_txt' => array(
      'label' => t('Convert file to .txt (text/plain) using Google Drive'),
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
      'base' => 'cf_googledrive_action_txt',
    ),
    'cf_googledrive_odt' => array(
      'label' => t('Convert file to .odt (application/vnd.oasis.opendocument.text) using Google Drive'),
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
      'base' => 'cf_googledrive_action_odt',
    ),
  );
}

/**
 * Rules action convert file to pdf format.
 *
 * @param stdClass $file
 *   File object to convert.
 * @param array $instance
 *   Field instance that this file was submitted to.
 *
 * @return
 *   TRUE on success, FALSE on failure. Also, sets property
 *   $file->convertfile['error'] to a string containing an error message if
 *   an error was encountered, otherwise properties absense means success.
 *
 * @see cf_convertfile_rules_action_info()
 * @see convertfile_validate_conversion()
 */
function cf_googledrive_action_pdf($file, $instance) {
  $report = FALSE;

  $report = cf_googledrive_action_common($file, $instance, 'application/pdf', 'pdf');

  return $report;
}

/**
 * Rules action convert file to txt format.
 *
 * @param stdClass $file
 *   File object to convert.
 * @param array $instance
 *   Field instance that this file was submitted to.
 *
 * @return
 *   TRUE on success, FALSE on failure. Also, sets new property of 
 *   c$file->convertfile['error'] to a string containing an error message 
 *   if an error was encountered, otherwise properties absense means
 *   success.
 *
 * @see cf_convertfile_rules_action_info()
 * @see convertfile_validate_conversion()
 */
function cf_googledrive_action_txt($file, $instance) {
  $report = FALSE;

  $report = cf_googledrive_action_common($file, $instance, 'text/plain', 'txt');

  return $report;
}

/**
 * Rules action convert file to odt format.
 *
 * @param stdClass $file
 *   File object to convert.
 * @param array $instance
 *   Field instance that this file was submitted to.
 *
 * @return
 *   TRUE on success, FALSE on failure. Also, sets new property of 
 *   c$file->convertfile['error'] to a string containing an error message 
 *   if an error was encountered, otherwise properties absense means
 *   success.
 *
 * @see cf_convertfile_rules_action_info()
 * @see convertfile_validate_conversion()
 */
function cf_googledrive_action_odt($file, $instance) {
  $report = FALSE;

  $report = cf_googledrive_action_common($file, $instance, 'application/vnd.oasis.opendocument.text', 'odt');

  return $report;
}

/**
 * Upload a file to google drive, convert to google document, and re-download
 * in another format.
 *
 * @param stdClass $file
 *   File object to convert.
 * @param array $instance
 *   Field instance that this file was submitted to.
 * @param string $format
 *   Mime type to download a google document as.
 *
 * @return
 *   TRUE on success, FALSE on failure. Also, sets new property of 
 *   $file->convertfile['error'] to a string containing an error message 
 *   if an error was encountered, otherwise properties absense means
 *   success.
 *
 * @see cf_convertfile_action_pdf()
 * @see cf_convertfile_action_txt()
 * @see cf_convertfile_action_html()
 */
function cf_googledrive_action_common($file, $instance, $format, $extension) {
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
        if ($data = cf_googledrive_download_file($service, $g_file, $format)) {
          $fp = fopen($file->uri, "wbx");
          if (fwrite($fp, $data)) {
            $ext_new = $extension;
            $file->filename = $base . '.' . $ext . '.' . $ext_new;
            $file->filesize = filesize($file->uri);
            $file->filemime = file_get_mimetype($file->filename);
            // We only need to update the destination if this is a new upload.
            if (!isset($file->fid)) {
              $file->destination = $file->destination . '.' . $ext_new;
            }
            $report = TRUE;
          }
          else {
            $file->convertfile['error'] = 'Could not write file to disk.';
          }
        }
        else {
          $file->convertfile['error'] = 'Could not retrieve converted file.';
        }
        // Remove the file from Google Drive.
        if (!cf_googledrive_delete_file($service, $g_file->getId())) {
         watchdog('convertfile', 'Can not remove from google drive.', NULL, WATCHDOG_ERROR);
        }
      }
      else {
        $file->convertfile['error'] = 'Could not upload file.';
      }
    }
    else {
      $file->convertfile['error'] = 'Could not connect to service.';
    }
  } else {
    $file->convertfile['error'] = 'Could not find Google API PHP Client library.';
  }

  return $report;
}
