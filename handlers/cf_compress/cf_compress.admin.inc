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
 * Display all handlers.
 */
function cf_compress_form() {

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Compress</h3>',
  );
  $form['enable'] = array(
    '#type' => 'checkbox',
    '#title' => 'Enable handler.',
    '#default_value' => variable_get('cf_compress_enable', TRUE),
  );

  $markup = '<p>This handler will convert to the following formats:</p>';
  // Create list of formats.
  $markup .= '<ul>';
  $info = cf_compress_convertfile_info();
  foreach($info['compress']['types'] as $key => $value) {
    $markup .= "<li>{$value}</li>";
  }
  $markup .= '</ul><br>';
  $form['description'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * Submit handler for config form.
 *
 * @todo Consider: adding some validation?
 */
function cf_compress_form_submit($form_id, &$form_state) {
  variable_set('cf_compress_enable', $form_state['values']['enable']);

  drupal_set_message(t('Settings have been updated.'));
}
