<?php
// $Id$

/**
 * @file
 *
 * AFO Portfolio: portfolio display.
 */

/************************************************ settings **/

/**
 * Return array of portfolio view types.
 */
function afo_portfolio_view_types() {
	return array(
		0 => 'slide show',
		1 => 'grid',
	);
}

/************************************************ display functions **/

/**
 * Implementation of hook_block().
 */
function afo_portfolio_build_portfolio_block($uid) {
	if(is_numeric($uid)) {
		
		// artist role rid
		$artist_rid = variable_get('afo_custommenu_roles_artist', 'none');
		
		// load portfolio's user object
		$portfolio_user = user_load(array('uid'=>$uid));

		// make sure artist role		
		if(!array_key_exists($artist_rid, $portfolio_user->roles)) return null;
		
		// determine type of display based on role:
		$member_role = variable_get('afo_custommenu_roles_artist_member', 'none');
		if(array_key_exists($member_role, $portfolio_user->roles)) {
			$view_type = 'member';
			$load_type = 'full'; // members need full load because they can have folder descriptions
		}
		else {
			$view_type = 'basic';
			$load_type = 'small'; // non-members don't need full load, so query is faster
		}
		
		// build portfolio		
		module_load_include('inc', 'afo_portfolio', 'afo_portfolio.classes');
		$portfolio = new portfolioPortfolio(array('uid'=>$uid, 'load type'=>$load_type));
		
		// portfolio empty
		if(count($portfolio->children) == 0) {
			global $user;
			if($user->uid == $portfolio_user->uid) {
				drupal_set_message(t('Your portfolio is empty. Click the Add New Artwork button above to get started.'), 'warning');
			}
		}
		
		// create slide show
		$portfolio_slides = $portfolio->getSlides();
		if(!is_array($portfolio_slides)) return null;

		// get settings
		$settings = afo_portfolio_getsettings($uid);

		// build member options
		if($view_type == 'member') {
			// determine which view type to display
			if($settings->view_type == 1) {
				$slides_class = 'hide';
				$grid_class = '';
			} else {
				$slides_class = '';
				$grid_class = 'hide';
			}
		
			// slide show
			$content['slideshow'] = array(
				'#value' => theme('afo_display_slideshow', $portfolio_slides, null, 'artwork', array('id'=>'afo-portfolio-slides', 'classes'=>array('afo-portfolio-display', $slides_class))),
				'#weight' => -2,
			);
			
			// grid
			$content['grid'] = array(
				'#value' => $portfolio->printItem('normal', array('classes'=>array('afo-portfolio-display', $grid_class))),
				'#weight' => -1,
			);
			

			/* custom full screen light box
			// light box
			$content['fullscreen'] = array(
				'#value' => theme('afo_display_lightbox', $portfolio_slides, null, 'artwork', array('id'=>'afo-portfolio-fullscreen')),
			);
			*/
			
			// controls
			$options[] = array(
				'#type' => 'slides',
				'#target' => 'afo-portfolio-slides',
				'#close' => 'afo-portfolio-display',
			);
			$options[] = array(
				'#type' => 'grid',
				'#target' => 'portfolio-'.$uid,
				'#close' => 'afo-portfolio-display',
			);
			$options[] = array(
				'#type' => 'fullscreen',
				'#nids' => $portfolio_slides,
				'#target' => 'afo-portfolio-fullscreen',
				'#class' => array('afo-slidelightbox-launch'),
			);
			
			$content['controls'] = array(
				'#value' => theme('afo_display_view_options', $options),
				'#weight' => -10,
			);
		}
		
		// else build basic
		else {
			/*
			$content['slideshow'] = array(
				'#value' => theme('afo_display_slideshow', $portfolio_slides, null, 'artwork', array('id'=>'afo-portfolio-slides', 'classes'=>array('afo-portfolio-display'))),
			);
			*/
			$content['grid'] = array(
				'#value' => $portfolio->printItem('normal', array('classes'=>array('afo-portfolio-display', $grid_class))),
			);
		}
				
		$block['content'] = drupal_render($content);		
		return $block;
	}
	return null;
}


/************************************************ Edit page OUTPUT **/

/**
 * Form builder for edit portfolio page.
 */
