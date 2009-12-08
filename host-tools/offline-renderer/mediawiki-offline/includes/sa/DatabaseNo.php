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

class DatabaseNo extends Database
{
	static $instance;

	static function getInstance() {
		if ( !isset( self::$instance ) ) {
			self::$instance = new self();
		}
		return self::$instance;
	}

#------------------------------------------------------------------------------
# Accessors
#------------------------------------------------------------------------------
	function failFunction( $function = NULL ) {
		return null;
	}

	function setOutputPage( $out ) {
		return null;
	}

	function debug( $debug = NULL ) {
		return null;
	}

	function bufferResults( $buffer = NULL ) {
		return null;
	}

	function ignoreErrors( $ignoreErrors = NULL ) {
		return true;
	}

	function trxLevel( $level = NULL ) {
		return 0;
	}

	function errorCount( $count = NULL ) {
		return 0;
	}

	function getLBInfo( $name = NULL ) {
		return true;
	}

	function setLBInfo( $name, $value = NULL ) {
		return true;
	}

	function cascadingDeletes() {
		return false;
	}

	function cleanupTriggers() {
		return false;
	}

	function strictIPs() {
		return false;
	}

	function realTimestamps() {
		return false;
	}

	function implicitGroupby() {
		return true;
	}

	function implicitOrderby() {
		return true;
	}

	function searchableIPs() {
		return false;
	}

	function functionalIndexes() {
		return false;
	}

	function lastQuery() {
		return true;
	}
	function isOpen() {
		return true;
	}

	function setFlag( $flag ) {
		return true;
	}

	function clearFlag( $flag ) {
		return true;
	}

	function getFlag( $flag ) {
		return true;
	}

	function getProperty( $name ) {
		return true;
	}


	function __construct( $server = false, $user = false, $password = false, $dbName = false,
		$failFunction = false, $flags = 0, $tablePrefix = 'get from global' ) {
		null;
	}

	static function newFromParams( $server, $user, $password, $dbName, $failFunction = false, $flags = 0 ) {
		return new Database( $server, $user, $password, $dbName, $failFunction, $flags );
	}

	function open( $server, $user, $password, $dbName ) {
		return true;
	}

	function close() {
		return true;
	}

	function reportConnectionError( $error = 'Unknown error' ) {
		return null;
	}

	public function query( $sql, $fname = '', $tempIgnore = false ) {
		return null;
	}

	function doQuery( $sql ) {
		return true;
	}

	function reportQueryError( $error, $errno, $sql, $fname, $tempIgnore = false ) {
		return null;
	}


	function prepare( $sql, $func = 'Database::prepare' ) {
		return null;
	}

	function freePrepared( $prepared ) {
		return null;
	}

	function execute( $prepared, $args = null ) {
		return true;
	}

	function safeQuery( $query, $args = null ) {
		return true;
	}

	function fillPrepared( $preparedQuery, $args ) {
		return true;
	}

	function fillPreparedArg( $matches ) {
		return true;
	}

	function freeResult( $res ) {
		return null;
	}

	function fetchObject( $res ) {
		return null;
	}

	function fetchRow( $res ) {
		return null;
	}

	function numRows( $res ) {
		return 0;
	}

	function numFields( $res ) {
		return 0;
	}

	function fieldName( $res, $n ) {
		return true;
	}

	function insertId() {
		return null;
	}

	function dataSeek( $res, $row ) {
		return true;
	}

	function lastErrno() {
		return null;
	}

	function lastError() {
		return null;
	}

	function affectedRows() {
		return 0;
	}

	function set( $table, $var, $value, $cond, $fname = 'Database::set' ) {
		return null;
	}

	function selectField( $table, $var, $cond='', $fname = 'Database::selectField', $options = array() ) {
		return null;
	}

	function makeSelectOptions( $options ) {
		return null;
	}

	function select( $table, $vars, $conds='', $fname = 'Database::select', $options = array() ) {
		if ( $this->isWriteQuery( $sql ) ) {
			return null;
		} else {
			return new ResultWrapper ( $this , null );
		}
	}

	function selectRow( $table, $vars, $conds, $fname = 'Database::selectRow', $options = array() ) {
		return null;
	}

	function estimateRowCount( $table, $vars='*', $conds='', $fname = 'Database::estimateRowCount', $options = array() ) {
		return 0;
	}


	static function generalizeSQL( $sql ) {
		return null;
	}

	function fieldExists( $table, $field, $fname = 'Database::fieldExists' ) {
		return null;
	}

