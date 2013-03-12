<?php
// $Id$

/**
 * @file
 *
 * Display functions for index list
 */
 
/**
 * theme index function
 */
function theme_afo_index($items, $options = array()) {
	
	// add search box
	if($options['#autocomplete']) {
		$content['index-search'] = array(
			'#type' => 'afo_contentbox',
			'#weight' => -5,
			'#value' => drupal_get_form('afo_display_indexsearch', $options),
		);
	}
	
	/********************* alpha list **/
	// create columns
	if(!is_numeric($options['#columns'])) $options['#columns'] = 4; // default number of columns 
	$num_cols = $options['#columns'];
	for($c=1; $c<=$num_cols; $c++) {
		$content['alpha-list']["column-$c"] = array(
			'#type' => 'afo_column',
			'#weight' => $c,
			'#width' => round(100 / $num_cols) . '%',
		);
	}
	foreach($items as $item) {
		$item_link = $item['link'];
		if(isset($item['count'])) {
			$item_link .= ' <span class="secondaryText">(';
			$item_link .= $item['count'];
			if(isset($item['count term'])) $item_link .= ' '.t($item['count term']);
			$item_link .= ')</span>';
		}
		$alpha_items[$item['category']][] = $item_link;
	}
	$current_col = 1;
	$weight = -15;
	foreach($alpha_items as $letter => $item) {
		$content['alpha-list']["column-$current_col"]['city-list-'.$letter] = array(
			'#value' => "<h3>$letter</h3>".theme('item_list', $item, null, 'ul', array('class'=>'arrow tightList')),
			'#weight' => $weight,
			'#suffix' => '<br />',
		);
		
		$current_col ++;
		if($current_col > $num_cols) $current_col = 1;
		$weight ++;
	}
	
	/********************* cloud **/
	if(!$options['#no cloud']) {
		
		$font_size_max = 50;
		
		foreach($items as $item) {
			
			// font size
			$font_size = 8 + $item['weight'] / .8;
			if($font_size > $font_size_max) $font_size = $font_size_max;
			$font_size .= 'px';
			
			$cloud_content .= "<div style='font-size:$font_size;' class='afo-cloud-tag'>".$item['link'].'</div>&nbsp;';
		}
		
		$content['cloud'] = array(
			'#type' => 'afo_contentbox',
			'#value' => $cloud_content,
			'#weight' => -1,
			'#attributes' => array('class'=>'width-auto'),
		);
	
	}
	
	return drupal_render($content);
}

/**
 * Form builder for index search form
 */
function afo_display_indexsearch($form_state, $options) {

	$city_vid = variable_get('afo_profile_city_vid', null);
	if(!$city_vid) return null;

	// add submit function
	if($options['#submit']) $form['#submit'][] = $options['#submit']; 	

	$form['indexsearch'] = array(
		'#type' => 'textfield',
		'#title' => t('Search'),
		'#size' => 60,
		'#maxlength' => 250,
		'#autocomplete_path' => $options['#autocomplete'],
		'#display' => 'normal',
		'#weight' => -1,
		'#display' => 'table',
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#prefix' => '<div class="inline">',
		'#value' => t('Go'),
		'#suffix' => '</div>',
		'#weight' => 0,
		'#display' => 'normal',
	);
	
	return $form;
}
/**
 * Theme index search form
 */
function theme_afo_display_indexsearch($form) {

	$output .= '<table class="form-table" border="0">';
	$output .= '<tr>';
	$output .= '<td>';
	$output .= drupal_render($form['indexsearch']);
	$output .= '</td>';
	$output .= '<td>';
	$output .= drupal_render($form['submit']);
	$output .= '</td>';
	$output .= '</tr>';
	$output .= '</table>';

	$output .= drupal_render($form);
	
	return $output;
}