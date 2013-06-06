<?php
// $Id$

/**
 * @file
 *
 * AFO Membership page.
 */

/********************************** display functions **/

/**
 * Build page
 */
function afo_member_membership_page() {

	global $user;
	$status_info = afo_member_get_status($user->uid);
	$status_types = afo_member_membership_status_settings();
	$site_mode = variable_get('afo_member_site_mode', null);
	$purchase_nid = variable_get('afo_member_product_nid', null);
	
	$content['col-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => -1,
	);
	
	/********************* status **/
	// check if authenticated user
	if(key_exists(2, $user->roles)) {
		if(key_exists(variable_get('afo_custommenu_roles_artist_member', 'none'), $user->roles)) $user_roles[] = t('artist (member)');
		elseif(key_exists(variable_get('afo_custommenu_roles_artist', 'none'), $user->roles)) $user_roles[] = t('artist (free)');
		if(key_exists(variable_get('afo_custommenu_roles_curator', 'none'), $user->roles)) $user_roles[] = t('curator');

		/*
		switch($site_mode) {
		
			case 'beta':
				// BETA version:
				if($status_info['status'] == 1) $status_message[] = t('Current Status: Free Account'); 
								
				if($status_info['status'] > 1) {
					$status_message[] = t('Current Status: Beta Membership'); 
					$status_message[] = '<span style="color: red;">' . ('Note: Your account will not be upgraded until the end of BETA testing. At that time you will receive a notification that your artist member account is ready for use, and your 6-month membership will begin.') . '</span>'; 		
				}
				
				break;
				
			default:
			*/
				// Normal:
				$status_message[] = t('Current status: %status', array('%status' => $status_types[$status_info['status']])); 
				
				if(is_array($user_roles)) $status_message[] = t('Role: %roles', array('%roles' => implode(', ', $user_roles))); 
				
				if($status_info['membership ends'] > 0) {
					$status_message[] = t('Membership ends: %end', array('%end' => date('j M Y', $status_info['membership ends']))); 
				}
				
				if($status_info['membership pending'] > 0) {
					$status_message[] = t('Membership pending payment ends: %end', array('%end' => date('j M Y', $status_info['membership pending']))); 
				}
				if($purchase_nid && $site_mode == 'normal') {
					//if(node_access('view', $purchase_nid)) {
						// add "purchase membership" button
						$content['col-left']['purchase-button'] = array(
							'#type' => 'afo_contentbox',
							'#title' => t('Purchase / Extend Membership'),
							'#value' => t('Buy a one year membership with Local Artists, or extend your current membership by one year.<br />') . l(t('Purchase Membership'), 'node/'.$purchase_nid, array('attributes'=>array('class'=>'buttonLink'))),
							'#weight' => 2,
						);
					//}
				}
				/*
				break;
		
		}*/

		$content['col-left']['membership-status'] = array(
			'#type' => 'afo_contentbox',
			'#title' => t('Your Membership Status'),
			'#value' => theme('item_list', $status_message, null, 'ul', array('class'=>'arrow tightList')),
			'#weight' => -5,
		);

		/********************* payments **/
		$payments_data = afo_member_get_payments(array('uid' => $user->uid));
		if($payments_data) {
			$content['col-left']['membership-status']['view-payments']['payments'] = array(
				'#type' => 'fieldset',
				'#title' => t('View Payments'),
				'#collapsible' => true,
				'#collapsed' => true,
			);
			$content['col-left']['membership-status']['view-payments']['payments']['payments-table'] = array(
				'#value' => theme('afo_member_payments_table', $payments_data),
				'#weight' => 0,
			);
		}


		
	}
	else {
		// sign up button
		$sign_up_content .= t('Learn about user options and create an account:');
		$sign_up_content .= '<p>'; 
		$sign_up_content .= l(t('Sign Up'), 'sign-up', array('attributes'=>array('class'=>'buttonLink')));
		
		$content['col-left']['sign-up-button'] = array(
			'#type' => 'afo_contentbox',
			'#title' => t('Not a user?'),
			'#value' => $sign_up_content,
			'#weight' => -5,
		);
	}
	
	/*
	$content['col-left']['sample-member'] = array(
		'#type' => 'afo_contentbox',
		'#value' => l(t('See Sample Member Page'), 'user/3', array('attributes'=>array('class'=>'buttonLink'))),
		'#weight' => 1,
	);	
	*/
	
	if($site_mode == 'beta') {
		$content['col-left']['preview-button'] = array(
			'#type' => 'afo_contentbox',
			'#value' => afo_custommenu_preview_form(false),
			'#weight' => 2,
		);	
	}
	
	$selected_uids = array(7692, 10646, 1241, 5680, 7731, 5475, 4437, 8804);
	//$selected_uids = array(6, 8, 9, 10, 11, 12, 13, 14);
	if(is_array($selected_uids)) {
		$content['col-left']['sample-members'] = array(
			'#type' => 'afo_contentbox',
			'#title' => t('Selected Member Pages'),
			'#value' => theme('afo_display_grid', $selected_uids, null, 'artists', array('classes'=>array('tightGrid'))),
			'#weight' => 10,
		);
	}
	


	$content['col-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '600px',
		'#weight' => 0,
	);
	
	/********************* membership explanation **/

	$benefits_text .= afo_member_comparison_table();
	
	$content['col-right']['benefits'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Artist Membership Options'),
		'#value' => t($benefits_text),
		'#weight' => -2,
	);

	return drupal_render($content);
}