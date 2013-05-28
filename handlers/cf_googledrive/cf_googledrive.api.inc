<?php

/**
 * @file
 * Simplify google drive SDK to convert files.
 *
 * @see https://developers.google.com/drive/auth/web-server
 * @see https://code.google.com/p/google-api-php-client/wiki/OAuth2 Not supported yet!
 */

/**
 * Communicate with ConvertAPI.
 *
 * @param string $file_name
 *   Full path and filename of file to convert.
 * @param string &$message
 *   Filled with verbose error or success message.
 *
 * @return string|bool
 *   A blob string on success containing the converted file, FALSE on error.
 */
function cf_googledrive_api_call($file_name, &$message) {
  
}
