<?php

/**
 * @file
 * Simplify google drive SDK to convert files.
 *
 * @see https://developers.google.com/drive/auth/web-server
 * @see https://code.google.com/p/google-api-php-client/
 * @see https://code.google.com/p/google-api-php-client/wiki/OAuth2 Not supported yet!
 */

require_once '/projects/activelamp/convertfile/drupal-7.22/sites/all/libraries/google-api-php-client/src/Google_Client.php';
require_once '/projects/activelamp/convertfile/drupal-7.22/sites/all/libraries/google-api-php-client/src/contrib/Google_Oauth2Service.php';

/**
 * Load client.
 */
function cf_googledrive_client() {
  $client = new Google_Client();
  $client->setApplicationName('Drupal Convert File Google Drive Handler');
  $client->setClientId(variable_get('cf_googledrive_client_id', NULL));
  $client->setClientSecret(variable_get('cf_googledrive_client_secret', NULL));
  $client->setRedirectUri(variable_get('cf_googledrive_client_redirect', NULL));
  $client->setScopes(array(
    'https://www.googleapis.com/auth/drive.file',
    'https://www.googleapis.com/auth/userinfo.email',
    'https://www.googleapis.com/auth/userinfo.profile',
  ));
  return $client;
}

/**
 * Load the drive service.
 *
 * @param Google_Client $client
 *   A client created with cf_googledrive_client()
 *
 * @return Google_DriveService|bool
 *   A Google_DriveService on success ready for reading and writing, or FALSE
 *   on failure.
 *
 * @see cf_googledrive_client()
 */
function cf_googledrive_service($client) {
  $app = NULL;

  $credentials = variable_get('cf_googledrive_credentials', FALSE);
  if ($client && $credentials) {
    $client->setAccessToken($credentials);
    $app = new Google_DriveService($client);
    try {
      $app->files->listFiles();
    } catch (exception $e) {
      $app = NULL;
    }
  }

  return $app;
}

/**
 * Insert new file.
 *
 * @param Google_DriveService $service Drive API service instance.
 * @param string $title Title of the file to insert, including the extension.
 * @param string $description Description of the file to insert.
 * @param string $parentId Parent folder's ID.
 * @param string $mimeType MIME type of the file to insert.
 * @param string $filename Filename of the file to insert.
 * @return Google_DriveFile The file that was inserted. NULL is returned if an API error occurred.
 */
function cf_googledrive_insert_file($service, $title, $description, $parentId, $mimeType, $filename) {
  $file = new Google_DriveFile();
  $file->setTitle($title);
  $file->setDescription($description);
  $file->setMimeType($mimeType);

  // Set the parent folder.
  if ($parentId != null) {
    $parent = new ParentReference();
    $parent->setId($parentId);
    $file->setParents(array($parent));
  }

  try {
    $data = file_get_contents($filename);

    $createdFile = $service->files->insert($file, array(
      'convert' => TRUE,
      'data' => $data,
      'mimeType' => $mimeType,
    ));

    // Uncomment the following line to print the File ID
    // print 'File ID: %s' % $createdFile->getId();

    return $createdFile;
  } catch (Exception $e) {
    print "An error occurred: " . $e->getMessage();
  }
}