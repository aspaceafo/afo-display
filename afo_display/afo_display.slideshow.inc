<?php
// $Id$

/**
 * @file
 *
 * Create slide show.
 */

/**
 * Assemble and return slide show
 */
function theme_afo_display_slideshow($nids, $title=null, $op='artwork', $options=array()) {
	if(!is_array($nids)) return null;

	// get slideshow data
	module_load_include('inc', 'afo_display', 'afo_display.data');
	$nodes = afo_display_data_get_slides($nids, $op);
	if(!is_array($nodes)) return null;
	
	// add includes for slide show
	if(count($nids) > 1) {
		drupal_add_js(drupal_get_path('module', 'afo_display').'/slideshow.js');
	}
	drupal_add_css(drupal_get_path('module', 'afo_display').'/slideshow.css');
	jquery_ui_add(array('effects.core', 'effects.slide'));
	
	// build slide show
	$content = afo_display_build_slideshow($nodes, $title, $options);

	return $content;
}
 
/**
 * Build slide show.
 */

function afo_display_build_slideshow($nodes, $title=null, $options=array()) {
	if(!is_array($nodes)) return null;

	$images_folder = drupal_get_path('module', 'afo_display').'/images/';

	// if no imagecache_preset, set default
	if(!isset($options['imagecache_preset'])) $options['imagecache_preset'] = 'artwork_full';

	if(isset($options['id'])) $addId = 'id="'.$options['id'].'"';
	if(isset($options['imagecache_preset'])) $options['classes'][] = 'afo-slideshow-'.$options['imagecache_preset']; 
	if(is_array($options['classes'])) $addClasses = implode(' ', $options['classes']);
	$content .= '<div '.$addId.' class="afo-slideshow '.$addClasses.'"><div class="afo-slideshow-inner">';
	
	if($title) {
		$content .= '<div class="afo-slideshow-title">'.$title.'</div>';
	}

	$content .= '<div class="afo-slide-controls">';
		$content .= '<div class="afo-slide-button afo-slide-button-previous"><a href="#">';
		$content .= theme('image', $images_folder.'icon_bigarrow_left.png', t('previous'), t('previous slide'));
		$content .= '</a></div>';
		$content .= '<div class="afo-slide-button afo-slide-button-play"><a href="#">';
		$content .= theme('image', $images_folder.'icon_bigarrow_pause.png', t('pause'), t('pause slide show'), array('class'=>'pause'));
		$content .= theme('image', $images_folder.'icon_bigarrow_play.png', t('play'), t('play slide show'), array('class'=>'play hide'));
		$content .= '</a></div>';
		$content .= '<div class="afo-slide-button afo-slide-button-next"><a href="#">';
		$content .= theme('image', $images_folder.'icon_bigarrow_right.png', t('next'), t('next slide'));
		$content .= '</a></div>';
	$content .= '</div>';
	
	$nodes[0]['classes'][] = 'current';

	foreach($nodes as $n) {
		$content .= afo_display_build_slide($n, $options);
	}

	$content .= '</div>';
	$content .= '<div class="horiz-clear">&nbsp;</div>';
	$content .= '</div>';

	return $content;
}

/**
 * Build slide
 */
function afo_display_build_slide($n, $options=array()) {
	if(!is_array($n)) return null;

	$images_folder = drupal_get_path('module', 'afo_display').'/images/';
	
	// open slide
	if(is_array($n['classes'])) $classes = implode(' ', $n['classes']);
	$slide .= "<div class='afo-slide $classes'><div class='afo-slide-inner'>";

		if($n['image']) {
			// image
			$slide .= '<div class="afo-slide-image">';

				// uncomment to set image link to node id 
				//if(!$n['path'] && is_numeric($n['nid'])) $n['path'] = 'node/'.$n['nid'];
				
				// queue class
				if(module_exists('afo_queue')) $queue_class = afo_queue_queueable_classes($n['nid'], 'node');
								
				$image = theme('imagecache', $options['imagecache_preset'], $n['image'], '', '', array('class'=>$queue_class));
				if($n['path']) $slide .= l($image, $n['path'], array('html'=>'true'));
				else $slide .= $image;

			$slide .= '</div>';
		}
		
		// info
		if(module_exists('afo_portfolio')) {
			// artist name
			if(is_numeric($n['uid'])) $n['artist'] = l($n['name'], 'user/'.$n['uid']);
			else $n['artist'] = $n['name'];
			
			// title
			if(is_numeric($n['nid'])) $n['title'] = l($n['title'], 'node/'.$n['nid']);
			
			$slide .= '<div class="afo-slide-info">';
			$slide .= theme('afo_portfolio_artwork_listing', $n);
			$slide .= '</div>';
		}
		
	// close slide
	$slide .= '</div></div>';

	return $slide;
}