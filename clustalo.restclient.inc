<?php
/*
 * Main function to do multiple sequences alignment
 * by sending to clustal omega (REST).
 */
function clustalo_submit_job($parameters, $phylo) {
  // Base URL for service
  $baseUrl = 'http://www.ebi.ac.uk/Tools/services/rest/clustalo';

  $checkInterval = 10;  // Set interval for checking status
  $outputLevel = 1;  // Output Level
  $debugLevel = 0;  // Debug Level

  // Process command-line options
  $paramOpt = array(
    // Tool specific options
    'guidetreeout',
    'noguidetreeout',
    'dismatout',
    'nodismatout',
    'dealign',
    'nodealign',
    'mbed',
    'nombed',
    'mbediteration',
    'nombediteration',
    'iterations',
    'gtiterations',
    'hmmiterations',
    'outfmt',
    'stype',
    'sequence',

    // General options
    'email',
    'title',
    'outfile',
    'outformat',
    'jobid',
    'async',
    'polljob',
    'resultTypes',
    'status',
    'params',
    'paramDetail',
    'quiet',
    'verbose',
    'debugLevel',
    'baseUrl' => $baseUrl,
    );

  $options = getopt(NULL, $paramOpt);
  $numOpts = count($options);  // Number of option arguments.
  
  // Increase output level
  if ($options['verbose']) {
    $outputLevel++;
  }
  // Decrease output level
  elseif ($options['quiet']) {
    $outputLevel--;
  }
  // Debug level
  if ($options['debugLevel']) {
    $debugLevel = $options['debugLevel'];
  }

  // default values for submit job
  $options['guidetreeout'] = array_key_exists('guidetreeout', $parameters) ? $parameters['guidetreeout'] : false;
  $options['noguidetreeout'] = array_key_exists('noguidetreeout', $parameters) ? $parameters['guidetreeout'] : true;
  $options['dismatout'] = array_key_exists('dismatout', $parameters) ? $parameters['dismatout'] : false;
  $options['nodismatout'] = array_key_exists('nodismatout', $parameters) ? $parameters['nodismatout'] : true;
  $options['dealign'] = array_key_exists('dealign', $parameters) ? $parameters['dealign'] : false;
  $options['nodealign'] = array_key_exists('nodealign', $parameters) ? $parameters['nodealign'] : true;
  $options['mbed'] = array_key_exists('mbed', $parameters) ? $parameters['mbed'] : false;
  $options['nombed'] = array_key_exists('nombed', $parameters) ? $parameters['nombed'] : true;
  $options['mbediteration'] = array_key_exists('mbediteration', $parameters) ? $parameters['mbediteration'] : false;
  $options['nombediteration'] = array_key_exists('nombediteration', $parameters) ? $parameters['nombediteration'] : true;
  $options['iterations'] = array_key_exists('iterations', $parameters) ? $parameters['iterations'] : 0;
  $options['gtiterations'] = array_key_exists('gtiterations', $parameters) ? $parameters['gtiterations'] : 0;
  $options['hmmiterations'] = array_key_exists('hmmiterations', $parameters) ? $parameters['hmmiterations'] : 0;
  $options['outfmt'] = array_key_exists('outfmt', $parameters) ? $parameters['outfmt'] : '';
  $options['stype'] = array_key_exists('stype', $parameters) ? $parameters['stype'] : '';
  $options['sequence'] = array_key_exists('sequence', $parameters) ? $parameters['sequence'] : '';

  $options['email'] = array_key_exists('email', $parameters) ? $parameters['email'] : '';
  $options['title'] = array_key_exists('title', $parameters) ? $parameters['title'] : '';
  $options['outfile'] = array_key_exists('outfile', $parameters) ? $parameters['outfile'] : '';
  $options['outformat'] = array_key_exists('outformat', $parameters) ? $parameters['outformat'] : '';
  $options['jobid'] = array_key_exists('jobid', $parameters) ? $parameters['jobid'] : '';

  // Submit job
  if (($options['email'] != '') && ($options['jobid']=='') && ($options['sequence'] != '')) {
    $params = array();
    $params['email'] = $options['email'];
    $params['guidetreeout'] = $options['guidetreeout'];
    $params['dismatout'] = $options['dismatout'];
    $params['dealign'] = $options['dealign'];
    $params['mbed'] = $options['mbed'];
    $params['mbediteration'] = $options['mbediteration'];
    $params['sequence'] = $options['sequence'];

    if ($options['stype'] != '') {
      $params['stype'] = $options['stype'];
    }
    if ($options['iterations'] != 0) {
      $params['iterations'] = $options['iterations'];
    }
    if ($options['gtiterations'] != 0) {
      $params['gtiterations'] = $options['gtiterations'];
    }
    if ($options['hmmiterations'] != 0) {
      $params['hmmiterations'] = $options['hmmiterations'];
    }
    if ($options['outfmt'] != '') {
      $params['outfmt'] = $options['outfmt'];
    }
    $jobId = _serviceRun($baseUrl, $params);
    sleep(5);
    $result = clustalo_get_result($jobId, $checkInterval, $baseUrl, $phylo);
  }

  // Get job status
  elseif (($options['status']!=NULL) && ($options['jobid']!='')) {
    _printGetStatus($options['jobid']);
  }
  // List result types for job
  elseif (($options['resultTypes']!=NULL) && ($options['jobid']!='')) {
    _printGetResultTypes($options['resultTypes']);
  }
  // Get results for job
  elseif (($options['polljob']!=NULL) && ($options['jobid'])!='') {
    clustero_get_result($options['jobid'], $checkInterval, $baseUrl, $phylo);
  }
  else {
    drupal_set_message(t('Error: unrecognised argument combinations'), 'error');
  }
  return $result;
}

