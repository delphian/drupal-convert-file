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
  $service = NULL;

  if (($library = libraries_load('google-api-php-client')) && !empty($library['loaded'])) {
    module_load_include('inc', 'cf_googledrive', 'cf_googledrive.api');
    // Set an initial access token.
    if (isset($_GET['code'])) {
      if ($client = cf_googledrive_client()) {
        $credentials = $client->authenticate();
        variable_set('cf_googledrive_credentials', $credentials);
        // Redirect to get rid of the code.
        drupal_goto(url($_GET['q'], array('absolute' => TRUE)));
      }
    }
    $service = cf_googledrive_service(cf_googledrive_client());
  }
  else {
    drupal_set_message('Could not find the Google API PHP Client library. Please download https://code.google.com/p/google-api-php-client/ and install into libraries directory.', 'warning');
  }

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => ($service) ? '<h3>Google Drive (<span style="color:#00dd00">Connected</span>)</h3>' : '<h3>Google Drive (<span style="color:#dd0000;">Disconnected</span>)</h3>',
  );
  if ($service) {
    // Disconnect the google drive from drupal.
    $form['disconnect'] = array(
      '#type' => 'button',
      '#button_type' => 'submit',
      '#executes_submit_callback' => TRUE,
      '#value' => 'Disconnect Google Drive',
    );
  }
  else {
    // Connect google drive to application.
    if (!empty($library['loaded']) && ($client = cf_googledrive_client())) {
      $authUrl = $client->createAuthUrl();
      $form['connect_google_drive'] = array(
        '#type' => 'button',
        '#button_type' => 'submit',
        '#executes_submit_callback' => TRUE,
        '#value' => 'Connect Google Drive',
        '#markup' => "<p><a href='$authUrl'>Connect to my Google Drive!</a></p>",
      );
    }
  }

  $markup = '<p>This provider will convert to the following formats:</p>';
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
    '#default_value' => variable_get('cf_googledrive_client_redirect', $link),
    '#description' => 'Recommended value: ' . $link,
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
  // Disconnect the google drive.
  if ($values['op'] == 'Disconnect Google Drive') {
    variable_set('cf_googledrive_credentials', NULL);
    drupal_set_message(t('Google Drive has been disconnected from drupal. Dont forget to revoke access of the application to your drive: <a href="https://support.google.com/accounts/answer/41236">https://support.google.com/accounts/answer/41236</a>'));
  }
  // Redirect to google drive authentication.
  elseif ($values['op'] == 'Connect Google Drive') {
    if ($client = cf_googledrive_client()) {
      $authUrl = $client->createAuthUrl();
      watchdog('cf_googledrive', "Redirecting admin to authorize google drive at: {$authUrl}", NULL, WATCHDOG_DEBUG);
      drupal_goto($authUrl, array('external' => TRUE));
    }
    else {
      drupal_set_message('Unable to instantiate google client. This is normally due to an invalid or missing client id.', 'error');
    }
  }
  else {
    variable_set('cf_googledrive_help', $values['enable_help']);
    variable_set('cf_googledrive_client_id', $values['google_client_id']);
    variable_set('cf_googledrive_client_secret', $values['google_client_secret']);
    variable_set('cf_googledrive_client_redirect', $values['google_client_redirect']);
    drupal_set_message(t('Settings have been updated.'));
  }
}
