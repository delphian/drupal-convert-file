<?php

/**
 * @file
 * Bare bones communication with the Google+ REST API.
 *
 * Centralized function to handle all curl requests. Two main functions
 * provide profile and the feed itself.
 */

/**
 * Send an api reqeust to google plus api.
 *
 * @param string $request
 *   The request to append to the api url.
 * @param array $options
 *   (optional) Key value pair to append as parameters to the url.
 *
 * @return string
 *   Result of the curl request. String should contain JSON object.
 */
function gplus_feed_block_api_get($request, $options = array()) {
  $api_key = variable_get('gplus_feed_block_api_key', FALSE);
  $url = "https://www.googleapis.com/plus/v1{$request}?key={$api_key}";
  // Append name value pairs.
  foreach ($options as $key => $value) {
    $url .= "&{$key}={$value}";
  }

  if (variable_get('gplus_feed_block_record_api', 0)) {
    watchdog('gplus_feed_block', 'API Request: %url', array('%url' => $url), WATCHDOG_INFO);
  }

  $ch = curl_init();

  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_USERAGENT, "DrupalGPlusFeedBlock/1.0");
  curl_setopt($ch, CURLOPT_HEADER, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_TIMEOUT, 10);

  $result = curl_exec($ch);
  curl_close($ch);

  if (variable_get('gplus_feed_block_record_api', 0)) {
    watchdog('gplus_feed_block', 'API Response: %result', array('%result' => $result), WATCHDOG_INFO);
  }

  return $result;
}

/**
 * Retrieve activities object from google plus api.
 *
 * Defaults to cached version, if available, when communications is down.
 *
 * @param string $profile_id
 *   Unique google plus profile identifier.
 * @param bool $reset
 *   (optional) If set to TRUE then bypass any cache that may exist.
 *
 * @return object|bool
 *   JSON object or FALSE on error.
 *
 * @see https://developers.google.com/+/api/latest/activities
 */
function gplus_feed_block_api_get_activities($profile_id, $reset = FALSE) {
  $activities = FALSE;
  if ($cache = cache_get('gplus_feed_block_activities_' . $profile_id, 'cache_block')) {
    $activities = $cache->data;
  }

  if (!$cache || (time() > $cache->expire) || $reset) {
    $request = "/people/{$profile_id}/activities/public/";
    $result = gplus_feed_block_api_get($request, array('maxResults' => variable_get('gplus_feed_block_max_results', 20)));
    if ($result) {
      $result = json_decode($result);
    }

    if (is_object($result) && !isset($result->error)) {
      $cache_expire = (variable_get('gplus_feed_block_cache_minutes', 5) * 60) + time();
      cache_set('gplus_feed_block_activities_' . $profile_id, $result, 'cache_block', $cache_expire);
      $activities = $result;
    }
    else {
      if (isset($result->error)) {
        watchdog('gplus_feed_block', 'Error, Google says: %reason', array('%reason' => $result->error->errors[0]->reason), WATCHDOG_ERROR);
      }
      else {
        watchdog('gplus_feed_block', 'Failed to retrieve JSON object from REST API (communication failure?).', NULL, WATCHDOG_ERROR);
      }
      $activities = FALSE;
    }
  }

  return $activities;
}

/**
 * Retrieve profile object from google plus api.
 *
 * Defaults to cached version, if available, when communications is down.
 *
 * @param string $profile_id
 *   Unique google plus profile identifier.
 * @param bool $reset
 *   (optional) If set to TRUE then bypass any cache that may exist.
 *
 * @return object|bool
 *   JSON object or FALSE on error.
 *
 * @see https://developers.google.com/+/api/latest/people
 */
function gplus_feed_block_api_get_profile($profile_id, $reset = FALSE) {
  $profile = FALSE;
  if ($cache = cache_get('gplus_feed_block_profile_' . $profile_id, 'cache_block')) {
    $profile = $cache->data;
  }

  if (!$cache || (time() > $cache->expire) || $reset) {
    $request = "/people/{$profile_id}";
    $result = gplus_feed_block_api_get($request);
    if ($result) {
      $result = json_decode($result);
    }

    if (is_object($result) && !isset($result->error)) {
      $cache_expire = (variable_get('gplus_feed_block_cache_minutes', 5) * 60) + time();
      cache_set('gplus_feed_block_profile_' . $profile_id, $result, 'cache_block', $cache_expire);
      $profile = $result;
    }
    else {
      if (isset($result->error)) {
        watchdog('gplus_feed_block', 'Error, Google says: %reason', array('%reason' => $result->error->errors[0]->reason), WATCHDOG_ERROR);
      }
      else {
        watchdog('gplus_feed_block', 'Failed to retrieve JSON object from REST API (communication failure?).', NULL, WATCHDOG_ERROR);
      }
      $profile = FALSE;
    }
  }

  return $profile;
}
