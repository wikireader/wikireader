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
require_once( "$IP/LocalSettings.php" );

class ParserStandAlone extends Parser
{
  const LINK_OPTION_BROKEN = 'broken';
  const LINK_OPTION_KNOWN = 'known';

  /**
   * @var array
   */
  static protected $languageNames = null;

  /**
   * @var array
   */
  static protected $templatesExistingCache = null;

  /**
   * @var array
   */
  static protected $templatesMissingCache = null;

  /**
   * @var array
   */
  static protected $detectedLanguageLinks = null;

  /**
   * Override the template-fetching-function of the Parser
   *
   * @global string $IP
   * @global string $wgTemplatePath
   * @global string $wgTemplateExtension
   * @global string $wgTemplatePrefix
   * @param Title $title
   * @return array
   */
  function fetchTemplateAndTitle( $title ) {
    #echo "\n--- Trying to find offline template: $title ---\n";

    global $wgTemplateDB, $wgTemplateFileID;
    $finalTitle    = $title;
    $template_text = null;

    # $$$ need to fix later for all languages
    # We pad the title with '~' to force the database to import strings
    $title_orig  = '~' . $wgTemplateFileID . '~' . strtolower($title);
    $db = new PDO('sqlite:' . $wgTemplateDB);
    $tl = $db->quote($title_orig);

    #echo "\n<!-- Orig: {$title_orig}\n     SQL: {$tl} -->\n";

    $result = $db->query("SELECT body FROM templates WHERE title = {$tl} LIMIT 1");
    $data = $result->fetchAll();
    $max_loop_count = 25;
    while ($max_loop_count && sizeof($data) == 0) {
      $result = $db->query("SELECT redirect FROM redirects WHERE title = {$tl} LIMIT 1");
      $data = $result->fetchAll();
      if (sizeof($data) == 0) {
        break;
      }
      $redirect = $db->quote($data[0]['redirect']);
      $result = $db->query("SELECT body FROM templates WHERE title = {$redirect} LIMIT 1");
      $data = $result->fetchAll();
      --$max_loop_count;
    }

    if (sizeof($data) > 0) {
      $template_text = substr($data[0]['body'], 1);
      #echo "\n--- TT:($template_text):TT --- \n";
    } else {
      $template_text = '';
    }

    $ret = array( $template_text, $finalTitle );
    return $ret;
  }

  /**
   * @param array $mList
   * @return bool
   */
  static public function disableSpecialPages( &$mList ) {
    $mList = array();
    return true;
  }

  /**
   * @param bool $refresh default: false
   * @return array
   */
  static public function getLanguageNames($refresh = false) {
    if ($refresh || self::$languageNames === null) {
      self::$languageNames = Language::getLanguageNames();
      #var_dump(self::$languageNames);
    }
    return self::$languageNames;
  }

  /**
   * @global bool $wgLanguageLinks;
   * @param Skin $skin
   * @param Title $target
   * @param string $text
   * @param array $customAttribs
   * @param array $query
   * @param array $options
   * @param mixed $ret default: null
   * @return bool
   */
  static public function hookLinkBegin( $skin, $target, &$text, &$customAttribs, &$query, &$options, &$ret ) {
    # first make link known.
    $brokenKey = array_search(self::LINK_OPTION_BROKEN, $options);
    if ( $brokenKey !== false ) {
      $options[$brokenKey] = self::LINK_OPTION_KNOWN;
    } else if ( array_search(self::LINK_OPTION_KNOWN, $options) === false ) {
      $options []= self::LINK_OPTION_KNOWN;
    }

    $targetNamespace = $target->getNamespace();
    if ($targetNamespace == NS_FILE || $targetNamespace == NS_FILE_TALK) {
      global $wgFileLinks;
      return $wgFileLinks;
    } else if ($targetNamespace == NS_CATEGORY || $targetNamespace == NS_CATEGORY_TALK) {
      global $wgCategoryLinks;
      return $wgCategoryLinks;
    }

    # try detect language link
    $titleText = $target->getUserCaseDBKey();
    #echo PHP_EOL . "TitleText: {$titleText} -- NameSpace: {$target->getNamespaceKey()}";
    $titleExploded = explode(':', $titleText, 2);
    if ( array_key_exists(1, $titleExploded) ) {
      $languageKey = $titleExploded[0];
      #echo PHP_EOL . "Title maybe a LanguageLink: {$languageKey}";
      if ( array_key_exists($languageKey, self::getLanguageNames()) ) {
        global $wgLanguageLinks;
        self::$detectedLanguageLinks []= $target->getFullText();
        #echo PHP_EOL . "LanguageLink detected: {$languageKey} $titleExploded[1]";
        return $wgLanguageLinks;
      } else {
        return true;
      }
    } else {
      return true;
    }
  }

  /**
   * @param Skin $skin
   * @param Title $target
   * @param array $options
   * @param string $text
   * @param array $attribs
   * @param mixed $ret default: null
   * @return bool
   */
  static public function hookLinkEnd( $skin, $target, $options, &$text, &$attribs, &$ret ) {
    return true;
  }

  /**
   *
   * @param Parser $parser
   * @param string $text
   * @return void
   */
  static public function hookParserBeforeTidy( &$parser, &$text ) {
    $parser->mOutput->setLanguageLinks(self::$detectedLanguageLinks);
    self::$detectedLanguageLinks = null; # clear
    return true;
  }

  /**
   *
   * @param Parser $parser
   * @param string $text
   * @param string $strip_state
   * @return bool
   */
  static public function hookParserBeforeStrip( &$parser, &$text, &$strip_state ) {
    self::$detectedLanguageLinks = null; # ensure clear
    return true;
  }
}
