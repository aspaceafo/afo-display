<?php
// $Id$

/**
 * @file
 *
 * View options display functions
 */
 
/**
 * Theme view options function
 */
function theme_afo_display_view_options($options) {
	if(!is_array($options)) return null;
	
	$images_folder = drupal_get_path('module', 'afo_display').'/images/';

	$option_types['slides'] = array(
		'image' => 'icon_view_slides.png',
		'title' => t('View as slide show'),
		'id' => 'afo-viewoption-slides',
	);
	$option_types['grid'] = array(
		'image' => 'icon_view_grid.png',
		'title' => t('View as grid'),
		'id' => 'afo-viewoption-grid',
	);
	$option_types['fullscreen'] = array(
		'image' => 'icon_view_expand.png',
		'title' => t('View full screen'),
		'id' => 'afo-viewoption-fullscreen',
	);
	$option_types['details'] = array(
		'image' => 'icon_view_details.png',
		'title' => t('View Details'),
		'id' => 'afo-viewoption-details',
	);
	
	foreach($options as $option) {
		if(isset($option_types[$option['#type']])) {
		
			$type = $option_types[$option['#type']];

			// button content
			if(isset($type['image'])) $button = theme('image', $images_folder.$type['image'], $type['title'], $type['title']);
			else $button = $type['title'];

			if($option['#type'] == 'fullscreen' && is_array($option['#nids'])) {
				$fullscreen = '<span id="portfolio-fullscreen">';
				foreach($option['#nids'] as $k => $nid) {
					$filepath = db_result(db_query('SELECT files.filepath FROM {files} LEFT JOIN {content_type_artwork} artwork ON artwork.field_artwork_image_fid = files.fid WHERE artwork.nid = %d', $nid));
					if($filepath) {
						if($k > 0) $img_class = 'lightbox_hide_image';
						else $img_class = '';
						$fullscreen .= l($button, $filepath, array('html'=>true, 'attributes'=>array('rel'=>'lightbox[fullscreen]', 'class'=>$img_class))); 
					}
				}
				$fullscreen .= '</span>';
				$buttons[] = $fullscreen;
			
			} else {
				// link attributes
				$attributes = array(
					'id' => $type['id'],
					'class' => $type['class'],
				);
				
				// add classes from option array
				if(is_array($option['#class'])) $attributes['class'] .= ' '.implode(' ', $option['#class']);
				
				$variables = 'target='.$option['#target'];
				$variables .= '&close='.$option['#close'];
	
				// set button link
				$buttons[] = l($button, null, array('html' => true, 'query' => $variables, 'attributes' => $attributes));
			}
		}
	}
		
	if(is_array($buttons)) {

		drupal_add_js(drupal_get_path('module', 'afo_display').'/view_options.js');
		
		$content .= '<div class="afo-viewoptions js-show"><div class="afo-viewoptions-inner">';
		$content .= implode('&nbsp;&nbsp;', $buttons);
		$content .= '</div></div>';

	}	

	return $content;
}