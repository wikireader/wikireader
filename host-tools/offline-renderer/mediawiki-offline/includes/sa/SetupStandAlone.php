<?php

/**
**/

require_once( dirname(__FILE__) .'/../AutoLoader.php' );

global $wgAutoloadLocalClasses;
global $wgStandAloneClasses;

$wgStandAloneClasses = array(
    'DatabaseNo', 'LBFactory_No', 'LoadBalancerNo', 'ParserStandAlone',
);

foreach ( $wgStandAloneClasses as $wgStandAloneClass) {
    $wgAutoloadLocalClasses[$wgStandAloneClass] = "includes/sa/{$wgStandAloneClass}.php";
}