function afo_portfolio_editpage() {

	global $user;
	$uid = $user->uid;

	include_once('afo_portfolio.classes.inc');
	$portfolio = new portfolioPortfolio(array('uid'=>$uid, 'load type'=>'small'));
	$collapsed = array();
	$portfolio->hierarchyCollapse($collapsed);

	global $base_path; 
	$images_folder = $base_path.drupal_get_path('module', 'afo_display').'/images';

	$artwork_type = afo_portfolio_gettypes('artwork');
	$folder_type = afo_portfolio_gettypes('folder');
	
	$form['#tree'] = TRUE;
	$form['#redirect'] = "user/$uid";
	$form['#theme'] = 'afo_portfolio_editpage';
	
	$form['uid'] = array(
		'#type' => 'value',
		'#value' => $uid,
	);

	/*** settings ***/
	$settings = afo_portfolio_getsettings($uid);
	
	if(user_access('edit own portfolio settings')) {
		$form['settings'] = array(
			'#type' => 'fieldset',
			'#title' => t('Settings'),
			'#collapsible' => true,
		);
		$form['settings']['view_type'] = array(
			'#type' => 'radios',
			'#title' => t('Default view'),
			'#description' => t('Set the method your portfolio will be displayed. Visitors will be able to switch to the other types.'),
			'#options' => afo_portfolio_view_types(),
			'#default_value' => $settings->view_type,
		);
	}
	
	foreach($collapsed as $row) {

		$form['rows'][$row->nid] = array();

		$form['rows'][$row->nid]['nid'] = array(
			'#title' => 'nid',
			'#type' => 'hidden',
			'#value' => $row->nid,
		 );

		$form['rows'][$row->nid]['folder-nid'] = array(
			'#title' => 'folder-nid',
			'#type' => 'hidden',
			'#default_value' => $row->structure['folder_nid'],
		 );

		$form['rows'][$row->nid]['depth'] = array(
			'#type' => 'value',
			'#value' => $row->structure['depth'],
		 );

		if($row->itemType == $artwork_type) 
			$thumbnail = theme('afo_display_node_thumbnail', $row->nid, 'artwork_tiny', null, array('no label'=>true));
		elseif($row->itemType == $folder_type)
			$thumbnail = "<img src='$images_folder/Folder-Icon.png' alt='folder' title='folder' />";
		else
			$thumbnail = '';
					
		$form['rows'][$row->nid]['thumbnail'] = array(
			'#title' => '',
			'#type' => 'markup',
			'#value' => $thumbnail,
		 );

		$form['rows'][$row->nid]['title'] = array(
			'#title' => 'title',
			'#type' => 'markup',
			'#value' => '<span class="item-title">'.check_plain($row->data['title']).'</span>',
		 );

		$form['rows'][$row->nid]['type'] = array(
			'#title' => 'type',
			'#type' => 'hidden',
			'#value' => check_plain($row->itemType),
		 );
		
		$form['rows'][$row->nid]['weight'] = array(
			'#type'=>'hidden',
			'#default_value'=>$row->structure['weight'],
		);
		
		$form['rows'][$row->nid]['edit-link'] = array(
			'#title' => '',
			'#type' => 'markup',
			'#value' => '&nbsp;&nbsp;&nbsp;<span class="edit-link">('.l(t('edit'), 'node/'.$row->nid.'/edit').')</span>',
		 );
	}	
	
	$form['submit-top'] = array(
		'#type' => 'submit',
		'#value' => t('Save Portfolio'),
		'#weight' => -20,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save Portfolio'),
	);
	
	return $form;
}
/**
 * Theme form.
 */
function theme_afo_portfolio_editpage($form) {	

	drupal_add_css(drupal_get_path('module', 'afo_portfolio').'/afo_portfolio.css');	

	drupal_add_tabledrag('portfolio-edit-table', 'order', 'self', 'item-weight');
	drupal_add_tabledrag('portfolio-edit-table', 'match', 'parent', 'item-parent', NULL, 'item-id', TRUE, 1);	
	
	$rows = array();

	foreach (element_children($form['rows']) as $key) {

		$element = &$form['rows'][$key];

		$row = array();

		$element['nid']['#attributes']['class'] = 'item-id';
		$element['folder-nid']['#attributes']['class'] = 'item-parent';
		$element['weight']['#attributes']['class'] = 'item-weight';

		// nid, thumbnail, title, edit link
		$row[] = 
			theme('indentation', $element['depth']['#value'])			
			. drupal_render($element['thumbnail'])
			. drupal_render($element['title'])
			. drupal_render($element['edit-link'])
		;
		
		// weight
		$row[] = drupal_render($element['weight']) . drupal_render($element['folder-nid']) . drupal_render($element['nid']);
		
		// put row
		$rows[] = array(
			'data' => $row,
			'class' => 'draggable portfolio-'.$element['type']['#value'],
		);
	}
	
	if (count($rows) == 0) {
		drupal_set_message(t('Your portfolio is empty. Click the Add New Artwork button above to get started.'), 'warning');
	}
	
	$headerText .= '<div class="secondaryText">';
	$headerText .= t('Drag arrow icon to re-order. The top artwork will be used as your thumbnail image. ');
	if(user_access('create folder content')) {
		$headerText .= t('<br />To move an artwork into a folder, drag the artwork under the folder and to the right. ');		
	}
	$headerText .= '</div>';
	
	$output .= $headerText;
	$output .= drupal_render($form['submit-top']);
	$output .= theme('table', array('', ''), $rows, array('id' => 'portfolio-edit-table'));
	$output .= drupal_render($form);

	return $output;
}


/**
 * Form submit for edit portfolio page.
 */
function afo_portfolio_editpage_submit($form, &$form_state) {
	foreach($form_state['values']['rows'] as $row) {
		$nid = $row['nid'];
		$folder_nid = is_numeric($row['folder-nid']) ? $row['folder-nid'] : 0;
		$weight = is_numeric($row['weight']) ? $row['weight'] : 0;
		if(is_numeric($nid)) {
			afo_portfolio_putstructure($nid, $weight, $folder_nid, 'check');
		}
	}
	
	if(is_numeric($form_state['values']['uid'])) {
	
		if(is_array($form_state['values']['settings'])) {
			// update settings
			$settings = new stdclass;
			foreach($form_state['values']['settings'] as $field => $setting) {
				$settings->$field = $setting;
			}
			afo_portfolio_putsettings($form_state['values']['uid'], $settings);
		}

		// update thumbnail nid field
		afo_portfolio_thumbnail_update($form_state['values']['uid']);
				
	}
}