<?php
// $Id$

/**
 * @file
 *
 * Registration checklist
 */
 
/**
 * build checklist block
 */
function afo_backend_checklist() {

	$steps[1] = array(
		'title' => 'Create your account',
		'description' => 'Enter basic info and tags.',
	);
	$steps[2] = array(
		'title' => 'Confirm Email',
		'description' => 'A message will be sent with a one-time access link.',
	);
	$steps[3] = array(
		'title' => 'Change password',
		'description' => 'Set your account password.',
	);
	$steps[4] = array(
		'title' => 'Get started!',
		'description' => 'Upload images to your portfolio and fill out your statement and CV.',
	);

	$this_page = implode('/', arg());
	global $user;
	
	if($this_page == 'user/register') $step = 1;
	else if($this_page == 'user/confirm-email' && $user->uid == 0) $step = 2;
	else if($this_page == 'user/confirm-email' && $user->uid > 0) $step = 3;

	if($step) {
		$options['current key'] = $step;
		return array(
			'subject' => t('Account Checklist'),
			'content' => theme('afo_backend_checklist', $steps, $options),
		);
	}
	else return null;
}

/**
 * theme checklist
 */
function theme_afo_backend_checklist($items, $options = array()) {

	$content .= '<div class="afo-checklist">';

	foreach($items as $k => $item) {
		if($k == $options['current key']) $cls = 'current';
		else $cls = '';
		$content .= '<div class="afo-checklist-item '.$cls.'"><div class="afo-checklist-item-inner">';
		$content .= '<div class="title">'.t('@k. @title', array('@k'=>$k, '@title'=>$item['title'])).'</div>';
		$content .= '<div class="description">'.t($item['description']).'</div>';
		$content .= '</div></div>';
	}

	$content .= '</div>';
	
	return $content;
}

/**
 * Confirm email and email confirmed instructions pages
 * User will see this page after registering, and after confirming email.
 */
function afo_backend_confirm_email_page() {

	global $user;

	// anonymous user sees Confirm Email instructions
	if($user->uid == 0) {
		$content['instructions'] = array(
			'#type' => 'afo_contentbox',
			'#value' => t('A message has been sent to your to confirm your email address. Just follow the instructions in that message, and you\'ll be able to log into your new account.'),
		);
	}
	
	// authenticated user sees What's Next message
	else {
		
		drupal_set_title(t('Welcome to Local Artists!'));
		
		$content['instructions'] = array(
			'#type' => 'afo_contentbox',
			'#value' => t('You have successfully created your account. <br /><b>IMPORTANT: </b>').l(t('You must now set your password.'), 'user/'.$user->uid.'/edit'),
		);

		/***** mini control panel ****/		
		/*
		$cp['options'] = array(
			'#title' => t('Get Started'),
		);	
		$cp['options']['add-artwork'] = array(
			'#title' => t('Add Artwork'),
			'#href' => 'node/add/artwork',
			'#access' => 'create artwork content',
			'#icon' => 'add artwork',
		);
		$cp['options']['edit-profile'] = array(
			'#title' => t('Edit profile'),
			'#href' => 'user/me/edit/Profile',
			'#icon' => 'profile',
		);
		$cp['options']['edit-cv'] = array(
			'#title' => t('Edit CV'),
			'#href' => 'user/me/cv/edit',
			'#access' => 'edit own cv',
			'#icon' => 'cv',
		);	
		$cp['options']['membership'] = array(
			'#title' => t('Upgrade Membership'),
			'#href' => 'membership',
			'#icon' => 'membership',
		);
		*/
		
		$cp['options'] = array(
			'#title' => t('Set Password'),
		);	
		$cp['options']['edit-account'] = array(
			'#title' => t('Account Settings'),
			'#href' => 'user/me/edit',
			'#icon' => 'settings',
		);

		
		$content['control-panel'] = array(
			'#type' => 'afo_contentbox',
			'#value' => theme('afo_backend_control_panel', $cp),
			'#weight' => 1,
		);
		
	}
	
	return drupal_render($content);
}

