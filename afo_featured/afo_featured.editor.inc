<?php
// $Id$

/**
 * @file
 *
 * Editor functions for AFO Featured module.
 */

/***************************************** db functions **/

/**
 * Add featured.
 */
function afo_featured_add($admin_uid, $val, $feature_id, $op='node', $featured_name=NULL, $featured_note=NULL) {

	if(is_numeric($admin_uid) && is_numeric($val) && is_numeric($feature_id)) {
		$check_existing = afo_featured_check($val, $feature_id, $op);

		if(!$check_existing) {

			$record = new stdClass();
			if($op=='user') {
				$table_name = 'afo_featured_users';
				$record->uid = $val;
				$success_message = 'This artist was successfully featured.';
			}
			else {
				$table_name = 'afo_featured_nodes';
				$record->nid = $val;
				$success_message = 'This item was successfully featured.';
			}
			$record->admin_uid = $admin_uid;
			$record->feature_id = $feature_id;
			$record->featured_name = $featured_name;
			$record->featured_note = $featured_note;
			$record->updated = time();
			
			$success = drupal_write_record($table_name, $record);
			if($success) {
				drupal_set_message($success_message);
				module_invoke_all('featured_itemfeatured', $record, $op);
			}
			
		}
	}
	
}

/***************************************** Add featured form **/

/**
 * Form builder: add to favorites.
 */
function afo_featured_addform(&$form_state, $nid=NULL, $uid=NULL, $op='new', $item_id=NULL) {
	global $user;
	
	if($op != 'new' && is_numeric($item_id)) {

		$item = afo_featured_getitem($item_id, $op);

		$form['item_id'] = array(
			'#type' => 'value',
			'#value' => $item_id,
		);
		
	}

	$form['op'] = array(
		'#type' => 'value',
		'#value' => $op,
	);
	
	/*
	$form['feature_this'] = array(
		'#type' => 'fieldset',
		'#title' => 'expand for options',
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	*/
	
	$form['feature_this']['admin_uid'] = array(
		'#type' => 'value',
		'#value' => $user->uid,
	);
	
	$feature_options = (array)afo_featured_get_feature_types();
	$feature_options[0] = '<new>';
	
	$form['feature_this']['feature_id'] = array(
		'#type' => 'select',
		'#title' => 'Feature type',
		'#options' => $feature_options,
		'#required' => TRUE,
		'#default_value' => $item->feature_id,
	);
	
	$form['feature_this']['featured_name'] = array(
		'#type' => 'textfield',
		'#title' => 'Feature name',
		'#size' => 20,
		'#maxlength' => 255,
		'#default_value' => $item->featured_name,
	);

	$form['feature_this']['featured_note'] = array(
		'#type' => 'textfield',
		'#title' => 'Feature note',
		'#size' => 20,
		'#maxlength' => 255,
		'#default_value' => $item->featured_note,
	);		

	if($nid > 0) {
		$form['feature_this']['nid'] = array(
			'#type' => 'value',
			'#value' => $nid,
		);
	
		$form['feature_this']['submit-feature-node'] = array(
			'#type' => 'submit',
			'#value' => 'Feature this item',
		);
	}
	
	if(is_numeric($uid)) {
		$form['feature_this']['uid'] = array(
			'#type' => 'value',
			'#value' => $uid,
		);
		
		$form['feature_this']['submit-feature-user'] = array(
			'#type' => 'submit',
			'#value' => 'Feature this Artist',
		);
	}
						
	return $form;
}
/**
 * Form builder: validate ADD form.
 */
function afo_featured_addform_validate($form, $form_state) {

}
/**
 * Form builder: submit ADD form.
 */
