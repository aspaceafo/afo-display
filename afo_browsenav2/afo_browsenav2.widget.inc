<?php
// $Id$

/**
 * @file
 *
 * Browse widget functions for AFO Browsenav2 module. 
 */

/***************************************** Construct widget functions **/
 
/**
 * Construct browsing widget.
 */
function afo_browsenav2_buildwidget(array $params) {

	$widget['main']['#title'] = t('main');
	$widget['main']['Type'] = afo_browsenav2_buildwidget_section($params, 'type');
	//$widget['main']['Op'] = afo_browsenav2_buildwidget_section($params, 'op');
	$widget['main']['Sorting'] = afo_browsenav2_buildwidget_section($params, 'sort');
	$widget['main']['Medium'] = afo_browsenav2_buildwidget_tags($params, variable_get('afo_browsenav2_medium_vid', null));
	//$widget['main']['Styles'] = afo_browsenav2_buildwidget_tags($params, variable_get('afo_browsenav2_styles_vid', null));	
	$widget['main']['Tags'] = afo_browsenav2_buildwidget_tags($params, variable_get('afo_browsenav2_tags_vid', null));

	$widget['regions']['#title'] = t('regions');
	$widget['regions']['Region'] = afo_browsenav2_buildwidget_regions($params);
	
	$widget['text']['#title'] = t('name');
	$widget['text']['Search'] = afo_browsenav2_buildwidget_searchbox($params);
	
	return $widget;

}

/**
 * Create widget array of options for standard section.
 */
function afo_browsenav2_buildwidget_section(array $params, $schema_key) {

	$parameter_schema = afo_browsenav2_parameter_schema();

	if(is_array($parameter_schema[$schema_key])) {

		$scheme = $parameter_schema[$schema_key];
		
		$widget_section['label'] = $scheme['label'];
		if(is_array($scheme['attributes'])) {
			foreach($scheme['attributes'] as $k => $v) {
				$widget_section[$k] = $v;
			}
		}

		// add class to section indicating whether it singular or multiple		
		if($scheme['multiple']) $widget_section['classes'][] = 'multiple';
		else $widget_section['classes'][] = 'singular';

		if(is_array($scheme['options'])) {
			foreach($scheme['options'] as $option => $option_label) {

				// check if option is in params field array
				if(is_array($params[$schema_key])) {
					if(in_array($option, $params[$schema_key])) {
						$op = 'remove';
						$sel = TRUE;
					}
					else {
						$op = 'add';
						$sel = FALSE;
					}
				}
				else {
					$op = 'add';
					$sel = FALSE;
				}

				// generate altered args
				$args = afo_browsenav2_params_to_args($params, array('op'=>$op, 'field'=>$schema_key, 'value'=>$option));
				
				// compile link classes
				$link_class = afo_browsenav2_link_to_class($schema_key, $option);

				// add link to widget array
				$widget_section['links'][] = array(
					'text' => $option_label,
					'value' => $option,
					'args' => $args,
					'selected' => $sel,
					'class' => $link_class,
				);
			
			}
		}

	}

	return $widget_section;
}

/**
 * Create widget array of tags for given Vocabulary id.
 */
function afo_browsenav2_buildwidget_tags(array $params, $vid) {
	if(is_numeric($vid)) {

		$vocabulary = taxonomy_vocabulary_load($vid);
		$widget_section['label'] = $vocabulary->name;
		$widget_section['classes'][] = 'multiple';
		
		$terms = taxonomy_get_tree($vid);
						
		foreach($terms as $term) {
			
			// check if tag is in params already
			if(in_array($term->tid, $params['tags'])) {
				$op = 'remove';
				$sel = TRUE;
			}
			else {
				$op = 'add';
				$sel = FALSE;
			}
			
			$args = afo_browsenav2_params_to_args($params, array('op'=>$op, 'field'=>'tags', 'value'=>$term->tid));

			// compile link classes
			$link_class = afo_browsenav2_link_to_class('tags', $term->tid);
			
			$widget_section['links'][] = array(
				'text' => $term->name,
				'value' => $term->tid,
				'args' => $args,
				'selected' => $sel,
				'class' => $link_class,
			);
		}
		
		return $widget_section;
	}
	return NULL;
}


/**
 * Create widget array for regions.
 */
