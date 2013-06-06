<?php
// $Id$

/**
 * @file
 *
 * AFO Membership editor pages.
 */

/********************************** editor functions **/

/**
 * Build payments page
 */
function afo_member_editor_payments() {

	$args = $_GET;
	$payments_data = afo_member_get_payments($args);
	
	$content['create-payment'] = array(
		'#type' => 'fieldset',
		'#title' => t('Create payment record manually'),
		'#weight' => -1,
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$content['create-payment']['add-payment'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Create payment record manually'),
		'#value' => drupal_get_form('afo_member_create_payment_form'),
	);

	$content['search-payments'] = array(
		'#type' => 'fieldset',
		'#title' => t('Search payments'),
		'#weight' => -1,
		'#collapsible' => true,
		'#collapsed' => true,
	);
	$content['search-payments']['search-form'] = array(
		'#value' => drupal_get_form('afo_member_search_payments_form'),
		'#weight' => 1,
	);
	
	$content['payments-table'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Member Payments'),
		'#value' => theme('afo_member_payments_table', $payments_data, array('show user'=>true, 'edit'=>true)),
		'#weight' => 3,
	);

	
	return drupal_render($content);
}

/**
 * Add payment manually form
 */
function afo_member_create_payment_form($form_state, $payment_id = null) {

	if(is_numeric($payment_id)) {
		$payment_data = afo_member_get_payments(array('payment_id'=>$payment_id));
		if(is_object($payment_data[0])) {
			$payment_data = $payment_data[0];
			
			$form['payment_id'] = array(
				'#type' => 'value',
				'#value' => $payment_data->payment_id,
			);

			$form['original_uid'] = array(
				'#type' => 'value',
				'#value' => $payment_data->uid,
			);

		}
	}

	$form['#redirect'] = 'editor/payments';

	$form['user_name'] = array(
		'#type' => 'textfield',
		'#title' => t('User name'),
		'#description' => t('Enter the user account to add the payment to.'),
		'#default_value' => $payment_data->name,
		'#autocomplete_path' => 'user/autocomplete',
		'#required' => false,
	);

	if(!is_numeric($payment_id)) {
		$form['user_emails'] = array(	
			'#type' => 'textarea',
			'#title' => t('OR batch of user emails'),
			'#description' => t('To add a payment to multiple users, paste a list of their emails, one on each line.'),
			'#rows' => 4,
		);
	}	
	
	$form['extend_membership'] = array(
		'#type' => 'textfield',
		'#title' => t('Extend membership (days)'),
		'#description' => t('Number of days this payment will extend the membership.'),
		'#default_value' => $payment_data->extend_membership,
		'#required' => true,
	);
	$form['payment_total'] = array(
		'#type' => 'textfield',
		'#title' => t('Payment total (USD)'),
		'#description' => t('Total payment amount in US dollars.'),
		'#default_value' => $payment_data->payment_total,
		'#required' => true,
	);	
	$status_options = afo_member_payment_status_settings();
	$form['payment_status'] = array(
		'#type' => 'radios',
		'#title' => t('Payment status'),
		'#description' => t('Status of the payment.'),
		'#options' => $status_options,
		'#default_value' => $payment_data->payment_status,
		'#required' => true,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Save',
	);

	if(is_numeric($payment_id)) {	
		$form['delete'] = array(
			'#type' => 'submit',
			'#value' => 'Delete payment',
		);
	}	
	
	return $form;

}
function _afo_member_create_payment_form_validate($form, &$form_state) {
	if(!empty($form_state['values']['user_name'])) {
		// check user field
		$user_account = user_load(array('name'=>$form_state['values']['user_name']));
		if($user_account) {
			$form_state['values']['uid'] = $user_account->uid;
		}
		else {
			form_set_error('user_name', t('Unable to find user'));
		}
	}
	
	// check numeric fields
	$numeric_fields = array('extend_membership', 'payment_total', 'payment_status');
	foreach($numeric_fields as $k) {
		if(isset($form_state['values'][$k]) && !is_numeric($form_state['values'][$k])) {
			form_set_error($k, t('Value must be a number.'));
		}
	}
}
function _afo_member_create_payment_form_submit($form, &$form_state) {
	if(!user_access('administer payments')) return false;

	$payment['payment_id'] = is_numeric($form_state['values']['payment_id']) ? $form_state['values']['payment_id'] : null;
	$payment['extend_membership'] = $form_state['values']['extend_membership'];
	$payment['payment_total'] = $form_state['values']['payment_total'];
	$payment['payment_status'] = $form_state['values']['payment_status'];

	
	// add delete op if that was button clicked
	if($form_state['clicked_button']['#id'] == 'edit-delete') {
		afo_member_add_payment(array('op'=>'delete', 'payment_id'=>$payment['payment_id']));
	}
	
	else {

		if(is_numeric($form_state['values']['uid'])) {
			// single user
			$payment['uid'] = $form_state['values']['uid'];
	
			// add/update payment record
			afo_member_add_payment($payment);
			
			// update user role to reflect new payment
			afo_member_update_role($payment['uid']);
	
			// if original uid different than payment uid (payment user changed), update original user role too
			if(is_numeric($form_state['values']['original_uid']) && $form_state['values']['original_uid'] != $payment['uid']) {
				afo_member_update_role($form_state['values']['original_uid']);
			}	
		}
		
		else if(!empty($form_state['values']['user_emails'])) {
			// batch of emails
			$emails = $form_state['values']['user_emails'];
			$emails = explode("\n", $emails);
	
			foreach($emails as $email) {
				$email = trim($email);
				$uid = null;
				$uid = db_result(db_query('SELECT uid FROM {users} WHERE mail = \'%s\'', $email));
				if(!is_numeric($uid)) drupal_set_message('Could not find user account for email: '.$email, 'error');
				else {
					$payment['uid'] = $uid;
	
					// add/update payment record
					afo_member_add_payment($payment);
					
					// update user role to reflect new payment
					afo_member_update_role($payment['uid']);
				}
			}
			
		}
		
	}

}

/**
 * Build search payments form
 */
function afo_member_search_payments_form() {

	$form['user_name'] = array(
		'#type' => 'textfield',
		'#title' => t('User name'),
		'#description' => t('Enter a portion of the user\'s name.'),
		'#default_value' => $_GET['name'],
		'#autocomplete_path' => 'user/autocomplete',
		'#required' => false,
	);

	$form['email'] = array(
		'#type' => 'textfield',
		'#title' => t('User email'),
		'#description' => t('Enter the user\'s email address.'),
		'#default_value' => $_GET['email'],
		'#required' => false,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Search',
	);
	
	return $form;
}
function _afo_member_search_payments_form_submit($form, &$form_state) {
	$name = strlen($form_state['values']['user_name']) > 0 ? $form_state['values']['user_name'] : null;
	$email = strlen($form_state['values']['email']) > 0 ? $form_state['values']['email'] : null;

	$name = filter_xss($name, array());	
	$email = filter_xss($email, array());	

	$name = trim($name);
	$email = trim($email);
	
	drupal_goto('editor/payments', "name=$name&email=$email");
}




