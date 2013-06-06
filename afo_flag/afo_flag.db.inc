<?php
// $Id$

/**
 * @file
 *
 * db functions for AFO flag module.
 */

/**
 * Insert flag into db.
 */
function afo_flag_putflag($val, $type = 0) {
	if(!is_numeric($val) || !is_numeric($type)) return false;
	
	global $user;
	$uid = $user->uid;
	
	$existing = afo_flag_getflag($val, $uid, $type);
	if($existing) {
		drupal_set_message(t('You have already flagged that item.'));
		return false;
	}
	
	$record = new stdclass;
	$record->val = $val;
	$record->type = $type;
	$record->uid = $uid;
	$record->updated = time();
	
	$result = drupal_write_record('afo_flag_flags', $record);
	if($result) {
		drupal_set_message(t('Item flagged.'));
		return true;
	}
	else {
		drupal_set_message(t('There was a database error while flagging the item.'), 'error');
		return false;
	}
	
	return false;
}

/**
 * Get flag from db.
 */
function afo_flag_getflag($val, $uid = 0, $type = 0) {
	if(!is_numeric($val) || !is_numeric($uid) || !is_numeric($type)) return null;
	
	$query = db_query('SELECT * FROM {afo_flag_flags} WHERE val = %d AND flag_type = %d AND uid = %d', $val, $type, $uid);
	if($flag = db_fetch_array($query)) return $flag;
	
	return null;
}