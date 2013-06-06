<?php
// $Id$

/**
 * @file
 *
 * AFO Portfolio artwork modifications.
 */

/********************************************** input functions */

/**
 * Alter artwork node form.
 */
function afo_portfolio_artwork_alternodeform(&$form, &$form_state) {
	
	global $user;

	if(is_numeric($form['nid']['#value'])) {
		$artwork_details = afo_portfolio_getartworkdata($form['nid']['#value']);
		if($artwork_details['year'] == 0) $artwork_details['year'] = '';
	}
	
	$form['#validate'][] = 'afo_portfolio_artwork_nodevalidate';
	$form['#submit'][] = 'afo_portfolio_artwork_nodesubmit';

	/*
	$form['artwork_media'] = array(
		'#type' => 'fieldset',
		'#title' => t('Media'),
		'#weight' => -4,
	);
	$imagefield = variable_get('afo_settings_artwork_videofield', '');
	$videofield = variable_get('afo_settings_artwork_imagefield', '');
	$form['artwork_media'][$imagefield] = $form[$imagefield];
	$form['artwork_media'][$videofield] = $form[$videofield];
	unset($form[$imagefield]);
	unset($form[$videofield]);
	*/
	
	$form['artwork_details'] = array(
		'#type' => 'fieldset',
		'#title' => t('Artwork details'),
		'#weight' => -2,
	);
	$form['artwork_details']['artwork_details_year'] = array(
		'#type' => 'textfield',
		'#title' => t('Year'),
		'#default_value' => $artwork_details['year'],
		'#size' => 4,
		'#maxlength' => 4,
		'#description' => t('Enter the year this piece was completed (e.g. 2010)'),
	);
	$form['artwork_details']['artwork_details_materials'] = array(
		'#type' => 'textfield',
		'#title' => t('Materials'),
		'#default_value' => $artwork_details['materials'],
		'#size' => 30,
		'#maxlength' => 100,
		'#description' => t('List the media used (e.g. Oil on canvas)'),
	);
	$form['artwork_details']['artwork_details_dimensions'] = array(
		'#type' => 'textfield',
		'#title' => t('Dimensions'),
		'#default_value' => $artwork_details['dimensions'],
		'#size' => 30,
		'#maxlength' => 30,
		'#description' => t('Enter the piece\'s dimensions (e.g. 36" x 24")'),
	);
	
	$form['body_field']['body']['#rows'] = 5;

	/********** folder structure ***********/
	if(user_access('create folder content')) {
		// current structure
		$nid = $form['nid']['#value'];
		
		$current_structure = afo_portfolio_getstructure($nid);
		if($current_structure===null) {
			$current_structure = new stdClass;
			$current_structure->weight = 0;
		}
		// user's folders
		$user_folders = afo_portfolio_getfolders($form['uid']['#value']);
		$folders = array(0 => t('<none>'));
		foreach($user_folders as $k=>$v) $folders[$k] = $v;
		
		$form['afo_portfolio_settings'] = array(
			'#type' => 'fieldset',
			'#title' => t('Portfolio Settings'),
		);
		$form['afo_portfolio_settings']['afo_portfolio_folder'] = array(
			'#type' => 'select',
			'#title' => t('Folder'),
			'#description' => t('Place this artwork in a folder. You can rearrange your artworks and folders on the Edit Portfolio page.'),
			'#options' => $folders,
			'#default_value' => $current_structure->folder_nid,
		);
		/*
		$form['afo_portfolio_settings']['afo_portfolio_order'] = array(
			'#type' => 'weight',
			'#title' => t('Order weight'),
			'#delta' => 20,
			'#default_value' => $current_structure->weight,
		);
		*/
	}

	// submit buttons
	if($form['buttons']['submit']) $form['buttons']['submit']['#value'] = t('Save Artwork');
	if($form['buttons']['delete']) $form['buttons']['delete']['#value'] = t('Delete Artwork');

	// disable form items
	if(!is_numeric($form['#node']->nid)) {
		$allowForm = afo_portfolio_artwork_allowed($user);	
	
		if(!$allowForm && module_exists('afo_backend')) {
			afo_backend_disableform($form);
		}
	}

}
/**
 * Artwork node form extra fields submit.
 */
