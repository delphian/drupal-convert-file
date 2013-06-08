<?php

/**
 * @file
 * Rules related actions, events and conditions for ImageMagick.
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
    'cf_imagemagick_jpg' => array(
      'label' => t('Convert file to .jpg (image/jpeg) using ImageMagick'),
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
      'base' => 'cf_imagemagick_action_jpg',
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
 * Rules action convert file to jpg format.
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
function cf_imagemagick_action_jpg($file, $instance) {
  $report = FALSE;

  $report = cf_imagemagick_action_common($file, $instance, 'image/jpeg', 'jpg');

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
 * @see http://php.net/manual/en/imagick.writeimage.php
 */
function cf_imagemagick_action_common($file, $instance, $format, $extension) {
  $report = FALSE;
  $settings = $instance['widget']['settings'];
  $im_opt = $settings['convertfile_ajax']['convertfile_specific']['cf_imfs'];
  $dir = pathinfo($file->uri, PATHINFO_DIRNAME);
  $base = pathinfo($file->filename, PATHINFO_FILENAME);
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);
  $im = NULL;

  if (class_exists('Imagick')) {
    try {
      $im = new Imagick();
      $im = cf_imagemagick_action_settings($im, $im_opt, $file);
      $im->setImageFormat($extension);
      if (file_put_contents($file->uri, $im) !== FALSE) {
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
      $im->clear(); 
      $im->destroy();
    } 
    catch (Exception $e) {
      $file->convertfile['error'] = $e->getMessage();
    }
  }
  else {
    $file->convertfile['error'] = 'Could not detect ImageMagick PHP extension.';
  }

  return $report;
}

/**
 * Apply instance specific configuration to ImageMagick object.
 *
 * @param Imagick $im
 *   The ImageMgaick object. This object will be modified.
 * @param array $settings
 *   Associative array, key being the method name to invoke, with the value
 *   containing the comma seperated parameters.
 * @param stdObject|int $file
 *   Drupal file object or FID that is being operated on.
 *
 * @return Imagick
 *   The $im passed in if all settings have been applied, throws an exception
 *   if one or more encountered an error.
 *
 * @see http://php.net/manual/en/book.imagick.php
 */
function cf_imagemagick_action_settings($im, $settings, $file) {
  $file = is_object($file) ? $file : file_load($file);

  if (method_exists($im, 'getFilename')) {
    // Execute methods that must happen before a file is loaded.
    foreach($settings as $setting => $arguments) {
      $setting = substr($setting, 19);
      $args = explode(',', $arguments);
      // Deviate from drupal standards for the sake of readability.
      if ($setting == 'setresolution') { $im->setResolution($args[0], $args[1]); }
    }
    // Load the file into Imagick object.
    $im->readImage(drupal_realpath($file->uri));
    // Execute methods that must happen after a file is loaded.
    foreach($settings as $setting => $arguments) {
      // Settings are only active if they have a value.
      if (strlen($arguments)) {
        $args = explode(',', $arguments);
        $setting = substr($setting, 19);
        if ($setting == 'resizeimage') { $im->resizeImage($args[0], $args[1], Imagick::FILTER_LANCZOS, 1); }
        if ($setting == 'setcompressionquality') { $im->setCompressionQuality($args[0]); }
        if ($setting == 'appendimages') {
          // I don't actually know what resetIterator does.
          $im->resetIterator();
          $im = $im->appendImages($args[0]);
        }
      }
    }
  }
  else {
    throw new Exception('Parameter provided was not Imagick object.');
  }

  return $im;
}
