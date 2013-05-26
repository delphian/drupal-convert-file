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
  // $form['gplus_api_key'] = array(
  //   '#prefix' => '<ul><li>' . t('Visit') . ' <a href="/admin/structure/block">blocks</a> ' . t('to position this block.') . '</li><li>' . t('Visit') . ' <a href="/admin/reports/dblog?type[]=convertfile">Recent log messages</a> ' . t('for any errors.') . '</li></ul>',
  //   '#type' => 'textfield',
  //   '#title' => t('Google API Key'),
  //   '#description' => t('Your google api key. This is the Simple API Access Key (for browser apps). Visit the Google') . ' <a href="https://code.google.com/apis/console#access">API Console</a> ' . t('for more information.'),
  //   '#default_value' => variable_get('convertfile_api_key', FALSE),
  // );
  // $form['gplus_profile_id'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('G+ User Profile Identification'),
  //   '#description' => t('The Gplus user profile ID to pull posts from. Viewing a public profile will expose this ID in the url.'),
  //   '#default_value' => variable_get('convertfile_profile_id', FALSE),
  // );
  // $form['gplus_max_results'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Max Results'),
  //   '#description' => t('Number of results to fetch and display.'),
  //   '#default_value' => variable_get('convertfile_max_results', 20),
  // );
  // $form['gplus_time_format'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Timestamp Format'),
  //   '#description' => 'See PHP\'s <a href="http://php.net/manual/en/function.date.php">date()</a> documentation.',
  //   '#default_value' => variable_get('convertfile_time_format', 'Y.m.d H:i'),
  // );

  // $form['gplus_container'] = array(
  //   '#type' => 'fieldset',
  //   '#title' => t('Advanced'),
  //   '#collapsible' => TRUE,
  //   '#collapsed' => TRUE,
  // );
  // $form['gplus_container']['gplus_cache_minutes'] = array(
  //   '#type' => 'textfield',
  //   '#title' => t('Cache Timeout'),
  //   '#description' => t('Number of minutes to cache the feed until another request is sent.'),
  //   '#default_value' => variable_get('convertfile_cache_minutes', 5),
  // );
  // $form['gplus_container']['gplus_record_api'] = array(
  //   '#type' => 'checkbox',
  //   '#title' => t('Record API Calls'),
  //   '#default_value' => variable_get('convertfile_record_api', 0),
  // );

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
  // variable_set('convertfile_api_key', $form_state['values']['gplus_api_key']);
  // variable_set('convertfile_cache_minutes', $form_state['values']['gplus_cache_minutes']);
  // variable_set('convertfile_profile_id', $form_state['values']['gplus_profile_id']);
  // variable_set('convertfile_time_format', $form_state['values']['gplus_time_format']);
  // variable_set('convertfile_max_results', $form_state['values']['gplus_max_results']);
  // variable_set('convertfile_record_api', $form_state['values']['gplus_record_api']);

  drupal_set_message(t('Settings have been updated.'));
}

/**
 * Menu callback.
 *
 * Display all handlers.
 */
function convertfile_handler_form() {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
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
