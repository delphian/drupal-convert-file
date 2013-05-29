<?php

/**
 * @file
 * Simplify google drive SDK to convert files.
 *
 * You need to download the google drive sdk first and make sure its included.
 * Then setup your api access with google (create a google application). After
 * that you will need to grant access to your personal Google Drive space to
 * your new google application. To create the url to do this use 
 * $client->createAuthUrl(), and follow that link. (google must have access
 * to redirect you back to your php application)
 *
 * @code
 * if ($service = cf_googledrive_service(cf_googledrive_client())) {
 *   if ($g_file = cf_googledrive_insert_file($service, $title, $description, NULL, $mime_type, $path_to_file)) {
 *     if ($data = cf_googledrive_download_file($service, $g_file, 'application/pdf')) {
 *       // $data is your blob, write it to the disk!
 *       if ($fp = fopen('newfile.pdf', 'wbx')) {
 *         fwrite($fp, $data);
 *       }
 *     }
 *   }
 * }
 * else {
 *   // Set an initial access token.
 *   if (isset($_GET['code'])) {
 *     $client = new Google_Client();
 *     $credentials = $client->authenticate();
 *     variable_set('cf_googledrive_credentials', $credentials);
 *   }
 *   else {
 *     $client = cf_googledrive_client();
 *     $authUrl = $client->createAuthUrl();
 *     drupal_set_message("<a href='$authUrl'>Connect to my Google Drive!</a>", 'status');
 *   }
 * }
 * @endcode
 *
 * @see https://code.google.com/p/google-api-php-client/
 * @see https://developers.google.com/drive/auth/web-server
 * @see https://www.google.com/apis/console
 */

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
    $client->setUseObjects(true);
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

/**
 * Download a file's content.
 *
 * @param Google_DriveService $service Drive API service instance.
 * @param File $file Drive File instance.
 * @return String The file's content if successful, null otherwise.
 */
function cf_googledrive_download_file($service, $file, $type) {
  $download_links = $file->getExportLinks();
  $downloadUrl = $download_links[$type];
  if ($downloadUrl) {
    $request = new Google_HttpRequest($downloadUrl, 'GET', null, null);
    $httpRequest = Google_Client::$io->authenticatedRequest($request);
    if ($httpRequest->getResponseHttpCode() == 200) {
      return $httpRequest->getResponseBody();
    } else {
      // An error occurred.
      return null;
    }
  } else {
    // The file doesn't have any content stored on Drive.
    return null;
  }
}

/**
 * Permanently delete a file, skipping the trash.
 *
 * @param Google_DriveService $service Drive API service instance.
 * @param String $fileId ID of the file to delete.
 */
function cf_googledrive_delete_file($service, $fileId) {
  $success = FALSE;
  try {
    $service->files->delete($fileId);
    $success = TRUE;
  } catch (Exception $e) {
    // Don't do anything.
  }
  return $success;
}
