<?php

namespace integralCES;

require_once( 'tools/configuration.inc' );

require_once( 'entities/payments.inc' );
require_once( 'entities/users.inc' );
require_once( 'entities/accounts.inc' );

/**
 * IntegralCES API main entry point.
 * Provides managers to connect, send and read data from IntegralCES API
 * as well as holds configuration/authorization data.
 */
class icesApi {

  /**
   * Configuration instance
   * @var \integralCES\configuration
   */
  public $configuration;     // @var integralCES\configuration

  public $payments;  // @var integralCES\payments
  public $users;     // @var integralCES\users
  public $accounts;  // @var integralCES\accounts

  function __construct() {
    // default config setup
    $this->configuration = new configuration();
    // entities
    $this->users = new users($this);
    $this->payments = new payments($this);
    $this->accounts = new accounts($this);
  }

}
