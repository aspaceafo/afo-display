<?php
// $Id$

/**
 * @file
 *
 * Modified AFO back end forms and functions.
 */
 
/**
 * Form builder for Editor merge terms page
 */
function afo_backend_editor_merge_terms() {

	$city_vid = variable_get('afo_profile_city_vid', null);

	if($city_vid) {
	
		$form['description'] = array(
			'#value' => t('The term to be merged will be deleted and anything tagged with it will be re-tagged with the target term.'),
			'#weight' => -5,
		);
	
		$form['merge-term'] = array(
			'#type' => 'textfield',
			'#title' => t('Merge term'),
			'#description' => t('Select the term to merge into another.'),
			'#autocomplete_path' => 'taxonomy/autocomplete/'.$city_vid,
		);

		$form['merge-target'] = array(
			'#type' => 'textfield',
			'#title' => t('Merge into'),
			'#description' => t('Select the term into which the other will be merge.'),
			'#autocomplete_path' => 'taxonomy/autocomplete/'.$city_vid,
		);
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Merge'),
		);
		
	}

	return $form;
}
/**
 * Form submit for Editor merge terms page
 */
function _afo_backend_editor_merge_terms_submit($form, $form_state) {

	if(!user_access('merge terms')) return null;

	$merge_term = $form_state['values']['merge-term'];
	$merge_target = $form_state['values']['merge-target'];
	
	if($merge_term && $merge_target) {

		$merge_term = db_result(db_query('SELECT tid FROM {term_data} WHERE name = \'%s\'', $merge_term));
		$merge_target = db_result(db_query('SELECT tid FROM {term_data} WHERE name = \'%s\'', $merge_target));

		if(is_numeric($merge_term) && is_numeric($merge_target)) {
		
			// set all term_node values to new tid
			$update_nodes = db_query('UPDATE {term_node} SET tid = %d WHERE tid = %d', $merge_target, $merge_term);
			$update_users = db_query('UPDATE {term_user} SET tid = %d WHERE tid = %d', $merge_target, $merge_term);
			
			// if no problem, delete term
			if(!$update_nodes) drupal_set_message(t('Error updating term nodes; merge cancelled.'), 'error');
			else if(!$update_users) drupal_set_message(t('Error updating term users; merge cancelled.'), 'error');
			else {
				taxonomy_del_term($merge_term);		
			}
			
		}
	}
}

