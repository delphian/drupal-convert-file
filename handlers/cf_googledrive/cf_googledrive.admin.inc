<?php

/**
 * @file
 * Administrative functionality.
 *
 * Form constructor for config settings and submit handler.
 */

// @todo move to library_load
require_once '/projects/activelamp/convertfile/drupal-7.22/sites/all/libraries/google-api-php-client/src/Google_Client.php';
require_once '/projects/activelamp/convertfile/drupal-7.22/sites/all/libraries/google-api-php-client/src/contrib/Google_DriveService.php';

/**
 * Menu callback.
 *
 * Display configuration form.
 */
function cf_googledrive_form() {

  module_load_include('inc', 'cf_googledrive', 'cf_googledrive.api');

  // Set an initial access token.
  if (isset($_GET['code'])) {
    $client = new Google_Client();
    $credentials = $client->authenticate();
    variable_set('cf_googledrive_credentials', $credentials);
  }
  $service = cf_googledrive_service(cf_googledrive_client());

  if (!$service) {
    $client = cf_googledrive_client();
    $authUrl = $client->createAuthUrl();
    $form['connect_google_drive'] = array(
      '#type' => 'markup',
      '#markup' => "<a href='$authUrl'>Connect Me!</a>",
    );
  }

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => ($service) ? '<h3>Google Drive (<span style="color:#00dd00">Connected</span>)</h3>' : '<h3>Google Drive (<span style="color:#dd0000;">Disconnected</span>)</h3>',
  );

  $markup = '<p>This handler will convert to the following formats:</p>';
  // Create list of formats.
  $markup .= '<ul>';
  $info = cf_googledrive_convertfile_info();
  foreach($info['cf_googledrive']['types'] as $key => $value) {
    $markup .= "<li>{$value}</li>";
  }
  $markup .= '</ul><br>';
  $form['description'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['enable_help'] = array(
    '#type' => 'checkbox',
    '#title' => 'Help text',
    '#description' => 'Enable help text which is inserted on various pages.',
    '#default_value' => variable_get('cf_googledrive_help', FALSE),
  );

  $form['google_client_id'] = array(
    '#type' => 'textfield',
    '#title' => 'Google Client ID',
    '#default_value' => variable_get('cf_googledrive_client_id', FALSE),
  );
  $form['google_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => 'Google Client Secret',
    '#default_value' => variable_get('cf_googledrive_client_secret', FALSE),
  );
  $form['google_client_redirect'] = array(
    '#type' => 'textfield',
    '#title' => 'Google Client Redirect',
    '#default_value' => variable_get('cf_googledrive_client_redirect', FALSE),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * Submit handler for config form.
 */
function cf_googledrive_form_submit($form_id, &$form_state) {
  $values = $form_state['values'];
  variable_set('cf_googledrive_help', $values['enable_help']);
  variable_set('cf_googledrive_client_id', $values['google_client_id']);
  variable_set('cf_googledrive_client_secret', $values['google_client_secret']);
  variable_set('cf_googledrive_client_redirect', $values['google_client_redirect']);

  $service = cf_googledrive_service(cf_googledrive_client());
  if ($service) {
    $title = 'testingthisout';
    $description = 'just a test file description here.';
    $parentId = NULL;
    $mimeType = 'text/plain';
    $filename = '/Users/bryanhazelbaker/Downloads/test.txt';
    if (cf_googledrive_insert_file($service, $title, $description, $parentId, $mimeType, $filename)) {
      drupal_set_message('Test file OK!');
    }
    drupal_set_message('Google drive is connected', 'status');
  }
  else {
    drupal_set_message('Google Drive is not connected', 'warning');
  }

  drupal_set_message(t('Settings have been updated.'));
}
