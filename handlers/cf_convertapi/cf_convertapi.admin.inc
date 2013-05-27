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
function cf_convertapi_form() {

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>ConvertAPI.com</h3>',
  );

  $markup = '<p>This handler will convert to the following formats:</p>';
  // Create list of formats.
  $markup .= '<ul>';
  $info = cf_convertapi_convertfile_info();
  foreach($info['cf_convertapi']['types'] as $key => $value) {
    $markup .= "<li>{$value}</li>";
  }
  $markup .= '</ul><br>';
  $form['description'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['api_key'] = array(
    '#type' => 'textfield',
    '#title' => 'API Key',
    '#description' => 'The API Key received after registering at convertapi.com',
    '#default_value' => variable_get('cf_convertapi_api_key', NULL),
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
function cf_convertapi_form_submit($form_id, &$form_state) {
  variable_set('cf_convertapi_api_key', $form_state['values']['api_key']);

  drupal_set_message(t('Settings have been updated.'));
}
