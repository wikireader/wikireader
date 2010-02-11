<?php

/**
 * This is the main cli entry point for MediaWiki.
 *
 * See the README and INSTALL files for basic setup instructions
 * and pointers to the online documentation.
 *
 * ----------
 *
 * Copyright (C) 2009 Michael Nowak
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

ini_set( 'memory_limit', '1G' );
ini_set( 'xdebug.max_nesting_level', 250 );

$wgTemplatePrefix = 'Template:';


# Initialise common code
require ( dirname(__FILE__) .'/includes/sa/SetupStandAlone.php' );
require ( dirname(__FILE__) .'/includes/WebStart.php' );

wfProfileIn('wr_parser_sa.php');

# Set parser options
wfSetParserOptions();

if ($argv[1] == "-") {
	$fp = STDIN;
} else {
	$fp = fopen($argv[1], "r");
}

# make sure output buffering is off before we start it
# this will ensure same effect whether or not ob is enabled already
while (ob_get_level()) {
    ob_end_flush();
}

# start output buffering
if (ob_get_length() === false) {
    ob_start();
}

$body = '';
$cnt  = 0;
$i    = 0;

while (!feof($fp)) {
    $line = fgets($fp, 8192);
	if ($line == "***EOF***\n") {
		echo wfParseTextAndWrapWOC(&$body);
		if ($i++ % 1000 == 0 ) {	# flush every 1000 articles
			ob_flush();		# otherwise PHP runs out of
			flush();		# memory
		}
		$body = '';
	} else {
		$body .= $line;
	}
}

fclose($fp);

# Log what the user did, for book-keeping purposes.
wfProfileOut('wr_parser_sa.php');
wfLogProfilingData();
exit(0);


# Global function to set parser options
function wfSetParserOptions() {
  global $wgParserOptions, $wgParser;
  $wgParserOptions = new ParserOptions(null);
  $wgParserOptions->setEditSection(false);
  $wgParser->Options($wgParserOptions);
  return $wgParserOptions;
}

# Global function for parsing text with ApiMain
function wfParseText($text, $action='parse', $format='xml') {
	# Initialise faux request
	$cliRequest = new FauxRequest( array( 'action' => &$action, 'text' => &$text, 'format' => &$format ) );

	# Initialise api and execute
	$processor = new ApiMain($cliRequest);
	$processor->execute();

	# generate result and print the result
	$printer = $processor->createPrinterByName($format);
	$result = $processor->getResult();
	if ($printer->getNeedsRawData()) {
		$result->setRawMode();
	}
    $result->cleanUpUTF8();
    #$printer->profileIn();
	$printer->initPrinter(false);
	$printer->execute();
	$printer->closePrinter();
    #$printer->profileOut();
    return true;
}


### Wikipedia Offline Client - Stuff ###########################################

function &wfOutputWrapperWOC($articleTitle, $articleText, $articleLanguageLinks) {

  # We store titles as unicode so do conversion here
  $articleTitle = htmlspecialchars($articleTitle);

  $articleOutput = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n".
  "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n".
  "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n".
  "  <head>\n".
  "    <title>".$articleTitle."</title>\n".
  "  </head>\n".
  "  <body>\n".
	"  <h1>".$articleTitle."</h1>\n".
  $articleText."\n".
  "<div class='noprint lang-links'>" . $articleLanguageLinks . "</div> \n".
  "  </body>\n".
  "</html>\n";
  return $articleOutput;
}


# Global function for 'Wikipedia Offline Client'-specific parsing
function &wfParseTextWOC($text) {
  global $wgParser, $wgParserOptions, $wgTemplateFileID;

  $nlidx = strpos($text, "\n");
  $temp_h = trim(substr($text, 0, $nlidx));
  $id = strpos($temp_h, ":");
  $wgTemplateFileID = trim(substr($temp_h, 0, $id));
  $articleTitle = trim(substr($temp_h, $id+1));

  $articleMarkup = substr($text, $nlidx + 1);
  $title = Title::newFromText($articleTitle);
  if (!$title) {
    $title = Title::newFromText('NULL Title');
  }

  $output = $wgParser->parse($articleMarkup, $title, $wgParserOptions, true, true, null);
  $articleText = $output->getText();

  # Make the language links
  $langLinks = "\n  <ul>\n";

  foreach ($output->getLanguageLinks() as $link){
	$langLinks .= '    <li><a class="lang-link" href="' . $link . '">' . $link . "</a></li>\n";
  }

  $langLinks .= "  </ul>\n";

  # change the links
  $articleText = str_replace(' (page does not exist)">', '">', $articleText);
  $articleText = preg_replace('/<a\s[^>]*title="([^"]*)">/', '<a href="$1">', $articleText);

  # fix blank tags and pre as start of article
  $articleText = preg_replace('/<(dt|dd|li)>\s*<\/\1>/', '', $articleText);
  $articleText = preg_replace('/<p>\s*<br\s*\/>\s*<\/p>|(<(ul|dl|ol)>\s*)+(<\/(ul|dl|ol)>\s*)+/', '', $articleText);
  $articleText = preg_replace('/^\s*<pre>(.*?)<\/pre>/s', '<p>$1</p>', $articleText);
  $articleText = preg_replace('/<p>\s*<br\s*\/>/', '<p>', $articleText);
  $articleText = preg_replace('/<p>\s*<br\s*\/>/', '<p>', $articleText);
  $articleText = preg_replace('/<a\s+name="([rR]eferences|[nN]otes)"\s+id="([rR]eferences|[nN]otes)"><\/a><h2>\s+<span\s+class="mw-headline">\s*([rR]eferences|[nN]otes)\s*<\/span><\/h2>\s*$/', '', $articleText);
  $articleText = str_replace('%25', '%', $articleText);

  $ret = array( &$articleTitle, &$articleText, &$langLinks );
  return $ret;
}

# Global function for 'WOC'-specific parsing
function &wfParseTextAndWrapWOC($text) {
  $result = wfParseTextWOC($text);
  $articleTitle = $result[0];
  $articleText = $result[1];
  $articleLangLinks = $result[2];
  $articleOutput = wfOutputWrapperWOC($articleTitle, $articleText, $articleLangLinks);
  return $articleOutput;
}

function &wfParseTextAndSkin($text) {
  global $wgParser;
  return $wgParser;
}

# Global helper function for 'WOC'-specific parsing
/** function for reading a file from end **/
/**
  * before you call this function first time on a handle
  * the file pointer have to be set at the end of the file '-2'
  * e.g. 'fseek($fileHandle, -2, SEEK_END);'
 **/
function fgets_reverse ($handle)
{
  $s_a = array();
  while("" != ($c = fread($handle, 1)))
  {
    if (ftell($handle) == 0)
    {
      fseek($handle, 0, SEEK_SET);
      break; // we are at the start of the file
    }
    else
    {
      fseek($handle, -2, SEEK_CUR);
    }
    if ($c != "\n" && $c != "")
    {
      array_push($s_a, $c);
    }
    else
    {
      break; // we are at the end of the line
    }
  }
  return implode("", array_reverse($s_a)); // create a string from this array in reversed order
}
