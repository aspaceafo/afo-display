<?php
// $Id$

/**
 * @file
 *
 * AFO Grid display.
 */
 
/**
 * Grid theme function.
 */
function theme_afo_display_grid($items, $title = null, $op = null, $options = array()) {
	if(!is_array($items)) return null;
	
	if(is_numeric($items[0])) {
		// if nids instead of full arrays, get node data
		module_load_include('inc', 'afo_display', 'afo_display.data');
		$items = afo_display_data_get_slides($items, $op);
	}
	
	if(!is_array($items[0])) return null;
	
	drupal_add_css(drupal_get_path('module', 'afo_display').'/grid.css');
	drupal_add_js(drupal_get_path('module', 'afo_display').'/grid.js');

	if(isset($options['id'])) $addId = 'id="'.$options['id'].'"';
	
	if($options['expandable']) $options['classes'][] = 'expandable no-roll';
	elseif($options['no rollover']) $options['classes'][] = 'no-roll';
	
	if(is_array($options['classes'])) $addClasses = implode(' ', $options['classes']);
	$content .= '<div '.$addId.' class="afo-grid '.$addClasses.'"><div class="grid-inner">';

	if($title) $content .= '<h3 class="grid-title">' . check_plain($title) . '</h3>';
	
	foreach($items as $item) {
		$content .= afo_display_grid_item($item, $op, $options);
	}
	
	$content .= '</div><div class="horiz-clear">&nbsp;</div></div>';
	
	return $content;
}

/**
 * Print grid item.
 */
