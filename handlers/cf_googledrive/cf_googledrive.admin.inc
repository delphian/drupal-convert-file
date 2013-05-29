<?php

/**
 * @file
 * Administrative functionality.
 *
 * Form constructor for config settings and submit handler.
 */

/**
 * Menu callback.
 *
 * Display configuration form.
 */
function cf_googledrive_form() {
  $client = NULL;

  if (($library = libraries_load('google-api-php-client')) && !empty($library['loaded'])) {
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
        '#markup' => "<a href='$authUrl'>Connect to my Google Drive!</a>",
      );
    }
  }
  else {
    drupal_set_message('Could not find the Google API PHP Client library. Please download https://code.google.com/p/google-api-php-client/ and install into libraries directory.', 'warning');
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
  $link = url($_GET['q'], array('absolute' => TRUE));
  $form['google_client_redirect'] = array(
    '#type' => 'textfield',
    '#title' => 'Google Client Redirect',
    '#default_value' => variable_get('cf_googledrive_client_redirect', FALSE),
    '#description' => 'Please set to: ' . $link,
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

  drupal_set_message(t('Settings have been updated.'));
}
