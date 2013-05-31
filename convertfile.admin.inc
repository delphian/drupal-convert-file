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
 *
 * @see rules_admin_reaction_overview()
 */
function convertfile_config_form() {

  $form['test'] = array(
    '#type' => 'convertfile_file',
    '#title' => 'Test Upload',
    '#stuff' => '#stuff',
  );

  $form['installed_on'] = array(
    '#type' => 'fieldset',
    '#title' => 'Widgets assigned to fields',
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE,
  );
  // Display all fields that are using the convertfile_file widget.
  if ($widgets = convertfile_find_widgets('convertfile_file')) {
    $markup = '<div><u>Convert File</u> widget is assigned on:</div><ul>';
    foreach($widgets as $widget) {
      $markup .= "<li>{$widget}</li>";
    }
    $markup .= '</ul>';
  }
  else {
    $markup = '<div>No <u>Convert File</u> widgets are assigned</div>';
  }
  $form['installed_on']['convertfile_file_installed'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  // Display all fields that are using the convertfile_file widget.
  if ($widgets = convertfile_find_widgets('convertfile_image')) {
    $markup = '<div><u>Convert Image</u> widget is assigned on:</div><ul>';
    foreach($widgets as $widget) {
      $markup .= "<li>{$widget}</li>";
    }
    $markup .= '</ul>';
  }
  else {
    $markup = '<div>No <u>Convert Image</u> widgets are assigned</div>';
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
  $base_path = 'admin/config/workflow/rules/reaction';
  $options = array('show plugin' => FALSE, 'base path' => $base_path);
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
function convertfile_config_form_submit($form, &$form_state) {

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
        'with a default handler.</p>',
    );
    $form['enable'] = array(
      '#title' => 'Enable Convert File default handler',
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
    if (module_enable(array('cf_convertfile'), TRUE)) {
      menu_rebuild();
    } else {
      drupal_set_message('Could not enable cf_convertfile.', 'error');
    }
  }

  drupal_set_message(t('Settings have been updated.'));
}

/**
 * Menu callback.
 *
 * Display all debug and error messages from watchdog for our module.
 *
 * @see dblog_overview()
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

  // Add css.
  drupal_add_css(
    drupal_get_path('module', 'dblog') . '/dblog.css', 
    array('group' => CSS_DEFAULT, 'every_page' => FALSE)
  );

  return $build;
}
