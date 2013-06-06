<?php
// $Id$

/**
 * @file
 * Administration callbacks for the AFO Popularity module.
 */
 

/**
 * Admin settings form builder
 *
 * @ingroup forms
 */
function afo_popularity_admin_settings() {
	$options['content types'] = node_get_types('names');
	$options['roles'] = user_roles(true);

	$ranking_types = afo_popularity_ranking_types();
	
	foreach($ranking_types as $key => $ranking_type) {
	
		$form[$ranking_type['id']] = array(
			'#type' => 'fieldset',
			'#title' => t($ranking_type['name']),
		);
			$form[$ranking_type['id']][$ranking_type['content types variable']] = array(
				'#type' => 'checkboxes',
				'#title' => t('Choose content types'),
				'#options' => $options[$ranking_type['options']],
				'#default_value' => variable_get($ranking_type['content types variable'], array()),
				'#description' => t("{$ranking_type[name]} will be indexed for these content types."),
			);

	}
	
	return system_settings_form($form);
}

/**
 * Admin settings page > index site
 */
function afo_popularity_admin_index() {	
	afo_popularity_index_site();
	return '';
}
