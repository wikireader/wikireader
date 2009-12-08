<?php

/**
**/

require_once( dirname(__FILE__) . '/DatabaseNo.php' );

class LoadBalancerNo
{
	static $instance;

	static function &getInstance() {
		if ( !isset( self::$instance ) ) {
			self::$instance = new self();
		}
		return self::$instance;
	}

	var $conn;

	function &getConnection( $i, $groups = array(), $wiki = false ) {
		if ( !isset( $this->conn )) {
			$this->conn = DatabaseNo::getInstance();
		}
		return $this->conn;
	}

	function getServerCount() {
		return 1;
	}
}
