<?php
// $Id$

/**
 * @file
 *
 * Display functions for AFO Open Call module.
 */
 
/**
 * Alter open call node for display
 * Called by hook_nodeapi
 */
function afo_opencall_nodedisplay(&$node) {

	if(!is_object($node->call_data)) return false;

	global $user;
	$call = $node->call_data;
	
	$node->content['deadline'] = array(
		'#value' => theme('afo_display_deadline', $call->deadline),
		'#weight' => -5,
	);
	
	// SUBMIT TO THIS link
	if(is_numeric($node->nid) && arg(0)=='node' && $user->uid != $node->uid) {
		// see if user has permissions
		if(user_access('submit to calls')) {
			$allow_message = afo_opencall_check_submit_allowed($node, $user, 'text');
		}
		else {
			$allow_message = t('To submit to this open call you must have an artist account. Please !link.', array('!link'=>l('sign-in or register', 'sign-in')));
		}
		if($allow_message === true) {
			$submitto = l(t('Submit to this open call'), 'calls/submit/'.$node->nid, array('attributes'=>array('class' => 'buttonLink')));
		} elseif(is_string($allow_message)) {
			$submitto = "<div class='message'>$allow_message</div>";
		}
		if($submitto) {
			$node->content['submitto'] = array(
				'#value' => $submitto,
				'#weight' => 4,
			);
		}
	}
	
	// check if owner
	if($user->uid == $node->uid) {
		module_load_include('inc', 'afo_opencall', 'afo_opencall.curate');
		$node->content['submissions'] = array(
			'#value' => drupal_get_form(afo_opencall_list_submissions_form, $node->nid),
			'#weight' => 5,
		);
	}

}

/********************************* theme functions **/

/**
 * Theme open calls list
 */
function theme_afo_opencall_list($data, $op='normal', $title=null) {
	if(!is_array($data)) return null;
	
	$content .= '<div class="afo-opencall-list"><div class="afo-opencall-list-inner">';
	foreach($data as $call) {
		$list[] = theme('afo_opencall_listing', $call, $op);
	}
	$content .= theme('item_list', $list, $title, 'ul', array('class'=>'arrow'));
	$content .= '</div></div>';
		
	return $content;
}

/**
 * Theme open call listing
 */
function theme_afo_opencall_listing($data, $op='normal') {
	if(!is_object($data)) return null;
	if(!isset($data->title) || !isset($data->nid)) return null;
	
	switch($op) {
	
		case 'short':
			$content .= '<div class="afo-opencall-listing tightText">';	
			$content .= '<div class="listing-title">'.l($data->title, 'node/'.$data->nid).'</div>';
			if($data->deadline) $content .= '<div class="listing-deadline">'.theme('afo_display_deadline', $data->deadline).'</div>';
			$content .= '</div>';
			break;
			
		default:
			$content .= '<div class="afo-opencall-listing tightText">';	
			$content .= '<div class="listing-title">'.l($data->title, 'node/'.$data->nid).'</div>';
			if($data->deadline) $content .= '<div class="listing-deadline">'.theme('afo_display_deadline', $data->deadline).'</div>';
			if($data->name && is_numeric($data->uid)) {
				$content .= '<div class="listing-author secondaryText">' . t('Posted by ') . l($data->name, 'user/'.$data->uid) . '</div>';		
			}
			if($data->body) {
				$body_text = theme('afo_display_text', $data->body, array('output'=>'plain'));
				$content .= '<div class="listing-body">' . $body_text . '</div>';
			}
			$content .= '</div>';
		
	}
		
	return $content;
}