function _afo_portfolio_artwork_nodevalidate($form, $form_state) {
	global $user;
	
	// if creating new node
	if(!is_numeric($form['#node']->nid)) {
		// check if user can create more artworks
		$error = afo_portfolio_artwork_allowed($user, 'form_error');
	}
	
	// check if year is number
	if(!is_numeric($form_state['values']['artwork_details_year']) && strlen($form_state['values']['artwork_details_year'])>0) 
		form_set_error('artwork_details_year', t('Year must be a number.'));
		
}
/**
 * Artwork node form extra fields submit.
 */
function _afo_portfolio_artwork_nodesubmit($form, &$form_state) {
	
	$nid = $form_state['values']['nid'];
	if(is_numeric($nid)) {
		/***** artwork details *****/
		if(array_key_exists('artwork_details_year', $form_state['values'])) {
			$data['year'] = $form_state['values']['artwork_details_year'] > 0 ? $form_state['values']['artwork_details_year'] : 'NULL';
		}
		if(array_key_exists('artwork_details_materials', $form_state['values'])) $data['materials'] = $form_state['values']['artwork_details_materials'];
		if(array_key_exists('artwork_details_dimensions', $form_state['values'])) $data['dimensions'] = $form_state['values']['artwork_details_dimensions'];
		if(is_array($data)) {
			afo_portfolio_setartworkdata($nid, $data);
		}

		/***** folder structure *****/
		if(user_access('create folder content')) {
			$weight = null; //is_numeric($form_state['values']['afo_portfolio_order']) ? $form_state['values']['afo_portfolio_order'] : 0;
			$folder_nid = $form_state['values']['afo_portfolio_folder'];
			afo_portfolio_putstructure($nid, $weight, $folder_nid);
		}
		
		// update thumbnail nid field
		afo_portfolio_thumbnail_update($form_state['values']['uid']);
	}

}


/********************************************** display functions */

/**
 * Modify artwork node.
 * Called by hook_nodeapi.
 */
