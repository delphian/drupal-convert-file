<?php

/**
 * @file
 * REST API to convert files from ConvertAPI.com
 */

/**
 * @todo write description.
 */
function cf_convertapi_api_parse_header($header = '') {
  $resArr = array();
  $headerArr = explode("\n",$header);
  foreach ($headerArr as $key => $value) {
    $tmpArr = explode(": ",$value);
    if (count($tmpArr) < 1)
      continue;
    $resArr = array_merge($resArr, array($tmpArr[0] => count($tmpArr) < 2 ? "" : $tmpArr[1]));
  }
  return $resArr;
}

/**
 * Communicate with ConvertAPI.
 *
 * @param string $fileToConvert
 *   Load the file to convert into this blob.
 * @param string $apiKey
 *   API Key granting access to ConvertAPI.com
 * @param string &$message
 *   Filled with verbose error or success message.
 *
 * @return string|bool
 *   A blob string on success containing the converted file, FALSE on error.
 */
function cf_convertapi_api_call($fileToConvert, $apiKey, &$message) {
  try {
    $rand = time();
    $fileName = $rand."MyFile.pdf";
     
    $postdata = array(
      'OutputFileName' => 'MyFile.pdf',
      'ApiKey' => $apiKey,
      'file'=> "@" . $fileToConvert
    );
    //$ch = curl_init("http://do.convertapi.com/word2pdf");
    $ch = curl_init("http://www.google.com");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, 1);
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $postdata);
    $result = curl_exec($ch); 
    $headers = curl_getinfo($ch);
    
    $header = cf_convertapi_api_parse_header(substr($result,0,$headers["header_size"]));
    $body = substr($result, $headers["header_size"]);
    curl_close($ch);

    if (0 < $headers['http_code'] && $headers['http_code'] < 400) {
      // Check for Result = true
      if (in_array('Result', array_keys($header)) ? !$header['Result']=="True" : true) {
        $message = "Something went wrong with request, did not reach ConvertApi service.";
        return false;
      }
      // Check content type 
      if ($headers['content_type'] <> "application/pdf") {
        $message = "Exception Message : returned content is not PDF file.";
        return false;
      }

      $message = "The conversion was successful! The word file $fileToConvert converted to PDF and saved at $pathToSaveOutputFile$fileName";
      return $body;
    }
    else {
      $message = "Exception Message: " . $result . ". Headers: " . print_r($headers, TRUE);
      return false; 
    }
  }
  catch (Exception $e) {
    $message = "Exception Message: " . $e->getMessage();
    return false;
  }
}
