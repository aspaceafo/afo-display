<?php
// $Id$

/**
 * @file
 *
 * Create slides lightbox for AFO Display module.
 */

/**
 * Assemble and return lightbox
 */
function theme_afo_display_lightbox($nids, $title=null, $op='artwork', $options=array()) {
	if(!is_array($nids)) return null;

	// get slideshow data
	module_load_include('inc', 'afo_display', 'afo_display.data');
	$nodes = afo_display_data_get_slides($nids, $op);
	if(!is_array($nodes)) return null;

	// add includes for slide show
	drupal_add_js(drupal_get_path('module', 'afo_display').'/lightbox.js');
	drupal_add_css(drupal_get_path('module', 'afo_display').'/lightbox.css');
	jquery_ui_add(array('ui.draggable', 'ui.resizable', 'ui.dialog'));
	
	// build slide show
	$content = afo_display_build_lightbox($nodes, $options);
	
	return $content;
}

/**
 * Build slides lightbox.
 */
function afo_display_build_lightbox($nodes, $options=array()) {
	if(!is_array($nodes)) return null;
	
	// random grouping title if none given
	if(!$options['grouping']) $options['grouping'] = uniqid('lightbox');

	if(isset($options['id'])) $addId = 'id="'.$options['id'].'"';
	if(is_array($options['classes'])) $classes = implode(' ', $options['classes']);
	$content .= "<div $addId class='afo-slidelightbox $classes'><div class='afo-slidelightbox-inner'>";
	
	if($options['title']) {
		$content .= '<div class="afo-slidelightbox-title">'.$options['title'].'</div>';
	}
	
	// add slides
	$nodes[0]['classes'][] = 'current';
	foreach($nodes as $n) {
		$content .= afo_display_build_lightboxslide($n, $options);
	}

	$content .= '</div>';
	$content .= '</div>';

	return $content;
}

/**
 * Build lightbox slide
 */
function afo_display_build_lightboxslide($n, $options=array()) {
	if(!is_array($n)) return null;
	
	// open slide
	if(is_array($n['classes'])) $classes = implode(' ', $n['classes']);

	$slide .= "<div class='afo-slide $classes'><div class='afo-slide-inner'>";
	$slide .= theme('image', $n['image'], t('Click for next artwork'), t('Click for next artwork'), null, true); 
	$slide .= '</div></div>';
	
	return $slide;
}