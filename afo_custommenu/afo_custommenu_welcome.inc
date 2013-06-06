<?php
// $Id$

/**
 * @file
 *
 * Custom welcome page.
 */

/**
 * Page builder.
 */
function afo_custommenu_welcome_page() {
	global $user;

	/****************************************** Columns **/
	$content['column-left'] = array(
		'#type' => 'afo_column',
		'#weight' => -2,
		'#position' => 'left',
		'#width' => '50%',
	);
	$content['column-right'] = array(
		'#type' => 'afo_column',
		'#weight' => 0,
		'#position' => 'right',
		'#width' => '50%',
	);
	
	/****************************************** Welcome messages **/
	foreach($user->roles as $role) {
		$varname = 'afo_custommenu_welcome_message_' . str_replace(' ', '_', $role);
		$role_message = variable_get($varname, NULL);
		if($role_message) {
			$content['column-left']["welcome-$role"] = array(
				'#type' => 'afo_contentbox',
				'#value' => '<div class="welcomeMessage">'.t($role_message).'</div>',
				'#weight' => -15,
			);
		}
	}
	
	/****************************************** Control panel **/
	$control_panel = afo_custommenu_build_control_panel();
	if($control_panel) {
		$content['column-left']['control-panel'] = array(
			'#type' => 'afo_contentbox',
			'#value' => $control_panel,
			'#weight' => -10,
		);
	}
	
	/****************************************** Favorites Newsfeed **/
	if(module_exists('afo_newsfeed')) {
		$favorites_activity = module_invoke('afo_newsfeed', 'block', 'view', 2);
		if($favorites_activity) {
			$newsfeed = '<h2>'.$favorites_activity['subject'].'</h2>'.$favorites_activity['content'];
			$content['column-right']['favorites-activity'] = array(
				'#type' => 'afo_contentbox',
				'#value' => $newsfeed,
				'#weight' => 5,
			);
		}
	}
	
	/****************************************** Open calls **/
	if(module_exists('afo_opencall')) {
		
		// open call submissions
		$opencall_submissions = afo_opencall_list_submissions($user->uid, 'Recent Submissions To Your Open Calls');
		if($opencall_submissions) {
			$content['column-right']['opencalls-recent-submissions'] = array(
				'#type' => 'afo_contentbox',
				'#value' => $opencall_submissions,
				'#weight' => -1,
			);
		}

		// open calls ending soon
		$call_args = array();
		$user_tids = afo_custommenu_getterms('medium', $user->uid, 'user', false);
		//$user_tids = array_merge($user_tids, afo_custommenu_getterms('tags', $user->uid, 'user', false));
		if(is_array($user_tids)) {
			foreach($user_tids as $t) $call_args['terms'][] = $t->tid;
			$calls_title = 'Open Calls In Your Medium';
		}
		else $calls_title = 'Open Calls Ending Soon';
		$ending_soon = afo_opencall_list_calls(array('title'=>t($calls_title), 'display op'=>'short', 'args'=>$call_args));
		if($ending_soon) {
			$content['column-right']['upcoming-deadlines'] = array(
				'#type' => 'afo_contentbox',
				'#value' => $ending_soon,
				'#weight' => 0,
			);
		}

	}
		
	return drupal_render($content);

}

/**
 * Construct page links
 */
function afo_custommenu_build_control_panel() {

	global $user;
	if($user->uid == 0) return null;

	/*********************** assemble control panel **/
	
	// portfolio section
	$cp['portfolio'] = array(
		'#title' => t('Portfolio'),
	);	
	$cp['portfolio']['edit-portfolio'] = array(
		'#title' => t('Edit Portfolio'),
		'#href' => 'user/portfolio/edit',
		'#icon' => 'portfolio',
	);
	$cp['portfolio']['add-artwork'] = array(
		'#title' => t('Add New Artwork'),
		'#href' => 'node/add/artwork',
		'#access' => 'create artwork content',
		'#icon' => 'add artwork',
	);
	$cp['portfolio']['add-folder'] = array(
		'#title' => t('Create Folder'),
		'#href' => 'node/add/folder',
		'#access' => 'create folder content',
		'#icon' => 'add folder',
	);

	// curating section
	$cp['curating'] = array(
		'#title' => t('Curate'),
	);	
	$cp['curating']['add-exhibition'] = array(
		'#title' => t('Create New Exhibition'),
		'#href' => 'node/add/exhibition',
		'#access' => 'create exhibition content',
	);
	$cp['curating']['add-open-call'] = array(
		'#title' => t('Create Open Call'),
		'#href' => 'node/add/open-call',
		'#access' => 'create open-call content',
	);

	// profile section
	$cp['profile'] = array(
		'#title' => t('Profile'),
	);	
	$cp['profile']['edit-profile'] = array(
		'#title' => t('Edit profile'),
		'#href' => 'user/me/edit/Profile',
		'#icon' => 'profile',
	);
	$cp['profile']['edit-cv'] = array(
		'#title' => t('Edit CV'),
		'#href' => 'user/me/cv/edit',
		'#access' => 'edit own cv',
		'#icon' => 'cv',
	);
	$cp['profile']['add-event'] = array(
		'#title' => t('Post Event'),
		'#href' => 'node/add/event',
		'#access' => 'create event content',
		'#icon' => 'add event',
	);

	// account section
	$cp['account'] = array(
		'#title' => t('Account'),
	);	
	$cp['account']['edit-account'] = array(
		'#title' => t('Account Settings'),
		'#href' => 'user/me/edit',
		'#icon' => 'settings',
	);
	$cp['account']['membership'] = array(
		'#title' => t('Membership'),
		'#href' => 'membership',
		'#icon' => 'membership',
	);

	// browse section
	$cp['browse'] = array(
		'#title' => t('Browse Local Artists'),
	);	
	$cp['browse']['curated'] = array(
		'#title' => t('New in Curated'),
		'#href' => 'curated',
		'#icon' => 'star',
	);
	$cp['browse']['browse-artwork'] = array(
		'#title' => t('Browse Artwork'),
		'#href' => 'browse',
		'#icon' => 'grid',
	);
	$city_term = afo_custommenu_getterms('city', $user->uid, 'user');
	if(is_object($city_term)) {
		$cp['browse']['city-page'] = array(
			'#title' => t('@city page', array('@city'=>$city_term->name)),
			'#href' => 'city/'.$city_term->tid,
			'#icon' => 'city',
		);
	}
	$cp['browse']['calendar'] = array(
		'#title' => t('Events Calendar'),
		'#href' => 'community/calendar',
		'#icon' => 'calendar',
	);
	
	$content = theme('afo_backend_control_panel', $cp);
	
	return $content;
}

/**
 * Get region from user profile.
 */
function afo_custommenu_get_region($uid) {

	if(is_numeric($uid)) {

		$region_tid = db_result(db_query('
			SELECT tid 
			FROM {term_user}
			WHERE uid = %d
			', $uid));
			
		if(is_numeric($region_tid)) {
			$term = taxonomy_get_term($region_tid);
			return $term;
		}
	}

	return NULL;
}