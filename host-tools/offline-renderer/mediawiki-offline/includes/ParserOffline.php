<?php

/**
 * Copyright (C) 2008 Michael Nowak
 *               Sean Moss-Pultz <sean@openmoko.com>
 *               Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * http://www.gnu.org/copyleft/gpl.html
**/

class ParserOffline {

  # Override the template-fetching-function of the Parser
  static function fetchTemplate( $title ) {
    #echo "\n--- Trying to find offline template: ". $title ."\n";
    global $IP, $wgTemplatePath, $wgTemplateExtension;
    $finalTitle = $title;
    $deps = array();

    $title_md5 = md5($title, false);

    # $$$ need to fix later for all languages
    $title_short_md5 = md5(strtolower(substr($title, 9, 1024)), false);
    $template_path = $wgTemplatePath .'/'. $title_md5 . $wgTemplateExtension;
    $template_short_path = $wgTemplatePath .'/'. $title_short_md5 . $wgTemplateExtension;
    $template_text = null;

    if ( file_exists( $template_path ) ) {
      $template_text = file_get_contents( $template_path );
    }
    elseif ( file_exists( $template_short_path ) ) {
      $template_text = file_get_contents( $template_short_path );
    }
    else {
      $template_text = null;
    }
    
    return array( 'text' => $template_text, 'finalTitle' => $finalTitle, 'deps' => $deps );
  }
}

?>
