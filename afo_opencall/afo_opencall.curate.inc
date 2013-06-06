<?php
// $Id$

/**
 * @file
 *
 * Curator functions for AFO open call system.
 */

/******************************************** create open call **/

/**
 * Alter open call node form.
 */
function afo_opencall_callformalter(&$form, $form_state) {

	$current_call = $form['#node']->call_data;

	$form['#submit'][] = 'afo_opencall_callformsubmit';

	$form['title']['#weight'] = -5;
	
	$form['body_field']['body']['#description'] = t('Add a short paragraph describing your request or project. If you are not accepting online submissions, include contact or submission information.');
	
	// submission deadline
	$default_date = is_numeric($current_call->deadline) ? date('Y-m-d G:i:s', $current_call->deadline) : date('Y-m-d G:i:s', strtotime('+30 days'));
	$form['deadline'] = array(
		'#type' => 'date_popup',
		'#title' => t('Submission Deadline'),
		'#description' => t('Your open call must have a deadline. After the deadline has been passed, submissions will no longer be accepted and your call will be removed from the active open call lists.'),
		'#weight' => -4,
		'#required' => true,
		'#default_value' => $default_date, 
		'#date_format' => 'j-n-Y',
		'#date_label_position' => 'within', // See other available attributes and what they do in date_api_elements.inc
		'#date_timezone' => 'America/New_York', // Optional, if your date has a timezone other than the site timezone.
		'#date_year_range' => '0:+1', // Optional, used to set the year range (back 3 years and forward 3 years is the default).
	);

	// submission artwork limit
	$default_limit = is_numeric($current_call->submitlimit) ? $current_call->submitlimit : variable_get('afo_settings_opencall_limit_default', 3);
	$limit_options[0] = t('No online submission');
	for($i=1; $i<=10; $i++) { $limit_options[$i] = $i; }
	$form['submitlimit'] = array(
		'#type' => 'select',
		'#title' => t('Submit artwork online'),
		'#description' => t('Select the maximum number of artworks each user can submit to your open call through %sitename, or "No online submission."', array('%sitename' => variable_get('site_name', 'this site'))),
		'#weight' => -3,
		'#default_value' => $default_limit,
		'#required' => true,
		'#options' => $limit_options,
	);
	
	$form['taxonomy']['#weight'] = -2;
		
}

/**
 * Supplement open call node form submit
 */
function _afo_opencall_callformsubmit($form, &$form_state) {

	if(is_numeric($form_state['values']['nid'])) {
		$call = new stdclass;
		$call->nid = $form_state['values']['nid'];
		$call->deadline = $form_state['values']['deadline'];
		$call->submitlimit = $form_state['values']['submitlimit'];
		afo_opencall_update_call($call);
	}
		
}

/**
 * Update open call values
 */
function afo_opencall_update_call($call) {
	if(!is_object($call)) return false;
	if(!is_numeric($call->nid)) return false;

	$record = new stdclass;
	$record->nid = $call->nid;

	// process deadline field
	if(isset($call->deadline)) {
		$record->deadline = strtotime($call->deadline);
	} else {
		$record->deadline = strtotime('+30 days'); // default
	}
	
	// process limit field 
	if(is_numeric($call->submitlimit)) $record->submitlimit = $call->submitlimit;
	else $record->submitlimit = 3; // default
	
	$exists = afo_opencall_get_call($record->nid);
	if($exists) $pkey = 'nid';
	else $pkey = null;
	
	drupal_write_record('afo_opencall_calls', $record, $pkey);
	
	return $record;
}


/******************************************** review submissions functions **/

/**
 * Submissions list form
 */
