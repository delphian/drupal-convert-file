<?php

/**
 * Implements hook_theme().
 */
function convertfile_theme() {
  return array(
    'convertfile_link' => array(
      'variables' => array('file' => NULL),
    ),
  );
}

/**
 * Returns HTML for a link to a file.
 *
 * @param $variables
 *   An associative array containing:
 *   - file: A file object to which the link will be created.
 *   - icon_directory: (optional) A path to a directory of icons to be used for
 *     files. Defaults to the value of the "file_icon_directory" variable.
 *
 * @return string
 *   HTML to display.
 *
 * @ingroup themeable
 */
function theme_convertfile_link($variables) {
  $file = $variables['file'];
  $icon_directory = $variables['icon_directory'];
  $html = '';

  $files = array($file);
  $fids = convertfile_db_conversion_get_backups($file->fid);
  foreach ($fids as $fid) {
    $files[] = file_load($fid);
  }

  foreach($files as $file) {
    $url = file_create_url($file->uri);
    $icon = theme('file_icon', array('file' => $file, 'icon_directory' => $icon_directory));
    // Set options as per anchor format described at
    // http://microformats.org/wiki/file-format-examples
    $options = array(
      'attributes' => array(
        'type' => $file->filemime . '; length=' . $file->filesize,
      ),
    );
    // Use the description as the link text if available.
    if (empty($file->description)) {
      $link_text = $file->filename;
    }
    else {
      $link_text = $file->description;
      $options['attributes']['title'] = check_plain($file->filename);
    }
    $html .= '<span class="file">' . $icon . ' ' . l($link_text, $url, $options) . '</span> ';
  }

  return $html;
}