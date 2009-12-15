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

if ( !isset( $IP ) ) {
  $IP = realpath( dirname(__FILE__) );
}

unset( $_SERVER['SERVER_NAME'] );
unset( $_SERVER['HOSTNAME'] );
unset( $_SERVER['HTTP_HOST'] );
unset( $_SERVER['SERVER_ADDR'] );
unset( $_SERVER['SERVER_PORT'] );

require_once( "$IP/includes/DefaultSettings.php" );

# Set some globals
$wgSitename = 'MediaWikiStandAlone';
$wgProto = 'woc';
$wgServerName = 'woc';
$wgServer = $wgProto .'://'. $wgServerName;
#$wgScriptPath = '/wiki';
#$wgArticlePath = "/article/$1";
#$wgUploadPath = $IP .'/sa';
$wgParserConf['class'] = 'ParserStandAlone';
$wgLBFactoryConf['class'] = 'LBFactory_No';
$wgCommandLineMode = true;
$wgStandAloneMode  = true;
$cliParserMode     = true;
$wgTemplateDB      = getenv('TEMPLATE_DB');
$wgTemplateFileID  = 0;

# Register parser hooks
$wgHooks['SpecialPage_initList'][] = 'ParserStandAlone::disableSpecialPages';

# Extension-Fixes before require
define('WH_IMG_DIR', $wgUploadDirectory .'/wikihiero/' ); # for WikiHiero

# Activate installed extensions
require_once( "$IP/extensions/Cite/Cite.php" );
require_once( "$IP/extensions/ParserFunctions/ParserFunctions.php" );
require_once( "$IP/extensions/Poem/Poem.php" );
#require_once( "$IP/extensions/SyntaxHighlight_GeSHi/SyntaxHighlight_GeSHi.php" );
#require_once( "$IP/extensions/ImageMap/ImageMap.php" );
#require_once( "$IP/extensions/timeline/Timeline.php" );
require_once( "$IP/extensions/wikihiero/wikihiero.php" );

# Extension-Fixes after require
putenv("GDFONTPATH=/usr/share/fonts/truetype/freefont"); # for EasyTimeline

# Settings for offline templates
$wgTemplateExtension = '';
$wgTemplatePath = getenv('WORKDIR') . "/templates";


# MediaWiki settings
$wgDBtype = 'mssql'; # select a different then mysql to make db-less parsing working
$wgAllDBsAreLocalhost = true;
$wgCheckDBSchema = false;
$wgEnableEmail = false;
$wgEnableUserEmail = false;
$wgLanguageCode = 'en';
$wgUseTeX = true;
$wgUseImageMagick = true;
$wgUseTidy = true;
$wgAlwaysUseTidy = true;
$wgAllowUserSkin = false;
$wgShowExceptionDetails = true;
$wgBrowserBlackList = array();

# For Math equations, (to activate, set $wgUseTeX is true )
$WorkDirectory   = getenv('WORKDIR');
$wgMathDirectory = "{$WorkDirectory}/math";
$wgMathPath      = "$wgMathDirectory";
$wgTmpDirectory  = "{$WorkDirectory}/tmp";
 
# Uncomment for debugging
#$wgDebugLogFile = "/tmp/wiki.log";

# Disable all kinds of MediaWiki caching
$wgMainCacheType = CACHE_ACCEL;
$wgMessageCacheType = CACHE_ACCEL;
$wgParserCacheType = CACHE_ACCEL;
$wgMiserMode = true;
$wgDisableQueryPages = true;