function afo_opencall_list_submissions_form($form_state, $nid) {
	if(!is_numeric($nid)) return null;
	
	$form['#tree'] = true;
	
	$form['submissions'] = array(
		'#type' => 'fieldset',
		'#prefix' => '<hr />',
		'#title' => t('Submissions'),
		'#collapsible' => true,
	);
	
	$submissions = afo_opencall_get_submissions($nid);

	// no submissions
	if(!$submissions) {
		$form['submissions']['message'] = array(
			'#value' => '<p>'.t('No one has submitted work to this call yet.').'</p>',
		);
		
		return $form;
	}
	
	// yes submissions
	drupal_add_js(drupal_get_path('module', 'afo_opencall').'/curate.js');	
	
	$form['submissions']['help'] = array(
		'#value' => '<p>'.t('These are artists\' submissions to your open call. They are hidden to other users. You may include these works in your exhibition, or browse other work by the artists. To ignore submissions, check the Ignore box.').'</p>',
		'#weight' => -5,
	);

	foreach($submissions as $sub) {
		$form['submissions']['artists'][$sub->submission_id] = array(
			'#prefix' => '<div class="afo-opencall-submission boxed"><div class="afo-opencall-submission-inner inner-medium">',
			'#suffix' => '</div></div>',
		);
		$form['submissions']['artists'][$sub->submission_id]['submission_id'] = array(
			'#type' => 'hidden',
			'#value' => $sub->submission_id,
			'#attributes' => array('class' => 'submission_id'),
		);
		/*
		$form['submissions']['artists'][$sub->submission_id]['ignore'] = array(
			'#type' => 'checkbox',
			'#prefix' => '<div class="listing-ignore js-show">',
			'#title' => t('ignore'),
			'#suffix' => '</div>',
			'#return_value' => 1,
			'#default_value' => $sub->curator_seen,
		);
		*/
		$form['submissions']['artists'][$sub->submission_id]['artist'] = array(
			'#prefix' => '<h2 class="inline">',
			'#value' => $sub->name,
			'#suffix' => '</h2>',
		);
		$form['submissions']['artists'][$sub->submission_id]['link'] = array(
			'#prefix' => '&nbsp;&nbsp;&nbsp;&nbsp;<div class="secondaryText inline">',
			'#value' => l(t('View portfolio in new window »'), 'user/'.$sub->uid, array('attributes'=>array('target'=>'_blank'))),
			'#suffix' => '</div>',
		);
		$form['submissions']['artists'][$sub->submission_id]['date'] = array(
			'#prefix' => '<div class="secondaryText">',
			'#value' => t('Submitted: ') . date('j M Y', $sub->updated),
			'#suffix' => '</div>',
		);
		$form['submissions']['artists'][$sub->submission_id]['note'] = array(
			'#prefix' => '<div class="secondaryText">',
			'#value' => $sub->note,
			'#suffix' => '</div>',
		);
		$form['submissions']['artists'][$sub->submission_id]['artwork'] = array(
			'#value' => theme('afo_display_grid', $sub->artworks, null, 'artwork', array('classes'=>array('tightGrid'), '#expandable'=>true)),
		);
	}

	return $form;
}

/**
 * Ignore submission callback
 */
function afo_opencall_ignore($submission_id = null) {
	if(!$submission_id) $submission_id = $_POST['submission_id'];
	if(!is_numeric($submission_id)) return false;

	global $user;
	
	// get call owner uid and current ignore status
	$query = db_query('SELECT n.uid, s.curator_seen FROM {afo_opencall_submissions} s LEFT JOIN {node} n ON n.nid = s.nid WHERE s.submission_id = %d', $submission_id);
	if($result = db_fetch_object($query)) {
		// make sure current user is owner of call
		if($result->uid === $user->uid) {
			$record = new stdclass;
			$record->submission_id = $submission_id;
			$record->curator_seen = $result->curator_seen==1 ? 0 : 1;
			$success = drupal_write_record('afo_opencall_submissions', $record, 'submission_id');
		}
	}

	if($success) $result = 1;
	else $result = 0;

	if(!empty($_POST['js'])) {
		drupal_json(array(
			'result' => $result,
		));
	}
	else {
		return "$result <br /> $success";
	}
}
