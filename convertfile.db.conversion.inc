<?php

/**
 * @file
 * Interface with the databse. Recording and retrieving conversion records.
 */

define('CONVERTFILE_DB_CONVERSION', 'convertfile_conversion');

/**
 * Define the schema for the conversion table.
 *
 * Used during hook_install()
 */
function convertfile_db_conversion_schema() {
  $schema[CONVERTFILE_DB_CONVERSION] = array(
    'description' => 'Tracking original file uploads against their converted results.',
    'fields' => array(
      'cid' => array(
        'description' => 'The primary identifier for a conversion.',
        'type' => 'serial',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'fid_backup' => array(
        'description' => 'A previous version of the file.',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0
      ),
      'fid_current' => array(
        'description' => 'The current converted file.',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'default' => 0
      ),
      'created' => array(
        'description' => 'The Unix timestamp when the conversion was created.',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0
      ),
    ),
    'indexes' => array(
      'fid_current' => array('fid_current'),
    ),
    'primary key' => array('cid'),
  );

  return $schema;
}

/**
 * Write a new conversion record to the databse.
 *
 * @param int $fid_backup
 *   The FID of the original file.
 * @param int $fid_current
 *   The FID of the newly converted file.
 * @param int $uid
 *   User identification that owns these files.
 *
 * @return int|bool
 *   The inserted CID (unique record id) on success, FALSE on failure.
 */
function convertfile_db_convertion_insert($fid_backup, $fid_current, $uid) {
  $cid = FALSE;

  // Initializing $record array with values.
  $record = array(
    'fid_backup' => $fid_backup,
    'fid_current' => $fid_current,
    'uid' => $uid,
    'created' => time(),
  );
  // Inserting $record (data) into the database. Here $table variable contains the name of the table.
  if (drupal_write_record(CONVERTFILE_DB_CONVERSION, $record)) {
    // Getting the last inserted ID. (cid) is the primary key of the table.
    $cid = $record['cid'];
  }

  return $cid;
}

/**
 * Retrieve a convertfile record based on CID.
 *
 * @param int $cid
 *   The unique file conversion identifier.
 *
 * @return array|bool
 *   An assocative array containing:
 *   - 'fid_backup' : (int) The previous file before conversion.
 *   - 'fid_current' : (int) The file after conversion.
 *   - 'uid' : (int) The owner of both files.
 *   - 'created' : (string) When the conversion was done.
 *   Or FALSE on failure or non existing record identifier.
 */
function convertfile_db_conversion_select($cid) {
  $record = db_select(CONVERTFILE_DB_CONVERSION)
    ->condition('cid', $cid)
    ->execute()
    ->fetchAssoc();

  return $record;
}

/**
 * Delete a convertfile record based on CID.
 *
 * @param int $cid
 *   The unique file conversion identifier.
 *
 * @return bool
 *   TRUE on success, FALSE on failure.
 */
function convertfile_db_conversion_delete($cid) {
  $num_deleted = db_delete(CONVERTFILE_DB_CONVERSION)
    ->condition('cid', $cid)
    ->execute();
  
  return ($num_deleted) ? TRUE : FALSE;
}

/**
 * Determine if a backup already exists.
 *
 * @param int $backup_fid
 *   FID for the backup file.
 *
 * @return int
 *   0 if file is not used as a backup, FID for current file otherwise.
 */
function convertfile_db_conversion_is_backup($fid_backup) {
  $record = db_select(CONVERTFILE_DB_CONVERSION, 'c')
    ->fields('c')
    ->condition('fid_backup', $fid_backup)
    ->execute()
    ->fetchAssoc();

  return isset($record['fid_current']) ? $record['fid_current'] : 0;
}
