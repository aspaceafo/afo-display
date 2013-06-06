<?php
// $Id$

/**
 * @file
 *
 * Open calls home page
 */

/******************************************** display functions **/
 
/**
 * Assemble and output Open Calls home page.
 */
function afo_opencall_callspage() {

	// valid GET arguments
	$valid_get = array('medium', 'tag', 'city', 'online-submit');
	foreach($valid_get as $k) {
		if(is_numeric($_GET[$k])) $args[$k] = $_GET[$k];
	}
	
	$content['col-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => -1,
		'#width' => '22%',		
	);
	
	$intro = variable_get('afo_opencall_callspage_intro', null);
	if($intro) {
		$content['col-left']['intro'] = array(
			'#type' => 'afo_contentbox',
			'#value' => t($intro),
			'#weight' => -5,
		);
	}

	$content['col-left']['filter-calls'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Filter Calls'),
		'#value' => drupal_get_form('afo_opencall_callspage_buildfilters', $args),
		'#weight' => 0,
	);


	$content['col-center'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => 0,
		'#width' => '20px',		
	);

	$content['col-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '75%',
		'#weight' => 1,
	);
	
	$listing = afo_opencall_list_calls(array('display op'=>'normal', 'theme' => 'table', 'args' => $args));
	$content['col-right']['upcoming-deadlines'] = array(
		'#prefix' => '<div style="display:none" id="filters-throbber"throbber">Loading...</div><div id="afo-opencalls-results">',
		'#value' => $listing,
		'#suffix' => '</div>',
		'#weight' => 5,
	);

	drupal_add_js(drupal_get_path('module', 'afo_backend').'/filter.js');

	return drupal_render($content);
}

/**
 * Build open call filters
 */
function afo_opencall_callspage_buildfilters(&$form_state, $args) {

	$form['#tree'] = true;

	$form['enable-filters'] = array(
		'#type' => 'hidden',
		'#value' => 1,
	);
	$form['filters-results'] = array(
		'#type' => 'hidden',
		'#value' => '#afo-opencalls-results',
	);
	$form['filters-ajax-url'] = array(
		'#type' => 'hidden',
		'#value' => 'calls/ajax',
	);
	$form['submit-type'] = array(
		'#type' => 'hidden',
		'#value' => 'button',
	);
	
	$mediums = afo_backend_tree_to_options(taxonomy_get_tree(variable_get('afo_custommenu_vocab_medium', 0)));
	$form['opencall-filters']['medium'] = array(
		'#type' => 'select',
		'#title' => t('Medium'),
		'#options' => $mediums,
		'#default_value' => $args['medium'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);

	$tags = afo_backend_tree_to_options(taxonomy_get_tree(variable_get('afo_custommenu_vocab_tags', 0)));
	$form['opencall-filters']['tag'] = array(
		'#type' => 'select',
		'#title' => t('Tag'),
		'#options' => $tags,
		'#default_value' => $args['tag'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);

/*
	$cities = afo_backend_tree_to_options(taxonomy_get_tree(variable_get('afo_custommenu_vocab_city', 0)));
	$form['opencall-filters']['city'] = array(
		'#type' => 'select',
		'#title' => t('City'),
		'#options' => $cities,
		'#default_value' => $args['city'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);
*/	
	$city_vid = variable_get('afo_profile_city_vid', null);
	$form['opencall-filters']['city'] = array(
		'#type' => 'textfield',
		'#title' => t('City'),
		'#autocomplete_path' => 'taxonomy/autocomplete/'.$city_vid,
		'#maxlength' => 1024,
		'#size' => 20,
		'#element_validate' => array('afo_backend_city_validate'),
		'#default_value' => $args['city'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);	

	$deadline_options = array(
		'upcoming' => t('All upcoming'),
		'3 months' => t('Next 3 months'),
		'3-6 months' => t('3 to 6 months'),
		'6 months' => t('6+ months'),
		'past' => t('past'),
	);
	$form['opencall-filters']['deadline'] = array(
		'#type' => 'select',
		'#title' => t('Deadline'),
		'#options' => $deadline_options,
		'#default_value' => $args['deadline'],
		'#display' => 'table',
		'#attributes' => array('class'=>'filter'),
	);

	$form['opencall-filters']['online-submit'] = array(
		'#type' => 'checkbox',
		'#title' => t('online submission <br />through Local Artists'),
		'#return_value' => 1,
		'#default_value' => $args['online-submit'],
		'#display' => 'table',
		'#attributes' => array(
			'class'=>'filter',
			'title' => t('Only show open calls that allow online submission.'),
		),
	);
	
	$form['spacer'] = array(
		'#value' => '<div style="width:65px; float: left">&nbsp;</div>',
		'#weight' => 7,
	);
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Search'),
		'#attributes' => array(
//			'class' => 'js-hide',
		),
		'#weight' => 8,
	);

	$form['reset'] = array(
		'#type' => 'markup',
		'#value' => l(t('reset'), 'calls', array('attributes'=>array('class'=>'reset-filters'))),
		'#weight' => 9,
	);

	return $form;
	
}