function afo_featured_addform_submit($form, &$form_state) {
	$admin_uid = $form_state['values']['admin_uid'];
	$feature_id = $form_state['values']['feature_id'];
	$featured_name = $form_state['values']['featured_name'];
	$featured_note = $form_state['values']['featured_note'];

	if($form_state['clicked_button']['#id'] == 'edit-submit-feature-node') {
		$op = 'node';
		$val = $form_state['values']['nid'];
	}
	elseif($form_state['clicked_button']['#id'] == 'edit-submit-feature-user') {
		$op = 'user';
		$val = $form_state['values']['uid'];
	}

	if(is_numeric($admin_uid) && is_numeric($val) && is_numeric($feature_id) && $op) {
		afo_featured_add($admin_uid, $val, $feature_id, $op, $featured_name, $featured_note);
	}
}

/***************************************** Edit Feature types form **/

/**
 * Editor function Edit Feature Types page
 */
function afo_featured_admin_types_page() {

	$query = db_query('SELECT * FROM {afo_featured_types} ORDER BY feature_name ASC');
	
	$headers = array('', 'Title', 'Description', 'Items', 'Users', '');
	
	while($row = db_fetch_array($query)) {
		// get item counts
		$user_count = db_result(db_query('SELECT COUNT(featured_id) AS total FROM {afo_featured_users} WHERE feature_id = %d', $row['feature_id']));
		$node_count = db_result(db_query('SELECT COUNT(featured_id) AS total FROM {afo_featured_nodes} WHERE feature_id = %d', $row['feature_id']));
		
		// get items
		$featured_items_list = null;
		if($user_count > 0 || $node_count > 0) {
			module_load_include('inc', 'afo_featured', 'afo_featured.browse');
			$featured_items = afo_featured_getfeature($row['feature_id']);
			$featured_types = array('nodes', 'users');
			foreach($featured_types as $k) {
				if(is_array($featured_items[$k])) {
					$items_list = array();
					foreach($featured_items[$k] as $i) {
						if($k == 'users') { $label = $i['name']; $url = 'user/'.$i['uid']; }
						else { $label = $i['title']; $url = 'node/'.$i['nid']; }
						$items_list[] = l($label, $url) . " ({$i[featured_name]}: {$i['featured_note']})";
					}
					$featured_items_list .= "$k:";
					$featured_items_list .= theme('item_list', $items_list, null, 'ul', array('class'=>'arrow'));					
				}
			}
		}
	
		$expand_link = theme('afo_display_icon', 'arrow');
		$desc = $row['feature_description'];
		if($featured_items_list) $desc .= '<br />' . $featured_items_list;
		$edit_link = l(t('edit'), 'editor/featured/types/edit/'.$row['feature_id']);
		$feature_types[] = array($expand_link, $row['feature_name'], $desc, $node_count, $user_count, $edit_link);
	}
	
	$content['feature-types'] = array(
		'#value' => theme_table($headers, $feature_types),
		'#weight' => -5,
	);

	$content['add-new-type'] = array(
		'#value' => drupal_get_form('afo_featured_admin_types'),
		'#weight' => 0,
	);
	
	return drupal_render($content);	
}
/**
 * Form builder for admin types page.
 */
function afo_featured_admin_types($form_state, $feature_id = null) {

	if(!$feature_id || !is_numeric($feature_id)) $feature_id = 'new';
	else {
		// get feature type data
		$query = db_query('SELECT * FROM {afo_featured_types} WHERE feature_id = %d', $feature_id);
		if(!$feature_data = db_fetch_object($query)) drupal_set_message(t('Unable to find feature type.'), 'error');
	}
	
	$form['#redirect'] = 'editor/featured/types';
	$form['newtype'] = array(
		'#type' => 'fieldset',
		'#title' => t('Add or edit feature type'),
	);	
	$form['newtype']['feature_id'] = array(
		'#type' => 'value',
		'#value' => $feature_id,
	);
	$form['newtype']['feature_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Feature title'),
		'#required' => TRUE,
		'#maxlength' => 200,
		'#default_value' => $feature_data->feature_name,
	);
	$form['newtype']['feature_description'] = array(
		'#type' => 'textfield',
		'#title' => t('Description'),
		'#required' => FALSE,
		'#maxlength' => 255,
		'#default_value' => $feature_data->feature_description,
	);
	$form['newtype']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);

	if($feature_id != 'new') {
		$form['return_link'] = array(
			'#value' => l('« Back to list', 'editor/featured/types'),
			'#weight' => -10,
		);
		$form['newtype']['delete'] = array(
			'#type' => 'submit',
			'#value' => 'Delete',
		);
	}

	return $form;	
}
/**
 * Form submit for admin types page.
 */
