<?php
  
    $string  = "<a href=\"/~mosko/mediawiki/index.php?title=Toyo_Ito&amp;action=edit&amp;redlink=1\" class=\"new\" title=\"Toyo Ito (page does not exist)\">Toyo Ito</a>";  
    $pattern = '<a href="/~mosko\/mediawiki\/index\.php\?title=([^&]*).*?>';
    $replace = '<a href="woc://\\1">';

    $string = preg_replace($pattern, $replace, $string);

    print $string . "\n\n"

?>
