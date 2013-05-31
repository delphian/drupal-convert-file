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
function cf_convertfile_form() {

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Convert File</h3>',
  );

  $markup = '<p>This provider will convert to the following formats:</p>';
  // Create list of formats.
  $markup .= '<ul>';
  $info = cf_convertfile_convertfile_info();
  foreach($info['cf_convertfile']['types'] as $key => $value) {
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
    '#description' => 'Enable help text which is inserted at variables pages.',
    '#default_value' => variable_get('cf_convertfile_help', FALSE),
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
function cf_convertfile_form_submit($form_id, &$form_state) {
  variable_set('cf_convertfile_help', $form_state['values']['enable_help']);

  drupal_set_message(t('Settings have been updated.'));
}