function afo_featured_admin_types_submit($form, &$form_state) {

	$feature_id = $form_state['values']['feature_id'];

	// delete button pressed	
	if($form_state['values']['delete'] == 'Delete' && is_numeric($feature_id)) {
	
		$success = db_query('DELETE FROM {afo_featured_types} WHERE feature_id = %d', $feature_id);
		if($success) {
			$delete_nodes = db_query('DELETE FROM {afo_featured_nodes} WHERE feature_id = %d', $feature_id);
			$delete_users = db_query('DELETE FROM {afo_featured_users} WHERE feature_id = %d', $feature_id);
			
			// delete from newsfeed
			if(module_exists('afo_newsfeed')) afo_newsfeed_delete_featured($feature_id);
			
			$message = 'Feature type deleted.';
		}
		else $message = 'Unable to delete feature type.';

	} else {

		$record = new StdClass();
		$record->feature_name = $form_state['values']['feature_name'];
		$record->feature_description = $form_state['values']['feature_description'];
		
		if(is_numeric($feature_id)) {
			$record->feature_id = $feature_id;
			drupal_write_record('afo_featured_types', $record, 'feature_id');
			$message = 'Feature type updated.';
		}
		else {
			drupal_write_record('afo_featured_types', $record);
			$message = 'New feature type added.';	
		}
		
	}
	
	if($message) drupal_set_message(t($message));
}

/***************************************** Edit Feature type form **/

/**
 * Form builder for featured item edit.
 */
function afo_featured_admin_edit_item($form_state, $op, $item_id) {

	if($op == 'user') {
		$table = 'afo_featured_users';
		$keyfield = 'uid';
	}
	elseif($op == 'node') {
		$table = 'afo_featured_nodes';
		$keyfield = 'nid';
	}
	
	$query = db_query('SELECT * FROM {%s} WHERE %s = %d', $table, $keyfield, $item_id);
	
	if($item = db_fetch_array($query)) {
	
		$types_table = theme_table($headers, $feature_types);
		
		$form['types_table'] = array(
			'#type' => 'markup',
			'#value' => $types_table,
		);
		
		$form['newtype'] = array(
			'#type' => 'fieldset',
			'#title' => t('Add or edit feature type'),
		);
		
		$form['newtype']['feature_id'] = array(
			'#type' => 'value',
			'#value' => 'new',
		);
		$form['newtype']['feature_name'] = array(
			'#type' => 'textfield',
			'#title' => t('Feature title'),
			'#required' => TRUE,
			'#maxlength' => 200,
		);
		$form['newtype']['feature_description'] = array(
			'#type' => 'textfield',
			'#title' => t('Description'),
			'#required' => FALSE,
			'#maxlength' => 255,
		);
		$form['newtype']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
		);

	}
	
	return $form;	
}
/**
 * Form submit for admin types page.
 */
function afo_featured_admin_edit_item_submit($form, &$form_state) {
	$record = new StdClass();
	$record->feature_name = $form_state['values']['feature_name'];
	$record->feature_description = $form_state['values']['feature_description'];
	
	if($form_state['values']['feature_id'] == 'new') {
		drupal_write_record('afo_featured_types', $record);
		drupal_set_message('New feature type added.');
	}
}

/***************************************** Editor Curating settings page **/
/**
 * Form builder
 */
function afo_featured_editor_curating() {

	$form['afo_featured_curatingpage_intro'] = array(
		'#type' => 'textarea',
		'#title' => t('Intro text'),
		'#description' => t('Block of text introducing the Curating page'),
		'#default_value' => variable_get('afo_featured_curatingpage_intro', ''),
	);
	
	return system_settings_form($form);
}