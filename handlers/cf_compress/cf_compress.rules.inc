<?

/**
 * Implementation of hook_rules_event_info().
 * @ingroup rules
 */
function cf_compress_rules_event_info() {
  return array(
    'cf_compress_request' => array(
      'label' => t('Convert File Handler Compress has been requested.'),
      'module' => 'cf_compress',
      'arguments' => array(
        'file' => array('type' => 'file', 'label' => t('The file on which compression has been requested.')),
        'instance' => array('type' => 'stdObject', 'label' => t('Field instance that file is attached to.')),
      ),
    ),
  );
}
