<?php
// Base URL for service
$baseUrl = 'http://www.ebi.ac.uk/Tools/services/rest/clustalo';

$checkInterval = 10;  // Set interval for checking status
$outputLevel = 1;  // Output Level
$debugLevel = 0;  // Debug Level
$numOpts = len($options);  // Number of option arguments.
$debugLevel = 0;  // Debug level

// Process command-line options
$parameters = array(
  // Tool specific options
  'guidetreeout' => NULL,
  'noguidetreeout' => NULL,
  'dismatout' => NULL,
  'nodismatout' => NULL,
  'dealign' => NULL,
  'nodealign' => NULL,
  'mbed' => NULL,
  'nombed' => NULL,
  'mbediteration' => NULL,
  'nombediteration' => NULL,
  'iterations' => 0,
  'gtiterations' => 0,
  'hmmiterations' => 0,
  'outfmt' => NULL,
  'stype' => NULL,
  'sequence' => NULL,
  
  // General options
  'email' => '',
  'title' => '',
  'outfile' => '',
  'outformat' => '',
  'jobid' => '',
  'async' => NULL,
  'polljob' => NULL,
  'resultTypes' => NULL,
  'status' => NULL,
  'params' => NULL,
  'paramDetail' => '',
  'quiet' => NULL,
  'verbose' => NULL,
  'debugLevel' => 0,
  'baseUrl' => $baseUrl,
  );

$options = getopt(NULL, $parameters);

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

/*
 * Debug print
 */
function _printDebugMessage($functionName, $message, $level) {
  $stderr = fopen('php://stderr', 'w+');
  if ($level <= $debuglevel) {
    fwrite($stderr,('['.$functionName.']'.$message));
    print $stderr;
  fclose($stderr);
  }
}

/*
 * User-agent for request
 */
function clustalo_get_user_agent() {
  _printDebugMessage('clustalo_get_user_agent', 'Begin', 11);
  // Set 'User-Agent' HTTP header to identify the client.
  $user_agent = 'Drupal(+http://drupal.org)';
  //$revisionNumber = $1;
  $user_agent = 'EBI-Sample-Client/'. __file__ . base_path().$user_agent;
  
  _printDebugMessage('clustalo_get_user_agent', 'user_agent: '.$user_agent, 12);
  _printDebugMessage('clustalo_get_user_agent', 'End', 11);
  return $user_agent;
}

/*
 * Wrapper for a REST (HTTP GET) request
 */
function clustalo_rest_request($url) {
  _printDebugMessage('clustalo_rest_request', 'Begin', 11);
  _printDebugMessage('clustalo_rest_request', 'url: '.$url, 11);
  try {
    $user_agent = clustalo_get_user_agent();
    $http_headers = array('User-Agent' => $user_agent);
    $request = drupal_http_request($url, $http_headers);
    // Make the request (HTTP GET).
    $reqH = get_object_vars($request);
    $contentType = $request->headers['content-type'];
    
    if (count($request)>0 && $contentType != "image/png;charset=UTF-8" && $contentType != "image/jpeg;charset=UTF-8" && contentType != "application/gzip;charset=UTF-8") {
      $result = utf8_encode(ToString($request));  //python result = str(regH, 'utf-8')
    }
    else {
      $result = $response;
    }
  }
  catch {}
  _printDebugMessage('clustalo_rest_request', 'End', 11);
  return $result;
}

/*
 * Get input parameters list
 */
function _serviceGetParameters() {
  _printDebugMessage('_serviceGetParameters', 'Begin', 1);
  $requestUrl = $baseUrl.'/parameters';
  _printDebugMessage('_serviceGetParameters', 'requestUrl: '.$requestUrl, 2);
  $xmlDoc = clustalo_rest_request($requestUrl);
  $doc = drupal_xml_parser_create($xmlDoc);
  _printDebugMessage('_serviceGetParameters', 'End', 1);
  return $doc['id'];
}

/*
 * Print list of parameters
 */
function _printGetParameters() {
  _printDebugMessage('_printGetParameters', 'Begin', 1);
  $idList = _serviceGetParameters();
  foreach ($idList as $id) {
    print ($id);
  }
  _printDebugMessage('_printGetParameters', 'End', 1);
}

/*
 * Get input parameter information
 */
function _serviceGetParameterDetails($paramName) {
  _printDebugMessage('_serviceGetParameterDetails', 'Begin', 1);
  _printDebugMessage('_serviceGetParameterDetails', 'requestUrl: '.$paramName, 2);
  $requestUrl = $baseUrl."/parameterdetails".$paramName;
  $xmlDoc = clustalo_rest_request($requestUrl);
  $doc = drupal_xml_parser_create($xmlDoc);
  _printDebugMessage('serviceGetParameterDetails', 'End', 1);
  return $doc;
}

