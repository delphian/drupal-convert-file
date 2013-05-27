<?php

/**
 * Implementation of hook_rules_event_info().
 */
function convertfile_rules_event_info() {
  return array(
    'convertfile_request' => array(
      'group' => 'Convert File',
      'label' => t('File conversion has been requested.'),
      'module' => 'convertfile',
      'variables' => array(
        'file' => array('type' => 'file', 'label' => t('The file on which compression has been requested.')),
        'instance' => array('type' => 'struct', 'label' => t('Field instance that file is attached to.')),
      ),
    ),
  );
}

/**
 * implementation of hook_rules_condition_info()
 */
function convertfile_rules_condition_info() {
  return array(
    'convertfile_condition_file_extension' => array(
      'label' => t('Original Format'),
      'group' => 'Convert File',
      'parameter' => array(
        'file' => array('type' => 'file', 'label' => t('The file object')),
        'extension' => array('type' => 'text', 'label' => t('The original extension is')),
      ),
      'module' => 'convertfile',
    ),
    'convertfile_condition_provider' => array(
      'label' => t('Conversion provider'),
      'group' => 'Convert File',
      'parameter' => array(
        'instance' => array('type' => 'struct', 'label' => t('File field instance')),
        'provider' => array(
          'type' => 'list<text>', 
          'label' => t('The provider is'),
          'options list' => 'convertfile_condition_provider_list',
        ),
      ),
      'module' => 'convertfile',
    ),
    'convertfile_condition_format' => array(
      'label' => t('Conversion format'),
      'group' => 'Convert File',
      'parameter' => array(
        'instance' => array('type' => 'struct', 'label' => t('File field instance')),
        'format' => array(
          'type' => 'list<text>', 
          'label' => t('The requested format is'),
          'options list' => 'convertfile_condition_format_list',
        ),
      ),
      'module' => 'convertfile',
    ),
  );
}

/**
 * Condition convertfile_condition_file_extension
 */
function convertfile_condition_file_extension($file, $extension) {
  $report = FALSE;
  $ext = pathinfo($file->filename, PATHINFO_EXTENSION);
  if ($ext == $extension) {
    $report = TRUE;
  }

  return $report;
}

/**
 * Condition convertfile_condition_provider
 */
function convertfile_condition_provider($instance, $provider) {
  $report = FALSE;
  $settings = $instance['widget']['settings'];
  if (array_key_exists($settings['convertfile_provider'], $provider)) {
    $report = TRUE;
  }

  return $report;
}

/**
 * Condition convertfile_condition_provider
 */
function convertfile_condition_format($instance, $format) {
  $report = FALSE;
  $settings = $instance['widget']['settings'];
  if (array_key_exists($settings['convertfile_format'], $format)) {
    $report = TRUE;
  }

  return $report;
}

/**
 * Construct options list for condition convertfile_condition_provider.
 *
 * @see convertfile_rules_condition_info()
 */
function convertfile_condition_provider_list(RulesAbstractPlugin $element) {
  $providers = array('convertfile_none' => '- None -');
  if (is_array($info = convertfile_collect_info())) {
    foreach($info as $machine_name => $provider) {
      $providers[$machine_name] = $provider['name'];
    }
  }

  return $providers;
}

/**
 * Construct options list for condition convertfile_condition_format.
 *
 * @see convertfile_rules_condition_info()
 */
function convertfile_condition_format_list(RulesAbstractPlugin $element) {
  $formats = array('convertfile_none' => '- None -');
  if (is_array($info = convertfile_collect_info())) {
    foreach($info as $mn_provider => $provider) {
      foreach($provider['types'] as $mn_type => $type) {
        $formats[$mn_type] = $type;
      }
    }
  }

  return $formats;
}
