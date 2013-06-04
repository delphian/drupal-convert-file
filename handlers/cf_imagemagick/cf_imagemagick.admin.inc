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
function cf_imagemagick_form() {

  if (!class_exists('Imagick')) {
    drupal_set_message('Could not detect ImageMagick PHP extension.', 'warning');
  }

  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>ImageMagick</h3>',
  );

  $markup = '<p>This handler will convert to the following formats:</p>';
  // Create list of formats.
  $markup .= '<ul>';
  $info = cf_imagemagick_convertfile_info();
  foreach($info['cf_imagemagick']['types'] as $key => $value) {
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
    '#default_value' => variable_get('cf_imagemagick_help', FALSE),
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
function cf_imagemagick_form_submit($form_id, &$form_state) {
  $values = $form_state['values'];

  variable_set('cf_imagemagick_help', $values['enable_help']);
  drupal_set_message(t('Settings have been updated.'));
}
