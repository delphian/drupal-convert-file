<?php

/**
 * @file
 * Interface with the databse. Recording and retrieving conversion records.
 */

/**
 * Write a new conversion record to the databse.
 *
 * @param int $fid_original
 *   The FID of the original uploaded file.
 * @param int $fid_converted
 *   The FID of the newly converted file.
 *
 * @return int|bool
 *   The inserted CID (unique record id) on success, FALSE on failure.
 */
function convertfile_db_insert($fid_original, $fid_converted) {
  $cid = FALSE;

  // Initializing $record array with values.
  $record = array(
    'fid_original' => $fid_original,
    'fid_converted' => $fid_converted,
    'created' => time(),
    'updated' => time(),
  );
  // Inserting $record (data) into the database. Here $table variable contains the name of the table.
  if (drupal_write_record('convertfile', $record)) {
    // Getting the last inserted ID. (cid) is the primary key of the table.
    $cid = $record['cid'];
  }

  return $cid;
}