function afo_portfolio_artwork_modifynode(&$node, $op, $a3 = NULL, $a4 = NULL) {
	switch($op) {
		case 'load':
			
			/************************ Listing **/
			// assemble artwork listing and store in node:
			$listing['artist'] = l($node->name, 'user/'.$node->uid);
			$listing['title'] = $node->title;
			// get extra details from db
			$details = afo_portfolio_getartworkdata($node->nid);
			if(is_array($details)) $listing = array_merge($listing, $details);
			// store in node listing
			$node->listing = $listing;
			
			/************************ Video **/
			$video_field_name = variable_get('afo_settings_artwork_videofield', null);
			if($video_field_name) {
				$video_field = $node->$video_field_name;
				if($video_field[0]['embed']) {
					$node->video = true;
				}
			}

			/************************ Fix artwork file display because nid/vid messed up **/
			/*
			$fid = db_result(db_query('SELECT field_artwork_image_fid FROM {content_type_artwork} WHERE nid = %d', $node->nid));
			if($node->field_artwork_image[0]['fid'] != $fid) {
				$file_data_query = db_query('SELECT * FROM {files} WHERE fid = %d', $fid);
				if($result = db_fetch_array($file_data_query)) {
					foreach($result as $k=>$v) {
						$node->field_artwork_image[0][$k] = $v;
					}
				}
			}
			*/
			break;
			
		case 'view':

			global $user;
			if($user->uid == 1 AND false) {

				dsm('Node nid = '.$node->nid);
				dsm('Node vid = '.$node->vid);
				dsm('Node title = '.$node->title);

				$q = db_query('SELECT rev.nid as rev_nid, rev.vid as rev_vid, rev.title FROM {node_revisions} rev WHERE rev.vid = %d', $node->vid);
				$results = array();
				while($result = db_fetch_array($q)) {
					$results[] = $result;
				}
				dsm($results);

				$q = db_query('SELECT rev.nid as rev_nid, rev.vid as rev_vid, rev.title FROM {node_revisions} rev WHERE rev.nid = %d', $node->nid);
				$results = array();
				while($result = db_fetch_array($q)) {
					$results[] = $result;
				}
				dsm($results);
				
				$q = db_query('SELECT art.nid as art_nid, art.vid as art_vid, files.filepath FROM {content_type_artwork} art LEFT JOIN {files} files ON files.fid = art.field_artwork_image_fid WHERE art.nid = %d', $node->nid);
				$results = array();
				while($result = db_fetch_array($q)) {
					$results[] = $result;
				}
				dsm($results);

				$q = db_query('SELECT art.nid as art_nid, art.vid as art_vid, files.filepath FROM {content_type_artwork} art LEFT JOIN {files} files ON files.fid = art.field_artwork_image_fid WHERE art.vid = %d', $node->vid);
				$results = array();
				while($result = db_fetch_array($q)) {
					$results[] = $result;
				}
				dsm($results);

				dsm($node);		

			}
			
			// set page title (not visible on page, but in window bar)
			drupal_set_title('\'' . $node->title . '\' - ' . $node->name);

			// determine type of display based on role:
			$member_role = variable_get('afo_custommenu_roles_artist_member', null);
			if($member_role) {
				if(db_result(db_query('SELECT uid FROM {users_roles} WHERE uid = %d AND rid = %d', $node->uid, $member_role))) $view_type = 'member';
				else $view_type = 'basic';
			}
			
			// add artwork css
			drupal_add_css(drupal_get_path('module', 'afo_portfolio').'/artwork.css');	
		
			// add listing to content
			if(is_array($node->listing)) {
				$listing_content = theme('afo_portfolio_artwork_listing', $node->listing);
				if($listing_content) {
					$node->content['listing'] = array(
						'#value' => $listing_content,
						'#weight' => -2,
					);
				}
			}
			
			// move body out of content
			$node->artwork_details['body'] = $node->content['body'];
			unset($node->content['body']);
			
			// hide image if video
			if($node->video) {
				$image_field_name = variable_get('afo_settings_artwork_imagefield', null);
				if($image_field_name) unset($node->content[$image_field_name]);
			}
			
			// add full size
			if(!$node->video && $view_type == 'member') {
				
				$image_field = variable_get('afo_settings_artwork_imagefield', null);
				$image_original = $node->$image_field;
				
				if($image_original) {
					$image_original = $image_original[0];
					
					$icon = theme('afo_display_icon', 'expand', array('alt' => t('See full screen')));
					
					$node->content['image-fullsize'] = array(
						'#value' => l($icon, $image_original['filepath'], array('attributes'=>array('rel'=>'lightbox'), 'html'=>true)),
						'#weight' => -1,
					);
					
				}
			}
			
			break;
			
		case 'insert':
			// set artwork data for new node, since form_submit function didn't get the new nid
			if(is_numeric($node->nid)) {
				$data['year'] = $node->artwork_details_year;
				$data['materials'] = $node->artwork_details_materials;
				$data['dimensions'] = $node->artwork_details_dimensions; 
				afo_portfolio_setartworkdata($node->nid, $data);

				/***** folder structure *****/
				if(user_access('create folder content')) {
					$folder_nid = $node->afo_portfolio_folder;
					if(is_numeric($folder_nid)) {
						afo_portfolio_putstructure($node->nid, 0, $folder_nid);
					}
				}				
			}
			break;

		case 'delete':
			// remove artwork structure data from db
			if(is_numeric($node->nid)) {
				afo_portfolio_deletestructure($node->nid);
				afo_portfolio_deletedetails($node->nid);
			}
			break;

	}
}

/**
 * Theme artwork listing.
 */
function theme_afo_portfolio_artwork_listing($listing_data) {
	if(!is_array($listing_data)) return null;
	
	$listing_order = array('artist', 'title', 'year', 'materials', 'dimensions');

	foreach($listing_order as $k) {
		if($listing_data[$k] && $listing_data[$k]!='') {
			$listing[] = "<span class='artwork-listing-$k'>".$listing_data[$k].'</span>';
		}
	}	

	$listing = implode(', ', $listing);
	$listing = "<div class='artwork-listing'>$listing</div>";
	return $listing;
}

/********************************************** artwork db functions */

/**
 * Get artwork data.
 */
function afo_portfolio_getartworkdata($nid) {
	if(!is_numeric($nid)) return null;
	$query = db_query('SELECT * FROM {afo_portfolio_artwork_data} WHERE nid = %d', $nid);
	if($result = db_fetch_array($query)) {
		return $result;
	}
	return null;
}

/**
 * Set artwork data.
 */
function afo_portfolio_setartworkdata($nid, $data) {
	if(!is_numeric($nid) || !is_array($data)) return null;
	
	$record = new stdclass;
	$record->nid = $nid;
	if(isset($data['year'])) $record->year = $data['year'];
	if(isset($data['materials'])) $record->materials = $data['materials'];
	if(isset($data['dimensions'])) $record->dimensions = $data['dimensions'];

	$existing = afo_portfolio_getartworkdata($nid);
	if($existing) drupal_write_record('afo_portfolio_artwork_data', $record, 'nid');
	else drupal_write_record('afo_portfolio_artwork_data', $record);

	return $record;
}