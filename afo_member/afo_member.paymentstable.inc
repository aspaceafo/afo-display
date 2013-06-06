<?php
// $Id$

/**
 * @file
 *
 * AFO Membership payments table.
 */

/**
 * Theme table function
 */
function theme_afo_member_payments_table($data, $args = array()) {
	if(!is_array($data)) return null;
	
	$payment_status = afo_member_payment_status_settings();

	if($args['edit']) $headers[] = '';
	if($args['show user']) {
		$headers[] = t('User');	
		$headers[] = t('Email');	
	}
	$headers[] = t('Date');
	$headers[] = t('Payment');
	if($args['edit']) {
		$headers[] = t('UC Order Total');
		$headers[] = t('UC Order Id');
	}
	$headers[] = t('Status');
	$headers[] = t('Membership Extended (days)');
	
	foreach($data as $row) {
		$table_row = array();

		if($args['edit']) $table_row[] = l(t('edit'), 'editor/payment/edit/'.$row->payment_id);
		if($args['show user']) {
			$table_row[] = l($row->name, 'user/'.$row->uid);
			$table_row[] = $row->mail;
		}
		$table_row[] = date('j M Y', $row->timestamp);
		$table_row[] = '$' . $row->payment_total . t(' USD');
		if($args['edit']) {
			$table_row[] = round($row->order_total, 2);
			$table_row[] = $row->payment_order_id > 0 ? l($row->payment_order_id, 'admin/store/orders/'.$row->payment_order_id) : '';
		}
		$table_row[] = t($payment_status[$row->payment_status]);
		$table_row[] = $row->extend_membership;
	
		$table_rows[] = array(
			'data' => $table_row,
			'class' => $payment_status[$row->payment_status],
		);
	}
	
	$content = theme('table', $headers, $table_rows, array('class'=>'afo-member-payments-table'));

	return $content;
}