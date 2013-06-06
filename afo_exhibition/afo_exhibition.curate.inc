<?php
// $Id$

/**
 * @file
 *
 * Curator edit exhibition functions.
 */

/**
 * Alter exhibition node form.
 */
function afo_exhibition_formalter(&$form, $form_state) {

//	$form['#tree'] = true;
	$form['#submit'][] = 'afo_exhibition_formsubmit';

	$form['exhibition_artwork'] = array(
		'#type' => 'fieldset',
		'#title' => 'Exhibition Works',
		'#weight' => -5,
		'#theme' => 'afo_exhibition_itemstable',
		'#collapsible' => true,
		'#tree' => true,
	);

	// check if node already exists
	if(is_numeric($form['#node']->nid)) {

		$exhibition_items = $form['#node']->exhibition_items;

		if(is_array($exhibition_items)) {
			// yes items
			$explain_text = t('Drag to re-order artworks. Save this form to keep the changes.');
		
			foreach($exhibition_items as $item) {
				$row = array();
				$row['item_nid'] = array(
					'#type' => 'hidden',
					'#value' => $item->item_nid,
				);
				$row['item_weight'] = array(
					'#type' => 'hidden',
					'#default_value' => $item->item_weight,
				);
				$row['thumbnail'] = array(
					'#value' => theme('afo_display_node_thumbnail', $item->item_nid, 'artwork_tiny'),
				);			
				$row['title'] = array(
					'#value' => $item->title,
				);
				$row['author'] = array(
					'#value' => l($item->name, 'user/'.$item->uid),
				);
				$row['remove'] = array(
					'#type' => 'checkbox',
				);
				
				$form['exhibition_artwork']['items'][$item->item_nid] = $row;
			}
		}
		else {
			// no items
			$explain_text = t('To add artworks to this exhibition, navigate to an artwork page and click the "Add to Exhibition" button.');
		}
		
		$form['exhibition_artwork']['images_explain'] = array(
			'#value' => $explain_text,
			'#weight' => -10,
		);

		
	}
	
	// creating new exhibition node
	else {
		$form['exhibition_artwork']['images_explain'] = array(
			'#value' => t('After you create the exhibition, you will be able to add works to it. Navigate to an artwork and click the "Add to Exhibition" button.'),
		);
	}

}

/**
 * Theme exhibition node form.
 */
function theme_afo_exhibition_itemstable($items) {	

	$exhibition_items = $items['items'];

	drupal_add_tabledrag('afo-exhibition-items-table', 'order', 'self', 'item-weight');
	
	$rows = array();

	foreach (element_children($exhibition_items) as $key) {

		$element = $exhibition_items[$key];

		$row = array();

		$element['item_nid']['#attributes']['class'] = 'item-nid';
		$element['item_weight']['#attributes']['class'] = 'item-weight';

		// thumbnail
		$row[] = drupal_render($element['thumbnail']) ;
		// title
		$row[] = drupal_render($element['title']) ;
		// author
		$row[] = drupal_render($element['author']) ;		
		// remove
		$row[] = drupal_render($element['remove']) ;		
		// weight
		$row[] = drupal_render($element['item_weight']) . drupal_render($element['item_nid']);
		
		// put row
		$rows[] = array(
			'data' => $row,
			'class' => 'draggable',
		);
	}
		
	$header = array('', t('Artwork Item'), t('Artist'), t('Remove'), '');
	$output = theme('table', $header, $rows, array('id' => 'afo-exhibition-items-table'));
	$output .= drupal_render($items['images_explain']);
	
	return $output;
}

/**
 * Submit exhibition node form.
 */
function _afo_exhibition_formsubmit($form, &$form_state) {
	$exhibition_nid = $form['#node']->nid;
	if(!is_numeric($exhibition_nid)) return null;
	
	$items = $form_state['values']['exhibition_artwork']['items'];
	if(is_array($items)) {
		foreach($items as $item) {
			if($item['remove'] == 1 && is_numeric($item['item_nid']) && is_numeric($exhibition_nid)) {
				$removeResult = db_query('DELETE FROM {afo_exhibition_items} WHERE exhibition_nid = %d AND item_nid = %d LIMIT 1', $exhibition_nid, $item['item_nid']);
			}
			elseif(is_numeric($item['item_nid']) && is_numeric($item['item_weight'])) {
				$record = new stdclass;
				$record->exhibition_nid = $exhibition_nid;
				$record->item_nid = $item['item_nid'];
				$record->item_weight = $item['item_weight'];
				drupal_write_record('afo_exhibition_items', $record, array('exhibition_nid', 'item_nid'));
			}
		}
	}
}