/*
 * Submit job
 */
function _serviceRun($baseUrl, $options) {
  $requestUrl = $baseUrl."/run/";
  // Make the submission (HTTP POST).
  $postdata = http_build_query($options);
  $option = array('http' => array(
                 'method' => 'POST',
                 'header' => 'Content-type: application/x-www-form-urlencoded',
                 'content' => $postdata
               )
             );
  $context = stream_context_create($option);
  $jobId = file_get_contents($requestUrl, false, $context);
  drupal_set_message(t('[_serviceRun] jobId: '.$jobId), 'status');
  drupal_set_message(t('[_serviceRun] End'), 'status');
  return $jobId;
}

/*
 * Get result for a jobid
 */
function clustalo_get_result($jobId, $checkInterval, $baseUrl, $phylo) {
  // Check status and wait if necessary
  _clientPoll($jobId, $checkInterval, $baseUrl);

  // Get available result types
  $resultTypes = _serviceGetResultTypes($jobId, $baseUrl);
  $xml = simplexml_load_string($resultTypes);
  $i = 0;
  $data = '';
  if ($phylo == false) {
    $filename = $jobId.".txt"; // filename for the result
    while ($i < count($xml->type)) {
      $type = array();
      $type = $xml->type[$i];

      // Get the result.
      if ($type->identifier == 'aln-clustal') {
        $result = _serviceGetResult($jobId, $type->identifier, $baseUrl);
        $data .= "Clustal Omega Job ID: ".$jobId."\n\n";
        $data .= $result."\n";
      }
      $i++;
    }
  }
  else {
    while ($i < count($xml->type)) {
      $type = array();
      $type = $xml->type[$i];

      // Get the phylogenetic tree result.
      if ($type->identifier == 'phylotree') {
        $result = _serviceGetResult($jobId, $type->identifier, $baseUrl);
        $data .= $result."\n";
      }
      $i++;
    }
  }
  return $data;
}

/*
 * Client-side poll
 */