	function indexExists( $table, $index, $fname = 'Database::indexExists' ) {
		return null;
	}


	function indexInfo( $table, $index, $fname = 'Database::indexInfo' ) {
		return null;
	}

	function tableExists( $table ) {
		return null;
	}

	function fieldInfo( $table, $field ) {
		return null;
	}

	function fieldType( $res, $index ) {
		return null;
	}

	function indexUnique( $table, $index ) {
		return null;
	}

	function insert( $table, $a, $fname = 'Database::insert', $options = array() ) {
		return null;
	}

	function makeUpdateOptions( $options ) {
		return null;
	}

	function update( $table, $values, $conds, $fname = 'Database::update', $options = array() ) {
		return null;
	}

	function makeList( $a, $mode = LIST_COMMA ) {
		return null;
	}

	function selectDB( $db ) {
		return null;
	}

	function tableName( $name ) {
		return null;
	}

	public function tableNames() {
		return null;
	}

	public function tableNamesN() {
		return null;
	}

	function tableNamesWithUseIndex( $tables, $use_index ) {
		return null;
	}

	function strencode( $s ) {
		return true;
	}

	function addQuotes( $s ) {
		return true;
	}

	function escapeLike( $s ) {
		return true;
	}

	function nextSequenceValue( $seqName ) {
		return null;
	}

	function useIndexClause( $index ) {
		return true;
	}

	function replace( $table, $uniqueIndexes, $rows, $fname = 'Database::replace' ) {
		return null;
	}

	function deleteJoin( $delTable, $joinTable, $delVar, $joinVar, $conds, $fname = 'Database::deleteJoin' ) {
		return null;
	}

	function textFieldSize( $table, $field ) {
		return null;
	}

	function lowPriorityOption() {
	}

	function delete( $table, $conds, $fname = 'Database::delete' ) {
		return true;
	}

	function insertSelect( $destTable, $srcTable, $varMap, $conds, $fname = 'Database::insertSelect',
		$insertOptions = array(), $selectOptions = array() )
	{
		return null;
	}

	function limitResult($sql, $limit, $offset=false) {
		return null;
	}

	function limitResultForUpdate($sql, $num) {
		return null;
	}

	function conditional( $cond, $trueVal, $falseVal ) {
		return null;
	}

	function wasDeadlock() {
		return null;
	}

	function deadlockLoop() {
		return null;
	}

	function masterPosWait( $file, $pos, $timeout ) {
		return null;
	}

	function getSlavePos() {
		return null;
	}

	function getMasterPos() {
		return null;
	}

	function begin( $fname = 'Database::begin' ) {
	}

	function commit( $fname = 'Database::commit' ) {
	}

	function rollback( $fname = 'Database::rollback' ) {
	}

	function immediateBegin( $fname = 'Database::immediateBegin' ) {
	}

	function immediateCommit( $fname = 'Database::immediateCommit' ) {
	}

	function timestamp( $ts=0 ) {
	}

	function timestampOrNull( $ts = null ) {
	}

	function resultObject( $result ) {
	}

	function aggregateValue ($valuedata,$valuename='value') {
	}

	function getSoftwareLink() {
		return "[http://www.mysql.com/ MySQL]";
	}

	function getServerVersion() {
	}

	function ping() {
		return true;
	}

	function getLag() {
		return 0;
	}

	function getStatus($which="%") {
		return true;
	}

	function maxListLen() {
		return 0;
	}

	function encodeBlob($b) {
		return $b;
	}

	function decodeBlob($b) {
		return $b;
	}

	public function setTimeout( $timeout ) {
	}

	function sourceFile( $filename, $lineCallback = false, $resultCallback = false ) {
		return null;
	}

	function sourceStream( $fp, $lineCallback = false, $resultCallback = false ) {
		return true;
	}


	protected function replaceVars( $ins ) {
		return true;
	}

	protected function tableNameCallback( $matches ) {
	}

	/*
	 * Build a concatenation list to feed into a SQL query
	*/
	function buildConcat( $stringList ) {
		return true;
	}
}

/**
 * Disable all types of databases (excluding Mysql) by replacing they classes with this one
**/

class DatabaseIbm_db2 extends DatabaseNo
{}

class DatabaseOracle extends DatabaseNo
{}

class DatabasePostgres extends DatabaseNo
{}

class DatabaseMssql extends DatabaseNo
{}

class DatabaseSqlite extends DatabaseNo
{}
