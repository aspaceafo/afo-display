<?php
// $Id$

/**
 * @file
 *
 * Queue page functions.
 */
 
 /**
 * Queue page
 */
function afo_queue_page() {
	
	$queue_data = afo_queue_build_queue(array('no wrapper'=>'true', 'page view'=>true, 'data only'=>true));
	
	$form['#tree'] = true;

	/************** Checkbox controls **/	
	$controls_content .= l(t('select all'), '#', array('query'=>'target=.queueItem&op=checkAll', 'attributes'=>array('class'=>'checkboxControl js-show')));
	$controls_content .= ' | ';
	$controls_content .= l(t('select none'), '#', array('query'=>'target=.queueItem&op=checkNone', 'attributes'=>array('class'=>'checkboxControl js-show')));	
	$form['controls'] = array(
		'#type' => 'afo_contentbox',
		'#value' => $controls_content,
		'#weight' => -2,
	);	

	/************** Queue items **/
	if(is_array($queue_data)) {
		foreach($queue_data as $i) {
			$form['queue'][$i['type'] . '-' . $i['id']] = array(
				'#type' => 'checkbox',
				'#display' => 'table',
				'#prefix' => '<div class="afo-queue-item">' . theme('afo_queue_item', $i),
				'#suffix' => '</div>',
				'#attributes' => array('class' => 'queueItem'),
			);
		}
	}
	else {
		drupal_set_message($queue_data, 'warning');
	}
	
	/************** Option buttons **/
	
	$form['options'] = array(
		'#type' => 'afo_contentbox',
		'#prefix' => '<div class="horiz-clear">&nbsp;</div>',
		'#weight' => 5,
	);

	$exhibition_type = variable_get('afo_settings_types_exhibition', 'exhibition');
	if(user_access("create $exhibition_type content")) {
		$form['options']['add-to-exhibition'] = array(
			'#type' => 'submit',
			'#name' => 'add-to-exhibition',
			'#value' => t('Add Selected to Exhibition'),
		);
	}

	if(module_exists('afo_favorite') && user_access('save favorites')) {
		$form['options']['add-to-favorites'] = array(
			'#type' => 'submit',
			'#name' => 'add-to-favorites',
			'#value' => t('Add Selected to Favorites'),
		);
	}
	
	$form['options']['remove'] = array(
		'#type' => 'submit',
		'#name' => 'remove',
		'#value' => t('Remove Selected'),
	);

	
	return $form;
}

 /**
 * Queue page submit
 */
function _afo_queue_page_submit($form, &$form_state) {

	global $user;

	foreach($form_state['values']['queue'] as $item_key => $val) {
		if($val == 1) {
			$item_key = explode('-', $item_key);
			$selected_items[] = array(
				'type' => $item_key[0],
				'id' => $item_key[1],
			);
		}
	}

	switch($form_state['clicked_button']['#name']) {

		case 'remove':
			foreach($selected_items as $i) {
				foreach($_SESSION['afo_queue'] as $k => $q) {
					if($i['type'] == $q['type'] && $i['id'] == $q['id']) {
						unset($_SESSION['afo_queue'][$k]);
					}
				}
			}
			break;
			
		case 'add-to-favorites':
			if(module_exists('afo_favorite') && user_access('save favorites')) {
				foreach($selected_items as $i) {
					afo_favorite_add($user->uid, $i['id'], $i['type']);
				}
			}
			break;

		case 'add-to-exhibition':
			$exhibition_type = variable_get('afo_settings_types_exhibition', 'exhibition');
			if(user_access("create $exhibition_type content")) {
				foreach($selected_items as $i) {
					if($i['type'] == 'node') $nids[] = $i['id'];
				}
				if(count($nids) > 0) {
					drupal_goto('exhibition/add-form/'.implode('-', $nids));
				}
			}
			break;
			
	}
}