function afo_display_grid_item($data, $op = 'artwork', $options = array()) {
	$type_class = str_replace(' ', '_', $type);
	$textmax = 600;
	
	$title_length = 40;
	if(strlen($data['title']) > $title_length) {
		$data['title'] = substr($data['title'], 0, $title_length);
		$data['title'] .= '...';
	}
	
	global $base_path;
	
	// --------------------------------------------- create item array from data array based on type
	
	if($data['buttons']) $item['buttons'] = $data['buttons'];
	
	if($data['type'] == 'label') {
		$item['label-title'] = array(
			'#content' => $data['label'],
			'#class' => 'no-roll',
		);
	}
	else {
	
		if($op == 'artwork' ) {
			$item_id = 'item-'.$data['nid'];
			$item_dest = 'node/'.$data['nid'];
			$item['thumbnail'] = theme('afo_display_node_thumbnail', $data, null, null, $options);
			if($options['expandable']) {
				$item_expanded['image-expanded'] = array(
					'#content' => theme('afo_display_node_thumbnail', $data, 'artwork_medium', null, array('no label'=>true)),
				);
				//$item_expanded['title'] = l($data['title'], 'node/'.$data['nid']);
				//$item_expanded['user'] = l($data['name'], 'user/'.$data['uid']);
				$item_expanded['title'] = "<a href='{$base_path}$item_dest'>{$data[title]}</a>";
				$item_expanded['user'] = "<a href='{$base_path}user/{$data[uid]}'>{$data[name]}</a>";
			}
			else {
				//$item['title'] = l(_afo_display_grid_trimText($data['title'], $options), $item_dest);
				//$item['user'] = l(_afo_display_grid_trimText($data['name'], $options), 'user/'.$data['uid']);
				$item['title'] = "<a href='{$base_path}$item_dest'>{$data[title]}</a>";
				$item['user'] = "<a href='{$base_path}user/{$data[uid]}'>{$data[name]}</a>";

			}
		}
	
		else if($op == 'artists' || $op == 'members') {
			$item_id = 'item-'.$data['uid'];
			$item_dest = 'user/'.$data['uid'];
			if(is_numeric($data['thumbnail_nid'])) $options['thumbnail_nid'] = $data['thumbnail_nid'];
			$item['thumbnail'] = theme('afo_display_user_thumbnail', $data, null, null, $options);
			if($options['expandable']) {
				$item_expanded['image-expanded'] = array(
					'#content' => theme('afo_display_user_thumbnail', $data, 'artwork_medium', null, array('no label'=>true)),
				);
				//$item_expanded['user'] = l($data['name'], $item_dest);
				$item_expanded['user'] = "<a href='{$base_path}$item_dest'>{$data[name]}</a>";
			}
			else {
				//$item['user'] = l($data['name'], $item_dest);
				$item['user'] = "<a href='{$base_path}$item_dest'>{$data[name]}</a>";
			}
		}
	
		else if($op == 'exhibitions') {
			$item_id = 'item-'.$data['nid'];
			$item_dest = 'node/'.$data['nid'];
			$item['thumbnail'] = afo_exhibition_thumbnail($data['nid'], $options);
			if($options['expandable']) {
				$item_expanded['image-expanded'] = array(
					'#content' => theme('afo_display_node_thumbnail', $data['nid'], 'artwork_medium', null, array('no label'=>true)),
				);
				$item_expanded['title'] = l($data['title'], $item_dest);
				$item_expanded['user'] = 'Curated by '.l($data['name'], 'user/'.$data['uid']);
			}
			$item['title'] = l($data['title'], $item_dest);
			$item['user'] = 'Curated by '.l($data['name'], 'user/'.$data['uid']);
		}
	
		else if($op == 'open calls') {
			$item_id = 'item-'.$data['nid'];
			$item_dest = 'node/'.$data['nid'];
			$item['#classes'][] = 'text-wide';
			$item['thumbnail'] = theme('afo_display_node_thumbnail', $data['nid'], 'artwork_medium', null, $options);
			$item['title'] = l($data['title'], $item_dest);
			$item['deadline'] = theme('afo_display_deadline', $data['deadline']);
			$item['user'] = l($data['name'], 'user/'.$data['uid']);
			$item['body'] = substr($data['body'], 0, $textmax);
			if(strlen($data['body']) > $textmax) $item['body'] .= '...';
		}
		
	}
	
	if(!is_array($item)) return null;

	// --------------------------------------------- Start Item

	if(is_array($item['#classes'])) $item_classes = implode(' ', $item['#classes']);
	$item_display = "<div id='$item_id' class='grid-item op-$op $type_class $item_classes'><div class='item-inner'>";

		// --------------------------------------------- Normal display
		$item_display .= '<div class="normal-view">';

		if(is_array($item['buttons'])) $buttonsArray = $item['buttons'];
		elseif(is_array($options['buttons'])) $buttonsArray = $options['buttons'];

		// --------------------------------------------- Item Buttons
		if($buttonsArray) {
			// op buttons
			$images_folder = $base_path.drupal_get_path('module', 'afo_display').'/images/';
			$button_images = array(
				'delete' => 'X.png',
				'add' => 'plus.png',
			);
						
			$item_display .= '<div class="imageButtons">';
			foreach($buttonsArray as $button_name => $button_op) {

				if(!is_array($button_op)) {
					$button_class = $button_op;
				} else {
					$button_url = $button_op['url'];
					$button_class = $button_op['class'];
				}

				$imgUrl = $images_folder.$button_images[$button_name];
				$button_image = "<img src='$imgUrl' alt='$button_name' title='$button_name' class='$button_class' />";
				
				if($button_image) {
					$item_display .= '<div class="'.$button_name.'-button">';
					$item_display .= l($button_image, $button_url, array('html'=>true));
					$item_display .= '</div>';
				}
				
			}
			$item_display .= '</div>';
		}

		// --------------------------------------------- print the item content

		$item_display .= _afo_display_grid_printItems($item, $options);
		
		$item_display .= '</div>';

		// --------------------------------------------- expanded display	
		
		if($options['expandable'] && is_array($item_expanded) && $item_dest) {
		
			// queue class
			if(module_exists('afo_queue')) {
				if($op == 'artists') { $queue_id = $data['uid']; $queue_op = 'user'; }
				else { $queue_id = $data['nid']; $queue_op = 'node'; }
				$queue_class = afo_queue_queueable_classes($queue_id, $queue_op);
			}
			
			// assemble expanded display
			$item_display .= '<div class="expanded-view hide expand">';
			//$item_display .= l("<div class='hit-area $queue_class'>&nbsp;</div>", $item_dest, array('html'=>true));
			$item_display .= "<a href='{$base_path}$item_dest'><div class='hit-area $queue_class'>&nbsp;</div></a>";
			$item_display .= _afo_display_grid_printItems($item_expanded, $options);
			$item_display .= '<div class="horiz-clear">&nbsp;</div>';
			$item_display .= '</div>';
		}		
	
	$item_display .= "</div></div>";
	
	return $item_display;
}
/**
 * Print grid items helper function
 */
function _afo_display_grid_printItems($item, $options=array()) {
	if(!is_array($item)) return null;
	
	foreach($item as $label => $item_content) {
		if($item_content && substr($label,0,1)!='#') {
			
			// define toggle class
			if($label != 'thumbnail' && !$options['expandable']) $toggleClass = 'rollToggle';
			else $toggleClass = '';
			
			if(!is_array($item_content)) {
				$item_display .= "<div class='item-$label $toggleClass'>";
				$item_display .= $item_content;
				$item_display .= "</div>";
			}
			else {
				$item_display .= '<div class="'.$toggleClass.' item-'.$label.' '.$item_content['#class'].'">';
				$item_display .= $item_content['#content'];
				$item_display .= "</div>";				
			}
		}
	}
	return $item_display;
}
/**
 * trim text helper function
 */
function _afo_display_grid_trimText($value, $options=array()) {
	// trim text if not expandable or roll-over
	if(!$options['expandable'] && $options['no rollover']) {
		
		$maxLen = 16;
		
		if(strlen($value) > $maxLen) {
			$value = substr($value, 0, $maxLen-3);
			$value .= '...';
		}
	}
	
	return $value;
}