/*
 * Print description of a parameter
 */
function _printGetParameterDetails($paramName) {
  _printDebugMessage('_printGetParameterDetails', 'Begin', 1);
  $doc = _serviceGetParameterDetails($paramName);
  print string($doc['name'])."\t".string($doc['type']);
  print $doc['description']
  foreach $doc['value'] in $value as $values {
    print ($value['value']).' ';
    if string($value->defaultValue) == 'true' {
      print 'default';
    }
    print "\n"."\t".string($value->label);
    if ($value['properties']) {
      foreach ($value['properties'] as $properties => $property) {
        if $property == 'wsProperty' {
          print "\t".string($property['wsProperty']['key'])."\t".
        }
      }
    }
  }
}

/*
 * Submit job
 */
function _serviceRun($email, $title, $params) {
  _printDebugMessage('_serviceRun', 'Begin', 1);
  // Insert e-mail and title into params
  $params['email'] = $email;
  if ($title) {
    $params['title'] = $title;
  }
  $requestUrl = $baseUrl."/run/";
  _printDebugMessage('serviceRun', 'requestUrl: '.$requestUrl, 2);

  // Get the data for the other options
  $requestData = urllib.parse.urlencode($params);

  _printDebugMessage('serviceRun', 'requestData: '.$requestData, 2);
  // Errors are indicated by HTTP status codes.
  try {
    // Set the HTTP User-agent.
    $user_agent = clustalo_get_user_agent();
    $http_headers = array('User-Agent' => $user_agent);
    $request = clustalo_rest_request($requestUrl, array('headers' => $http_headers), 'POST', $params);
    // Make the submission (HTTP POST).
  }
  catch (Exception 
  except urllib2.HTTPError as ex:
    // Trap exception and output the document to get error message.
    print (ex.read(), file=sys.stderr)
    raise
    _printDebugMessage('serviceRun', 'jobId: ' + jobId, 2)
    _printDebugMessage('serviceRun', 'End', 1)
    return jobId
}

/*
 * Get job status
 */
function _serviceGetStatus($jobId) {
  _printDebugMessage('serviceGetStatus', 'Begin', 1);
  _printDebugMessage('serviceGetStatus', 'jobId: '.$jobId, 2);
  $requestUrl = $baseUrl."/status/".$jobId;
  _printDebugMessage('serviceGetStatus', 'requestUrl: '.$requestUrl, 2);
  $status = restRequest($requestUrl);
  _printDebugMessage('serviceGetStatus', 'status: '.$status, 2);
  _printDebugMessage('serviceGetStatus', 'End', 1);
  return $status;
}


/*
 * Print the status of a job
 */
function _printGetStatus($jobId){
  _printDebugMessage('_printGetStatus', 'Begin', 1);
  $status = _serviceGetStatus($jobId);
  print $status;
  _printDebugMessage('_printGetStatus', 'End', 1);
}

/*
 * Get available result types for job
 */
function _serviceGetResultTypes($jobId){
  _printDebugMessage('_serviceGetResultTypes', 'Begin', 1);
  _printDebugMessage('_serviceGetResultTypes', 'jobId: '.$jobId, 2);
  $requestUrl = $baseUrl."/resulttypes/".$jobId;
  _printDebugMessage('_serviceGetResultTypes', 'requestUrl: '.$requestUrl, 2);
  $xmlDoc = clustalo_rest_request($requestUrl);
  $doc = drupal_xml_parser_create($xmlDoc);
  _printDebugMessage('_serviceGetResultTypes', 'End', 1);
  return $doc['type'];
}

/*
 * Print list of available result types for a job.
 */
function _printGetResultTypes($jobId) {
  _printDebugMessage('_printGetResultTypes', 'Begin', 1);
  $resultTypeList = _serviceGetResultTypes($jobId);
  foreach ($resultTypeList as $resultType) {
    print ($resultType['identifier'])
    if ($resultType['label']) {
      print "\t".$resultType['label'];
    }
    if ($resultType['description']) {
      print "\t".$resultType['description'];
    }
    if ($resultType['mediaType']) {
      print "\t".$resultType['mediaType'];
    }
    if($resultType['fileSuffix']) {
      print "\t".$resultType['fileSuffix'];
    }
  }
  _printDebugMessage('_printGetResultTypes', 'End', 1);
}

/*
 * Get result
 */
function _serviceGetResult($jobId, $type_) {
  _printDebugMessage('serviceGetResult', 'Begin', 1);
  _printDebugMessage('serviceGetResult', 'jobId: '.$jobId, 2);
  _printDebugMessage('serviceGetResult', 'type_: '.$type_, 2);
  $requestUrl = $baseUrl.'/result/'.$jobId.'/'.$type_;
  $result = restRequest($requestUrl);
  _printDebugMessage('serviceGetResult', 'End', 1)
  return $result;
}

/*
 * Client-side poll
 */
function clientPoll($jobId) {
  _printDebugMessage('clientPoll', 'Begin', 1);
  $result = 'PENDING';
  while ($result == 'RUNNING') || ($result == 'PENDING') {
    $result = _serviceGetStatus($jobId);
    file_put_contents($stderr, $result);
    if ($result == 'RUNNING') or ($result == 'PENDING') {
      sleep($checkInterval);
    }
  }
  _printDebugMessage('clientPoll', 'End', 1);
}

/*
 * Get result for a jobid
 */
function clustalo_get_result($jobId) {
  _printDebugMessage('clustalo_get_result', 'Begin', 1);
  _printDebugMessage('clustalo_get_result', 'jobId: '.$jobId, 1);
  # Check status and wait if necessary
  clientPoll($jobId);
  # Get available result types
  $resultTypes = _serviceGetResultTypes($jobId);
  foreach ($resultTypes as $resultType) {
    # Derive the filename for the result
    if options.outfile:
    filename = options.outfile + '.' + str(resultType['identifier']) + '.' + str(resultType['fileSuffix'])
    else:
    filename = jobId + '.' + str(resultType['identifier']) + '.' + str(resultType['fileSuffix'])
    # Write a result file
    if not options.outformat or options.outformat == str(resultType['identifier']):
      # Get the result
      result = serviceGetResult(jobId, str(resultType['identifier']))
    if(str(resultType['mediaType']) == "image/png"
    or str(resultType['mediaType']) == "image/jpeg"
    or str(resultType['mediaType']) == "application/gzip"):
      fmode= 'wb'
    else:
      fmode='w'
      fh = open(filename, fmode);
      file_put_contents($fh, $result);
      fclose($fh);
      print ($filename);
  }
    _printDebugMessage('clustalo_get_result', 'End', 1);
}

/*
 * Read a file
 */
function readFile($filename){
  _printDebugMessage('readFile', 'Begin', 1)
  $fh = fopen($filename, 'r')
  $data = fread($fh, filesize($filename));
  fclose($fh);
  _printDebugMessage('readFile', 'End', 1);
  return $data;
}

/*
 * No options... print help.
 *
if numOpts < 2:
  parser.print_help()
# List parameters
elif options.params:
  printGetParameters()
# Get parameter details
elif options.paramDetail:
  printGetParameterDetails(options.paramDetail)
# Submit job
elif options.email and not options.jobid:
  params = {}
  if len(args) > 0:
    if os.access(args[0], os.R_OK): # Read file into content
      params['sequence'] = readFile(args[0])
    else: # Argument is a sequence id
      params['sequence'] = args[0]
    elif options.sequence: # Specified via option
      if os.access(options.sequence, os.R_OK): # Read file into content
        params['sequence'] = readFile(options.sequence)
      else: # Argument is a sequence id
         params['sequence'] = options.sequence
  # Booleans need to be represented as 1/0 rather than True/False
  if options.guidetreeout:
    params['guidetreeout'] = True
  else:
    params['guidetreeout'] = False
  if options.dismatout:
    params['dismatout'] = True
  else:
    params['dismatout'] = False
  if options.dealign:
    params['dealign'] = True
  else:
    params['dealign'] = False
  if options.mbed:
    params['mbed'] = True
  else:
    params['mbed'] = False
  if options.mbediteration:
    params['mbediteration'] = True
  else:
    params['mbediteration'] = False

  # Add the other options (if defined)
  if options.stype:
    params['stype'] = options.stype
  if options.iterations:
    params['iterations'] = options.iterations
  if options.gtiterations:
    params['gtiterations'] = options.gtiterations
  if options.hmmiterations:
    params['hmmiterations'] = options.hmmiterations
  if options.outfmt:
    params['outfmt'] = options.outfmt
    
  # Submit the job
  jobid = serviceRun(options.email, options.title, params)
  if options.async: # Async mode
    print (jobid)
  else: # Sync mode
    print (jobid, file=sys.stderr)
    time.sleep(5)
    getResult(jobid)
# Get job status
elif options.status and options.jobid:
  printGetStatus(options.jobid)
# List result types for job
elif options.resultTypes and options.jobid:
  printGetResultTypes(options.jobid)
# Get results for job
elif options.polljob and options.jobid:
  getResult(options.jobid)
else:
  print ('Error: unrecognised argument combination', file=sys.stderr)
  parser.print_help()
  */