function _clientPoll($jobId, $checkInterval, $baseUrl) {
  $result = 'PENDING';
  while (($result == 'RUNNING') || ($result == 'PENDING')) {
    $result = _serviceGetStatus($jobId, $baseUrl);
    //file_put_contents($stderr, $result);
    if (($result == 'RUNNING') or ($result == 'PENDING')) {
      sleep($checkInterval);
    }
  }
}

/*
 * Get job status
 */
function _serviceGetStatus($jobId, $baseUrl) {
  $requestUrl = $baseUrl."/status/".$jobId;
  $status = clustalo_rest_request($requestUrl);
  return $status;
}

/*
 * Print the status of a job
 *
function _printGetStatus($jobId){
  drupal_set_message(t('[_serviceGetStatus] Begin'), 'status');
  $status = _serviceGetStatus($jobId);
  print $status;
  drupal_set_message(t('[_serviceGetStatus] End'), 'status');
}*/

/*
 * Get available result types for job
 */
function _serviceGetResultTypes($jobId, $baseUrl){
  $requestUrl = $baseUrl."/resulttypes/".$jobId;
  $resultTypes = clustalo_rest_request($requestUrl);
  return $resultTypes;
}

/*
 * Print list of available result types for a job.
 *
function _printGetResultTypes($jobId) {
  drupal_set_message(t('[_printGetResultTypes] Begin'), 'status');
  $xmlDoc = _serviceGetResultTypes($jobId);
  $xml = simplexml_load_string($xmlDoc);
  foreach ($xml->resultType as $resultType) {
    print ($resultType['identifier']);
    if (array_key_exists($resultType['label'])) {
      print "\t".$resultType['label'];
    }
    if (array_key_exists($resultType['description'])) {
      print "\t".$resultType['description'];
    }
    if (array_key_exists($resultType['mediaType'])) {
      print "\t".$resultType['mediaType'];
    }
    if(array_key_exists($resultType['fileSuffix'])) {
      print "\t".$resultType['fileSuffix'];
    }
  }
  drupal_set_message(t('[_printGetResultTypes] End'), 'status');
}*/

/*
 * Get result
 */
function _serviceGetResult($jobId, $type, $baseUrl) {
  $requestUrl = $baseUrl.'/result/'.$jobId.'/'.$type;
  $result = file_get_contents($requestUrl);
  return $result;
}

/*
 * User-agent for request
 */
function _getUserAgent() {
  // Set 'User-Agent' HTTP header to identify the client.
  $user_agent = 'Drupal(+http://drupal.org)';
  $user_agent = 'EBI-Sample-Client/'. __file__ . base_path().$user_agent;
  return $user_agent;
}

/*
 * Wrapper for a REST (HTTP GET) request
 */
function clustalo_rest_request($url) {
  $user_agent = _getUserAgent();
  $http_headers = array('User-Agent' => $user_agent);
  $request = drupal_http_request($url);
  // Make the request (HTTP GET).
  $reqH = get_object_vars($request);
  $contentType = $request->headers['content-type'];
  if ((count($reqH)>0) && ($contentType!="image/png;charset=UTF-8") && ($contentType!="image/jpeg;charset=UTF-8") && ($contentType!="application/gzip;charset=UTF-8")) {
    $result = $request->data;
  }
  else {
    $result = $reqH;
  }
  return $result;
}

/*
 * Get input parameters list
 */
function _serviceGetParameters($baseUrl) {
  $requestUrl = $baseUrl.'/parameters';
  $xmlDoc = clustalo_rest_request($requestUrl); // xml-string data
  return $xmlDoc;
}

/*
 * Print list of parameters
 */
function _printGetParameters() {
  $xmlDoc = _serviceGetParameters(); // Data in xml string form.
  $idList = simplexml_load_string($xmlDoc); // Turn data to object
  foreach ($idList->id as $parameter) {
    print $parameter."\n";
  }
}