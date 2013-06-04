<?php

/**
 * @file
 * Rules related actions, events and conditions.
 */

/**
 * Implements hook_rules_action_info().
 */
function cf_imagemagick_rules_action_info() {
  return array(
    'cf_imagemagick_png' => array(
      'label' => t('Convert file to .png (image/png) using ImageMagick'),
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
      'base' => 'cf_imagemagick_action_png',
    ),
  );
}

/**
 * Rules action convert file to png format.
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
function cf_imagemagick_action_png($file, $instance) {
  $report = FALSE;

  $report = cf_imagemagick_action_common($file, $instance, 'image/png', 'png');

  return $report;
}

/**
 * Convert a file using the ImageMagick PHP extension.
 *
 * @param stdClass $file
 *   File object to convert.
 * @param array $instance
 *   Field instance that this file was submitted to.
 * @param string $format
 *   Mime type to convert to
 * @param string $extension
 *   The file extension to append. Do not include dot (.).
 *
 * @return
 *   TRUE on success, FALSE on failure. Also, sets new property of 
 *   $file->convertfile['error'] to a string containing an error message 
 *   if an error was encountered, otherwise properties absense means
 *   success.
 *
 * @see cf_imagemagick_action_png()
 */
function cf_imagemagick_action_common($file, $instance, $format, $extension) {
  $report = FALSE;
  $settings = $instance['widget']['settings'];
  $dir = pathinfo($file->uri, PATHINFO_DIRNAME);
  $base = pathinfo($file->filename, PATHINFO_FILENAME);
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);
  $im = NULL;

  if (class_exists('Imagick')) {
    try {
      $im = new Imagick(drupal_realpath($file->uri));
      $im->setImageColorspace(255); 
      $im->setResolution(300, 300);
      $im->setCompressionQuality(95); 
      $im->setImageFormat('png'); 
      $write_success = $im->writeImage(drupal_realpath($file->uri));
      $im->clear(); 
      $im->destroy();
    } 
    catch (Exception $e) {
      $file->convertfile['error'] = $e->getMessage();
    }
    if ($write_success) {
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
      //$file->convertfile['error'] = 'Could not write file to disk.';
    }
  } 
  else {
    $file->convertfile['error'] = 'Could not detect ImageMagick PHP extension.';
  }

  return $report;
}