function afo_browsenav2_buildwidget_regions(array $params) {
	$vid = variable_get('afo_browsenav2_region_vid', null); // region vid

	if(is_numeric($vid)) {

		$vocabulary = taxonomy_vocabulary_load($vid);
		$widget_section['label'] = $vocabulary->name;
		$widget_section['classes'][] = 'multiple';
		$widget_section['classes'][] = 'columnize';
		$widget_section['print type'] = 'list';
		
		$terms = taxonomy_get_tree($vid);
		
		foreach($terms as $term) {
			
			if($term->depth > $current_depth && $term->depth > 0) $print_op = 'list open';
			elseif($term->depth < $current_depth) {
				$num_close = $current_depth - $term->depth;
				$print_op = "list close $num_close";
			}
			else $print_op = null;
			$current_depth = $term->depth;
				
			// check if tag is in params already
			if(in_array($term->tid, $params['regions'])) {
				$op = 'remove';
				$sel = TRUE;
			}
			else {
				$op = 'add';
				$sel = FALSE;
			}
			
			$args = afo_browsenav2_params_to_args($params, array('op'=>$op, 'field'=>'regions', 'value'=>$term->tid));
	
			// compile link classes
			$link_class = afo_browsenav2_link_to_class('regions', $term->tid);
			
			$widget_section['links'][] = array(
				'text' => $term->name,
				'value' => $term->tid,
				'args' => $args,
				'selected' => $sel,
				'class' => $link_class,
				'print op' => $print_op,
			);
		}
	
		return $widget_section;
	}	
	
	return null;
}

/**
 * Create widget array for cv options.
 */
function afo_browsenav2_buildwidget_cv(array $params, $cv_section) {
	if(!module_exists('afo_cv')) return null;
	include_once(drupal_get_path('module', 'afo_cv').'/afo_cv_index.inc');
	$venue_list = afo_cv_get_venuelist($cv_section);
	
	if(is_array($venue_list)) {
	
		foreach($venue_list as $venue) {
			// check if search is in params already
			if(in_array($venue['venueid'], $params[$cv_section])) {
				$op = 'remove';
				$sel = TRUE;
			}
			else {
				$op = 'add';
				$sel = FALSE;
			}
			
			$args = afo_browsenav2_params_to_args($params, array('op'=>$op, 'field'=>$cv_section, 'value'=>$venue['venueid']));
	
			// compile link classes
			$link_class = afo_browsenav2_link_to_class($cv_section, $venue['venueid']);
			
			$widget_section['links'][] = array(
				'text' => $venue['name'],
				'value' => $venue['venueid'],
				'args' => $args,
				'selected' => $sel,
				'class' => $link_class,
				'print op' => $print_op,
			);
		}
	
		return $widget_section;
	}	
	
	return null;
}

/**
 * Create widget array for text search box.
 */
function afo_browsenav2_buildwidget_searchbox($params) {

	$parameter_schema = afo_browsenav2_parameter_schema();
	
	//$m .= '<div class="description">'.t('Search: ').'</div>';

	/** text search types **/
	if(!isset($params['textsearchtype'][0])) $params['textsearchtype'][0] = $parameter_schema['textsearchtype']['default'];
/*
	foreach($parameter_schema['textsearchtype']['options'] as $optval => $optlabel) {
		if($params['textsearchtype'][0] == $optval) $sel = 'checked';
		else $sel = '';
		$m .= '<label for="browsenav-text-search-type-'.$optval.'">';
		$m .= '<input type="radio" name="browsenav-text-search-type" class="browsenav-text-search-option" id="browsenav-text-search-type-'.$optval.'" value="'.$optval.'" '.$sel.' /> ';
		$m .= t($optlabel);
		$m .= '</label>&nbsp;&nbsp;';
	}
*/

	$m .= t('Search Artist Names');
	
	$m .= '<div class="horiz-clear">&nbsp;</div>';
	
	/** text search input **/
	$m .= '<input type="text" name="browsenav-text-search" id="browsenav-text-search" class="form-text width-auto" size="40" maxlength="200" />&nbsp;';

	/** text search button **/
	$m .= '<input type="button" name="browsenav-text-search-add" id="browsenav-text-search-add" class="form-submit" value="'.t('Search').'" />';
	
	$widget_section['label'] = t('Text search');
	$widget_section['markup'] = $m;
	return $widget_section;
}

/***************************************** Print widget functions **/

/**
 * Print browsing widget.
 */
