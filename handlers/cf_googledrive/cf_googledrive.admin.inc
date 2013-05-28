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

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Google Drive</h3>',
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

  drupal_set_message(t('Settings have been updated.'));
}
