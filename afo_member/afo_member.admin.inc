<?php
// $Id$

/**
 * Admin callbacks for afo_member module.
 */
 
/**
 * Form builder for admin settings page.
 */
function afo_member_admin_settings() {

	$form['afo_member_site_mode'] = array(
		'#type' => 'radios',
		'#title' => 'Site mode',
		'#default_value' => variable_get('afo_member_site_mode', null),
		'#options' => array(
			'normal' => 'Normal',
			'beta' => 'Beta launch',
		),
	);

	$form['afo_member_extend_per_purchase'] = array(
		'#type' => 'textfield',
		'#title' => 'Days Extended per Purchase',
		'#description' => 'How many days should memberships be extended per purchase?',
		'#default_value' => variable_get('afo_member_extend_per_purchase', null),
	);

	$form['afo_member_product_nid'] = array(
		'#type' => 'textfield',
		'#title' => 'Purchase Membership nid',
		'#description' => 'Node nid of current membership product',
		'#default_value' => variable_get('afo_member_product_nid', null),
	);
	
	if($_GET['upgrade'] == 1) {		
		$q = db_query('SELECT p.uid, u.mail FROM afo_member_payments p LEFT JOIN users u ON u.uid = p.uid WHERE p.payment_id > 484');
		while($row = db_fetch_array($q)) {
			if($row['uid'] > 1) {
				$uids[] = $row['uid'];
				$emails[] = $row['mail'];
			}
		}
		
		$rid = variable_get('afo_custommenu_roles_artist_member', null);
		if(is_numeric($rid) && false) {
			user_multiple_role_edit($uids, 'add_role', $rid);
			dsm('Member user roles added.');
		}
		$email_list = implode(', ', $emails);
		dsm(array($email_list));
		dsm($uids);
	}

	return system_settings_form($form);
}