function afo_browsenav2_widget_print($widget, $start_page = 'main') {

	$schema = afo_browsenav2_parameter_schema();

	global $base_path; 
	$images_folder = $base_path.drupal_get_path('module', 'afo_display').'/images';

	// widget outer box
	$w .= '<div class="browsenav-widget"><div class="browsenav-widget-inside">';
	$w .= '<div class="current-search">';
		$w .= '<div id="show-button"><a href="#" id="show-widget"><img src="'.$images_folder.'/Arrow-Right.png" alt=">" title="Show Options" /></a></div>';
		$w .= '<div id="current-search-options"></div>';
	$w .= '</div>';
	$w .= '<div class="search-options">';	

	// widget page buttons
	$w .= "<div id='browsenav-page-options'>";
		$w .= '<div id="hide-button"><a href="#" id="hide-widget"><img src="'.$images_folder.'/Arrow-Down.png" alt=">" title="Hide Options" /></a></div>';

		// each page button
		foreach($widget as $pagename => $page) {
			if($pagename == $start_page) $link_class = 'active-page';
			else $link_class = 'inactive-page';
			
			$link_id = afo_browsenav2_link_to_class('page', $pagename);
			$w .= "<div class='change-page'><a class='$link_class' href='#' id='$link_id'>".$page['#title'].'</a></div>';	
		}

		// search / clear buttons
		$w .= '<div id="clear-button">';
			$w .= '<a href="#" id="clear-search">'.t('clear').'</a>';
		$w .= '</div>';	
		$w .= '<div id="search-button">';
			$w .= '<input type="button" value="'.t('search').'" id="go-search" />';
			//$w .= l(t('SEARCH'), '#', array('attributes'=>array('id'=>'go-search')));
		$w .= '</div>';	

		// results preview
		$w .= '<div id="results-preview"><span class="results-total"></span><span class="results-label hide">'.t(' results expected').'</span>';
		
		// selected terms
		$w .= '<div id="selected-search-terms">';
		$w .= '</div>';	

		$w .= '';
		
	$w .= '</div><div class="horiz-clear">&nbsp;</div></div>';

	$w .= '<div class="horiz-clear">&nbsp;</div>';
	
	foreach($widget as $pagename => $page) {

		if($pagename == $start_page) $page_class = 'active-page';
		else $page_class = 'inactive-page';
	
		// widget page
		$w .= "<div id='browsenav-page-$pagename' class='widget-page $page_class'><div class='widget-page-inside'>";

		// section table
		$w .= "<table class='widget-page-table' border='0' cellspacing='0' cellpadding='0'><tr>";

		$page_sections = element_children($page);
		if(count($page_sections) > 0) {

			foreach($page_sections as $sectionname) {
			
				$section = $page[$sectionname];
			
				if(is_array($section['classes'])) $section_classes = implode(' ', $section['classes']);

				// close column;
				if(!$section['column no open']) $w .= '<td class="widget-page-column">';
							
				// page section
				$w .= "<div id='browsenav-section-$sectionname' class='section $section_classes'><div class='section-inside'>";

					// section label
					if($section['label']) $w .= '<div class="section-label">' . $section['label'] . '</div>';
					
					// open UL if print type is list
					if($section['print type']=='list') {
						$w .= "<ul id='browsenav-list-$sectionname'>";
						$list_depth = 1;
					}
					
					// section markup
					if(isset($section['markup'])) {
						$w .= $section['markup'];
					}
					
					// section links
					if(is_array($section['links'])) {
						foreach($section['links'] as $k => $link) {
						
							$link_text = t($link['text']);
							
							if(is_array($link['args'])) $link_URL = afo_browsenav2_args_to_URL($link['args']);
							if($link['selected']) $active_class = ' active';
							else $active_class = '';

							//if($link_URL) $link_content = l($link_text, $link_URL, array('attributes'=>array('class'=>$link['class'].$active_class)));
							
							// old version: have link point at static version if no JS
							// if($link_URL) $link_content = "<a href='$link_URL' class='".$link['class'].$active_class."'>$link_text</a>";
							// now only JS work to stop robots?
							if($link_URL) $link_content = "<a href='#' class='".$link['class'].$active_class."'>$link_text</a>";
							else $link_content = $link_text;
							
							$div_class = 'link-for_'.str_replace(' ', '-', $link['value']);

							// list type
							if($section['print type']=='list') {
								if($link['print op']=='list open') {
									$w .= '<ul>';
									$list_depth ++;
								}
								elseif(substr($link['print op'], 0, 10)=='list close') {
									$num_close = substr($link['print op'], 10);
									if(is_numeric($num_close)) {
										for($c = 0; $c < $num_close; $c++) {
											$w .= '</li></ul>';
										}
									}
									$list_depth = $list_depth - $num_close;
									$w .= '</li>';
								}
								elseif($k != 0) $w .= '</li>';
								$w .= "<li class='widget-item $div_class'>$link_content";
							}
							
							// otherwise div
							else $w .= "<div class='widget-item $div_class'>$link_content</div>";
												
						}
					}

					// close list
					if($section['print type']=='list') {
						for($l=0; $l<$list_depth; $l++) {
							$w .= '</li></ul>';
						}						
					}

				// close section
				$w .= '</div></div>';

				$w .= '<div class="horiz-clear">&nbsp;</div>';
				
				// close column;
				if(!$section['column no close']) $w .= '</td>';
				
			}
			
		}

		// close table
		$w .= '</tr></table>';
		
		// close page
		$w .= '</div></div>';
		
	}
	
	// widget sidebar
	$w .= '<div class="widget-sidebar">';
		
	// close sidebar
	$w .= '</div>';	
	
	$w .= '<div class="horiz-clear">&nbsp;</div>';
	
	// widget footer
	$w .= '<div class="widget-footer"><div class="widget-footer-inside">';

	// close footer	
	$w .= '</div></div>';

	$w .= '</div>';
	
	// close outer box
	$w .= '</div></div>';

	$w .= '<div class="horiz-clear">&nbsp;</div>';
	
	return $w;
}