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
 * Generate configuration settings page.
 */
function convertfile_config_form() {

  $form['installed_on'] = array(
    '#type' => 'fieldset',
    '#title' => 'Widgets installed',
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE,
  );
  // Display all fields that are using the convertfile_file widget.
  if ($widgets = convertfile_find_widgets('convertfile_image')) {
    $markup = '<h5><u>Convert File</u> widget is installed on:</h5><ul>';
    foreach($widgets as $widget) {
      $markup .= "<li>{$widget}</li>";
    }
    $markup .= '</ul>';
  }
  else {
    $markup = '<h5>No <u>Convert File</u> widgets are installed</h5>';
  }
  $form['installed_on']['convertfile_file_installed'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  // Display all fields that are using the convertfile_file widget.
  if ($widgets = convertfile_find_widgets('convertfile_file')) {
    $markup = '<h5><u>Convert Image</u> widget is installed on:</h5><ul>';
    foreach($widgets as $widget) {
      $markup .= "<li>{$widget}</li>";
    }
    $markup .= '</ul>';
  }
  else {
    $markup = '<h5>No <u>Convert Image</u> widgets are installed</h5>';
  }
  $form['installed_on']['convertfile_image_installed'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  // Show all of the active rules.
  $conditions = array(
    'plugin' => 'reaction rule', 
    'active' => TRUE,
    'tags' => array('convertfile'),
  );
  $options = array('show plugin' => FALSE);
  $form['active'] = rules_ui()->overviewTable($conditions, $options);
  $form['active']['#caption'] = t('Active rules');
  $form['active']['#empty'] = t('There are no active rules. <a href="!url">Add new rule</a>.', array('!url' => url('admin/config/workflow/rules/reaction/add')));
  // Show all of the inactive rules.
  $conditions['active'] = FALSE;
  $form['inactive'] = rules_ui()->overviewTable($conditions, $options);
  $form['inactive']['#caption'] = t('Inactive rules');
  $form['inactive']['#empty'] = t('There are no inactive rules.');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * Submit handler for admin config form.
 *
 * @todo Consider: adding some validation?
 */
function convertfile_config_form_submit($form_id, &$form_state) {

  drupal_set_message(t('Settings have been updated.'));
}

/**
 * Menu callback.
 *
 * Display all handlers.
 */
function convertfile_handler_form() {
  // Display some helpful text if no handler's have been installed yet.
  if (!$info = convertfile_collect_info(TRUE)) {
    $form['hint'] = array(
      '#type' => 'markup',
      '#markup' => '<p>No file conversion handlers have been installed. You must ' .
        'install at least one handler to use this module. Convert File comes ' .
        'with a default handler to compress uploaded text files.</p>',
    );
    $form['enable'] = array(
      '#title' => 'Enable Compress',
      '#type' => 'checkbox',
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * Submit handler for convertfile_handler_form.
 *
 * @see convertfile_handler_form()
 */
function convertfile_handler_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  if (isset($values['enable']) && $values['enable']) {
    if (module_enable(array('cf_compress'), TRUE)) {
      menu_rebuild();
    } else {
      drupal_set_message('Could not enable compress.', 'error');
    }
  }

  drupal_set_message(t('Settings have been updated.'));
}

/**
 * Menu callback.
 *
 * Display all debug and error messages from watchdog for our module.
 */
function convertfile_watchdog_form() {
  $rows = array();
  $classes = array(
    WATCHDOG_DEBUG     => 'dblog-debug',
    WATCHDOG_INFO      => 'dblog-info',
    WATCHDOG_NOTICE    => 'dblog-notice',
    WATCHDOG_WARNING   => 'dblog-warning',
    WATCHDOG_ERROR     => 'dblog-error',
    WATCHDOG_CRITICAL  => 'dblog-critical',
    WATCHDOG_ALERT     => 'dblog-alert',
    WATCHDOG_EMERGENCY => 'dblog-emerg',
  );

  $header = array(
    '', // Icon column.
    array('data' => t('Type'), 'field' => 'w.type'),
    array('data' => t('Date'), 'field' => 'w.wid', 'sort' => 'desc'),
    t('Message'),
    array('data' => t('User'), 'field' => 'u.name'),
    array('data' => t('Operations')),
  );

  $query = db_select('watchdog', 'w')->extend('PagerDefault')->extend('TableSort');
  $query->leftJoin('users', 'u', 'w.uid = u.uid');
  $query
    ->fields('w', array('wid', 'uid', 'severity', 'type', 'timestamp', 'message', 'variables', 'link'))
    ->addField('u', 'name');
  $query->where('(w.type = :type)', array(':type' => 'convertfile'));

  $result = $query
    ->limit(50)
    ->orderByHeader($header)
    ->execute();

  foreach ($result as $dblog) {
    $rows[] = array('data' =>
      array(
        // Cells
        array('class' => 'icon'),
        t($dblog->type),
        format_date($dblog->timestamp, 'short'),
        theme('dblog_message', array('event' => $dblog, 'link' => TRUE)),
        theme('username', array('account' => $dblog)),
        filter_xss($dblog->link),
      ),
      // Attributes for tr
      'class' => array(drupal_html_class('dblog-' . $dblog->type), $classes[$dblog->severity]),
    );
  }

  $build['dblog_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'admin-dblog'),
    '#empty' => t('No log messages available.'),
  );
  $build['dblog_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Search out and find all fields that use our widgets.
 */
function convertfile_find_widgets($widget, $reset = FALSE) {
  if (!$reset && ($cache = cache_get('convertfile_find_widget_' . $widget, 'cache'))) {
    $data = $cache->data;
  }
  else {
    $data = array();
    $instances = field_info_instances();
    foreach($instances as $entity_key => $entity) {
      foreach($entity as $bundle_key => $bundle) {
        foreach($bundle as $field_key => $field) {
          $type = $field['widget']['type'];
          if ($type == $widget) {
            $data[] = "{$entity_key} : {$bundle_key} : {$field_key}";
          }
        }
      }
    }
    cache_set('convertfile_find_widgets', $data, 'cache');
  }

  return $data;
}
