<?php

/**
 * @file
 * Interface with the databse. Recording and retrieving conversion records.
 */

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
function convertfile_db_insert($fid_backup, $fid_current, $uid) {
  $cid = FALSE;

  // Initializing $record array with values.
  $record = array(
    'fid_backup' => $fid_backup,
    'fid_current' => $fid_current,
    'uid' => $uid,
    'created' => time(),
  );
  // Inserting $record (data) into the database. Here $table variable contains the name of the table.
  if (drupal_write_record('convertfile', $record)) {
    // Getting the last inserted ID. (cid) is the primary key of the table.
    $cid = $record['cid'];
  }

  return $cid;
}
