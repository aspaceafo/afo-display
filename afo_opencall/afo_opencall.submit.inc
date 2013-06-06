<?php
// $Id$

/**
 * @file
 *
 * Submission functions for AFO Open Call module.
 */
 
/**
 * Form builder for submit to call
 */
function afo_opencall_submitto($form_state, $nid) {
	if(!is_numeric($nid)) return null;

	global $user;
	$call_node = node_load($nid);
	$form['#tree'] = true;
	$form['#redirect'] = 'node/'.$nid;
	
	$allowed = afo_opencall_check_submit_allowed($call_node, $user, 'message');
	if(!$allowed) return null;
	
	// load user's portfolio
	module_load_include('inc', 'afo_portfolio', 'afo_portfolio.classes');
	$portfolio = new portfolioPortfolio(array('uid'=>$user->uid));
	$collapsed = array();
	$portfolio->hierarchyCollapse($collapsed);	

	$form['call_node'] = array(
		'#type' => 'fieldset',
		'#title' => t('Call details'),
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$form['call_node']['node'] = array(
		'#value' => node_view($call_node, true, false, false),
	);
	
	$form['uid'] = array(
		'#type' => 'value',
		'#value' => $user->uid,
	);

	$form['nid'] = array(
		'#type' => 'value',
		'#value' => $nid,
	);

	$form['#call_node'] = $call_node;
	
	// check if user has artworks
	if(count($collapsed) == 0) {
		drupal_set_message(t('Your portfolio contains no artworks to submit.'));
	}
	
	// user has artworks
	else {
		$form['portfolio'] = array(
			'#type' => 'fieldset',
			'#title' => t('Your portfolio'),
			'#description' => t('Choose up to %max pieces to submit to the curator.', array('%max'=>$call_node->call_data->submitlimit)),
			'#theme' => 'afo_opencall_submitto_artworktable',
		);
		foreach($collapsed as $item) {
			if($item->itemType == 'artwork') {
				$form['portfolio'][$item->nid]['nid'] = array(
					'#type' => 'hidden',
					'#value' => $item->nid,
				);
				$form['portfolio'][$item->nid]['include'] = array(
					'#type' => 'checkbox',
					'#return_value' => $item->nid,
				);
				$form['portfolio'][$item->nid]['thumbnail'] = array(
					'#value' => theme('afo_display_node_thumbnail', $item->nid, 'artwork_tiny', 'none'),
				);
				$form['portfolio'][$item->nid]['title'] = array(
					'#value' => $item->data['title'],
				);
			}
		}
		
		$form['note'] = array(
			'#type' => 'textfield',
			'#title' => t('Short note'),
			'#description' => t('Include a short note for the curator (optional)'),
			'#maxlength' => 255,
		);
	
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Send submission'),
		);

	}
	
	return $form;
}
/**
 * Submit to call form: validate
 */
function _afo_opencall_submitto_validate($form, &$form_state) {

	global $user;

	// check if allowed to submit
	$allowed = afo_opencall_check_submit_allowed($form['#call_node'], $user, 'form_error');
	
	$call_data = $form['#call_node']->call_data;
	
	// check if no artworks submitted
	$artworks = afo_opencall_submitto_compile_artworks($form_state['values']['portfolio']);
	if(!is_array($artworks)) {
		form_set_error('portfolio', t('You must submit at least one artwork.'));
	}
	// check if too many artworks submitted
	if(count($artworks) > $call_data->submitlimit) {
		form_set_error('portfolio', t('You can submit a maximum of %max pieces.', array('%max'=>$call_data->submitlimit)));	
	}
	
}
/**
 * Submit to call form: submit
 */
function _afo_opencall_submitto_submit($form, &$form_state) {

	$submission = new stdclass;
	$submission->nid = $form_state['values']['nid'];
	$submission->uid = $form_state['values']['uid'];
	$submission->note = check_plain($form_state['values']['note']);
	$submission->updated = time();

	drupal_write_record('afo_opencall_submissions', $submission);

	if(is_numeric($submission->submission_id)) {
		$artworks = afo_opencall_submitto_compile_artworks($form_state['values']['portfolio']);
		foreach($artworks as $artwork_nid) {
			$artwork = new stdclass;
			$artwork->submission_id = $submission->submission_id;		
			$artwork->artwork_nid = $artwork_nid;
			drupal_write_record('afo_opencall_submissions_artworks', $artwork);
		}
		
		drupal_set_message(t('Your open call submission has been received.')); 
	}

}

/**
 * Compile submit artworks array
 */
function afo_opencall_submitto_compile_artworks($portfolio) {
	if(!is_array($portfolio)) return null;
	
	foreach($portfolio as $item) {
		if($item['include'] > 0) {
			$artworks[] = $item['nid'];
		}
	}
	
	return $artworks;
}

/**
 * Theme portfolio table for submit to call form
 */
function theme_afo_opencall_submitto_artworktable($element) {

	foreach (element_children($element) as $key) {

		$artwork = &$element[$key];

		$row = array();
		$row[] = drupal_render($artwork['nid']) . drupal_render($artwork['include']); 			
		$row[] = drupal_render($artwork['thumbnail']); 			
		$row[] = drupal_render($artwork['title']);
		
		$rows[] = $row;
	}
		
	$header = array('', '', t('Select artworks'));
	$content = theme('table', $header, $rows);

	return $content;
}







