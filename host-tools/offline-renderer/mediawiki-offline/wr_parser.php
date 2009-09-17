<?php

/**
 * This is the main cli entry point for MediaWiki.
 *
 * See the README and INSTALL files for basic setup instructions
 * and pointers to the online documentation.
 *
 * ----------
 *
 * Copyright (C) 2008 Michael Nowak
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

# ShortOptions:
#  -f: Path to the file with wiki-markup or '-' to read wiki-markup from STDIN
#  -l: Path to the file that contains the list of files that contains wiki-markup
#  -m: Memory-Size in MB for PHP
#  -o: Output-Format: XML, HTML, YAML, JSON, TXt
#  -k: Keep wiki-markup files after parsing
$parserShortOptions = 'f:l:m:o:k';
# LongOptions: -
// MOSKO $parserLongOptions = array(); # array( 'file', 'list', 'memory-php', 'output-format', 'keep' );
# Get options
// MOSKO $parserOptions = getopt( $parserShortOptions, $parserLongOptions );

if ( isset($parserOptions['m']) ) {
  ini_set( 'memory_limit', $parserOptions['m'].'M' );
} else {
  #ini_set( 'memory_limit', '128M' );
  ini_set( 'memory_limit', '1G' );
}

if ( isset($parserOptions['o']) ) {
  $outputFormat = strtolower( $parserOptions['o'] );
} else {
  $outputFormat = 'woc';
}

# Initialise common code
require (dirname(__FILE__) . '/includes/WebStart.php');
require (dirname(__FILE__) . '/includes/ParserOffline.php');


wfProfileIn('wr_parser.php');

# Setup for CLI & API mode
$wgCommandLineMode = true;
$cliParserMode     = true;
$wgEnableAPI       = true;

# Set parser options
wfSetParserOptions();

# Misc
$text       = null;
$action     = 'parse';
$format     = $outputFormat;
$fileName   = null;
$fileList   = null;
$extMarkup  = '.markup';
$extArticle = '.article';
$rmMarkup   = true;

//*
#
# $$$ Added the following code to take the file from
#     Python script or by passing a file
#
if ($argv[1] == "-") {
	$fp = STDIN;
} else {
	$fp = fopen($argv[1], "r");
}


$body = '';
$cnt = 0;

while (!feof($fp)) {
    #$line = fgets($fp, 4096);
    $line = fgets($fp, 8192);

	if ($line == "***EOF***\n") {
		echo wfParseTextAndWrapWOC(&$body);
		$body = '';
	} else {
		$body .= $line;
	}
}

fclose($fp);

# Log what the user did, for book-keeping purposes.
wfProfileOut('wr_parser.php');
wfLogProfilingData();
exit(0);



# Global function to set parser options
function wfSetParserOptions() {
  global $wgParserOptions, $wgParser;
  $wgParserOptions = new ParserOptions(null);
  $wgParserOptions->setEditSection(false);
  $wgParserOptions->setTemplateCallback( array( 'ParserOffline', 'fetchTemplate' ) );
  $wgParser->Options($wgParserOptions);
  #$wgParser->disableCache();
  return $wgParserOptions;
}

# Global function for parsing text with ApiMain
function wfParseText($text, $action='parse', $format='xml') {
	# Initialise faux request
	$cliRequest = new FauxRequest( array( 'action' => &$action, 'text' => &$text, 'format' => &$format ) );

	# Initialise api and execute
	$processor = new ApiMain($ctmp2liRequest, $wgEnableWriteAPI);
	$processor->execute();

	# generate result and print the result
	$printer = $processor->createPrinterByName($format);
	$result = $processor->getResult();
	if ($printer->getNeedsRawData()) {
		$result->setRawMode();
	}
	$printer->initPrinter(false);
	$printer->execute();
	$printer->closePrinter();
  return true;
}



### Wikipedia Offline Client - Stuff ###########################################

function &wfOutputWrapperWOC($articleTitle, $articleText) {
  $articleOutput = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n".
  "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n".
  "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n".
  "  <head>\n".
  "    <title>".$articleTitle."</title>\n".
  "  </head>\n".
  "  <body>\n".
	"  <h1>".$articleTitle."</h1>\n".
  $articleText."\n".
  "  </body>\n".
  "</html>\n";
  return $articleOutput;
}


# Global function for 'Wikipedia Offline Client'-specific parsing
function &wfParseTextWOC($text) {
  global $wgParser, $wgParserOptions;
  $nlidx = strpos($text, "\n");
  $articleTitle = trim(substr($text, 0, $nlidx));
  $articleMarkup = substr($text, $nlidx + 1);
  $title = new Title();
  #$output = $wgParser->preparse($articleMarkup, $title, $wgParserOptions, null);
  $output = $wgParser->parse($articleMarkup, $title, $wgParserOptions, true, true, null);
  $articleText = $output->getText();

  # $$$ MOSKO: change the links
  $articleText = str_replace(' (page does not exist)">', '">', $articleText);
  $articleText = preg_replace('/<a\s[^>]*title="([^"]*)">/', '<a href="$1">', $articleText);

  #$articleText = html_entity_decode($articleText, ENT_COMPAT, 'UTF-8');
	#$articleText = str_replace('&', '&amp;', $articleText);

  return array( &$articleTitle, &$articleText );
}

# Global function for 'WOC'-specific parsing
function &wfParseTextAndWrapWOC($text) {
  $result = wfParseTextWOC($text);
  $articleTitle = $result[0];
  $articleText = $result[1];
  $articleOutput = wfOutputWrapperWOC($articleTitle, $articleText);
  return $articleOutput;
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



?>
