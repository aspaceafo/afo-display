<?php
// $Id$

/**
 * @file
 *
 * AFO Table display.
 */
 
/**
 * Table theme function.
 */
function theme_afo_display_table($items, $title = null, $op = null, $options = array()) {
	if(!is_array($items)) return null;
	
	if(is_numeric($items[0])) {
		// if nids instead of full arrays, get node data
		module_load_include('inc', 'afo_display', 'afo_display.data');
		$items = afo_display_data_get_slides($items, $op);
	}

//	if(!is_array($items[0])) return null;
	
	if(isset($options['id'])) $addId = 'id="'.$options['id'].'"';
	if($options['#expandable']) $options['classes'][] = 'expandable';
	if(is_array($options['classes'])) $addClasses = implode(' ', $options['classes']);
	$content .= '<div '.$addId.' class="afo-table '.$addClasses.'"><div class="table-inner">';

	if($title) $content .= '<h3 class="grid-title">' . check_plain($title) . '</h3>';

	$content .= '<table>';
	
	foreach($items as $item) {
		$content .= '<tr>';
		$content .= afo_display_tablerow($item, $op, $options);
		$content .= '</tr>';
	}
	
	$content .= '</table>';
	
	$content .= '</div></div>';
	
	return $content;
}

/**
 * Print table row
 */
function afo_display_tablerow($data, $op, $options) {
	$type_class = str_replace(' ', '_', $type);
	//$textmax = 600;
	
	// if object, convert to array
	if(is_object($data)) $data = (array) $data;

	if(!is_array($data)) return null;

	// assemble item
	switch($op) {

		case 'open calls':
			$item_id = 'item-'.$data['nid'];
			$item_dest = 'node/'.$data['nid'];

			$item['col1']['thumbnail'] = theme('afo_display_node_thumbnail', $data['nid'], 'artwork_thumbnail', null, array('no label'=>true));

			$item['col2']['title'] = l($data['title'], $item_dest);
			$item['col2']['deadline'] = theme('afo_display_deadline', $data['deadline']);
			$item['col2']['user'] = l($data['name'], 'user/'.$data['uid']);
			$item['col2']['body'] = theme('afo_display_text', $data['body'], array('read more' => $item_dest));

			break;

		case 'events':
			$item_id = 'item-'.$data['nid'];
			$item_dest = 'node/'.$data['nid'];

			$item['col1']['thumbnail'] = theme('afo_display_node_thumbnail', $data['nid'], 'artwork_thumbnail', null, array('no label'=>true));

			$item['col2']['title'] = l($data['title'], $item_dest);
			
			$date = format_date($data['date'], 'custom', variable_get('afo_display_dateformats_short', 'j M'));
			$date_end = format_date($data['date_end'], 'custom', variable_get('afo_display_dateformats_short', 'j M'));
			if($date_end != $date) $date .= ' - ' . $date_end; 
			$item['col2']['date'] = $date;

			$item['col2']['city'] = l($data['city'], 'city/'.$data['citytid']);
			$item['col2']['user'] = l($data['username'], 'user/'.$data['uid']);

			break;
	
	}
	
	// print item
	foreach($item as $key => $col) {
	
		$content .= "<td class='$key'>";

		foreach($col as $label => $i) {
			if($i && substr($label,0,1)!='#') {				
				if(!is_array($i)) {
					$content .= "<div class='item-$label'>";
					$content .= $i;
					$content .= "</div>";
				}
				else {
					$content .= '<div class="item-'.$label.' '.$i['#class'].'">';
					$content .= $i['#content'];
					$content .= "</div>";				
				}
			}		
		}

		$content .= '</td>';
	
	}
	
	return $content;	
}
