Class OpenTransactClient() {

  private $clientKey;
  private $clientSecret;
  private $clientUri;
  const $ICES_ENDPOINT = 'ces/interop/opentransact';
  
  function __constructor($clientKey, $clientSecret, $clientUri) {
    $this->clientKey = $clientKey;
    $this->clientSecret = $clientSecret;
    $this->clientUri = $clientUri;
  }
  
  /**
   * PRIVATE BLOCK==================================================
   */
  private function hmac_base64($data, $key) {
    $hmac = base64_encode(hash_hmac('sha256', (string) $data, (string) $key, TRUE));
    return strtr($hmac, array('+' => '-', '/' => '_', '=' => ''));
  }
  
  private function get_hash_salt() {
    return hash('sha256', mcrypt_create_iv(16, MCRYPT_DEV_URANDOM));
  }
  
  private function get_token($clientKey,$clientSecret) {
    return hmac_base64($clientKey, $clientKey. $clientSecret . get_hash_salt());
  }
  
  /**
   * PUBLIC BLOCK=====================================================
   */
  public function getAuthorizeTransactionURL(from, to, amount, note) {
    $query = array(
      // OAuth2 parameters.
      'response_type' => 'code',
      'client_id' => $this->clientKey,
      'state' => $this->get_token($this->clientKey, $this->clientSecret),
      // The "authorized" url doesn't actually exist, but we don't need it.
      'redirect_uri' => $this->clientUri,
      // OpenTransact parameters.
      'from' => $from,
      'to' => $to,
      'amount' => $amount,
      'note' => $note,
      // 10 hours.
      'validity' => 'PH10',
    );
    return($this->$ICES_ENDPOINT . http_build_query($array));
  }
  
  public function doTransact($authorization_code) { 
    $token_url = $this->$ICES_ENDPOINT . '/token';
    $data = array(
      'grant_type' => 'authorization_code',
      'code' => $authorization_code,
      'redirect_uri' => $this->clientUri,
    );
    $options = array(
      'method' => 'POST',
      'data' => http_build_query($data),
      'headers' => array(
        'Content-Type' => 'application/x-www-form-urlencoded',
        'Authorization' => 'Basic ' . base64_encode($this->clientKey . ':' . $this->clientSecret),
      ),
    );
    $request = $token_url . http_build_query($options);
    $session = curl_init($request);

    curl_setopt($session, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($session, CURLOPT_SSL_VERIFYPEER, 0);
  
    $response = curl_exec($session);
    if ( ! $response) {
      $response = curl_error($session);
      error_log("no response");
      return $response;
    }
    curl_close($session);
    
    $response = json_decode($reponse->data);
    $access_token = $response->access_token;
    
    $resource_url = $this->ICES_ENDPOINT;
    $options = array(
      'headers' => array(
        'Authorization' => 'Bearer ' . $access_token,
      ),
    );
    $request = $resource_url . http_build_query($options);
    $session = curl_init($request);

    curl_setopt($session, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($session, CURLOPT_SSL_VERIFYPEER, 0);
  
    $response = curl_exec($session);
    if ( ! $response) {
      $response = curl_error($session);
      error_log("no response");
      return $response;
    }
    curl_close($session);
    
    $response = json_decode($response->data);
  }
}
