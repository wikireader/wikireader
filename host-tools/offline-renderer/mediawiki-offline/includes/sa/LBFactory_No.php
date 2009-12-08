<?php

/**
**/

require_once( dirname(__FILE__) .'/LoadBalancerNo.php' );

class LBFactory_No extends LBFactory
{
	var $mainLB;
	var $externalLB;

	function __construct( $conf ) {
		return null;
	}

	function newMainLB( $wiki = false ) {
		return LoadBalancerNo::getInstance();
	}

	function getMainLB( $wiki = false ) {
		if ( !isset( $this->mainLB ) ) {
			$this->mainLB = $this->newMainLB( $wiki );
		}
		return $this->mainLB;
	}

	function newExternalLB( $cluster, $wiki = false ) {
		return array(
			$this->getMainLB( $wiki ),
		);
	}

	function &getExternalLB( $cluster, $wiki = false ) {
		if ( !isset( $this->externalLB ) ) {
			$this->externalLB = $this->newExternalLB( $cluster, $wiki );
		}
		return $this->externalLB;
	}

	function forEachLB( $callback, $params = array() ) {
	}
}
