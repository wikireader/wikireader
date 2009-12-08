<?php

/**
 * Copyright (C) 2008, 2009 Michael Nowak
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

require_once( dirname(__FILE__) .'/LBFactory_No.php' );

class ParserStandAlone extends Parser
{
  # Override the template-fetching-function of the Parser
  function fetchTemplateAndTitle( $title ) {
    #echo "\n--- Trying to find offline template: $title";
    global $IP, $wgTemplatePath, $wgTemplateExtension, $wgTemplatePrefix;
    $finalTitle = $title;
    #$deps = array();

    if ( !isset( $wgTemplatePath ) ) {
      #$wgTemplatePath = "$IP/sa/templates";
      $wgTemplatePath = "/home/mosko/samo/work/templates";
    }
    if ( !isset( $wgTemplateExtension ) ) {
      #$wgTemplateExtension = '.mwt';
      $wgTemplateExtension = '';
    }
    if ( !isset( $wgTemplatePrefix ) ) {
      $wgTemplatePrefix = 'Template:';
    }

    # $$$ need to fix later for all languages
    $title_short = substr($title, strlen($wgTemplatePrefix));
    #echo " (Short: $title_short)";
    $title_md5 = md5($title, false);
    $title_short_md5 = md5(strtolower($title_short), false);

    $template_path = $wgTemplatePath .'/'. $title_md5 . $wgTemplateExtension;
    $template_short_path = $wgTemplatePath .'/'. $title_short_md5 . $wgTemplateExtension;
    $template_text = null;

    if ( file_exists( $template_path ) ) {
      $template_text = file_get_contents( $template_path );
      #echo " - Found\n";
    }
    elseif ( file_exists( $template_short_path ) ) {
      $template_text = file_get_contents( $template_short_path );
      #echo " - Found Short\n";
    }
    else {
      $template_text = null;
      #echo " - Not Found\n";
    }
    #foreach ($deps as $dep ) {
    #    $this->mOutput->addTemplate( $dep['title'], $dep['page_id'], $dep['rev_id'] );
    #}
    return array( $template_text, $finalTitle );
  }

  static public function disableSpecialPages( &$mList ) {
    $mList = array();
    return true;
  }
}
