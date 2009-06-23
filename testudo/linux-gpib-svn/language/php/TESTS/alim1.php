#!/usr/bin/php4 -q
<?
$u = ibfind("dev1");
print "numero d'unite = $u\n";
ibpad($u,25) or die("ibpad");
ibwrt($u,"*RST");
?>
       
