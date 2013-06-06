<?php
// $Id$

/**
 * @file
 *
 * Favorites page.
 */
 
/**
 * Build favorites page.
 */
function afo_favorite_favorites_page($uid = null) {
	global $user;
	
	if (is_numeric($uid) && $uid != $user->uid) {
		// load user
		$u = user_load($uid);
		$currentUser = false;
		// set title
		drupal_set_title(t('@user: Favorites', array('@user'=>$u->name)));
	} else if (!$uid && $user->uid === 0) {
		// anonymous user: send them to register page
		drupal_goto('sign-up');
	} else {
		// use current user
		$u = $user;
		$currentUser = true;
		// set title
		drupal_set_title(t('Your Favorites'));
	}
	
	// access check
	if (!$currentUser) {
		if (!afo_favorite_access_favs($u->uid)) {
			return drupal_access_denied();
		}
	}
	
	if (!$u) return t('Unable to find user.');
	
	$content = '';
	
	/*
	$fav_types[] = array(
		'type' => 'user',
		'title' => t('Users'),
		'grid op' => 'artists',
		'parent' => 'left-col',
	);
	*/
	$fav_types[] = array(
		'type' => 'node',
		//'title' => t('Artworks and Exhibitions'),
		'grid op' => 'artwork',
		'parent' => 'left-col',
	);
	
	$content['left-col'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => 1,
		'#width' => '50%',
	);
	$content['right-col'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#weight' => 2,
		'#width' => '50%',
	);
	
	// Import lightbox
	if ($currentUser) {
		$a_id = afo_import_get_a_id($u->uid);
		if($_GET['import'] == 'lb') {
			module_load_include('inc', 'afo_favorite', 'afo_favorite.import');
			$lb = afo_favorite_lb_import_get($u->uid, $a_id);
			$lb_data = afo_favorite_lb_list($lb);
			if(is_array($lb_data)) $lb_content = theme('afo_display_grid', $lb_data, t('Artist File Light box Items'), 'artists', array('no rollover'=>false, 'classes'=>array('tightGrid')));
			else $lb_content = t('Unable to find your light box. Please use the <a href="@contact">contact page</a> to inform a site administrator.', array('@contact'=>url('contact')));
			$content['lb'] = array(
				'#weight' => -5,
				'#value' => $lb_content,
			);
		} 
		else {
			if($a_id) {
// REMOVE LIGHTBOX TEXT
//				$lb_message = l('View Artists File Light Boxes', 'favorites', array('query'=>'import=lb', 'attributes'=>array('class'=>'buttonLink'))) .t('<p><b>Looking for your light boxes?</b> Soon we will provide a way to import your light boxes from the old Artists File. As a temporary convenience, click the above button to see the artists saved in those light boxes.</p>');
			} else {
//				$lb_message = t('<p><b>Looking for your light boxes?</b> We are working on importing the light boxes from the old Artists File. However, the email associated with this account does not match any users on that site. If you need access to your old light boxes, try <a href="@setting">changing this account\'s email</a> to match your old Artists File account.<p>', array('@setting'=>url('user/'.$u->uid.'/edit'))); 
			}
			$content['right-col']['import'] = array(
				'#type' => 'afo_contentbox',
				'#value' => $lb_message,
				'#weight' => -5,
			);
		}
	}

	// setup buttons array
	foreach($fav_types as $fav_type) {	
		$favs = afo_favorite_getdata($u->uid, $fav_type['type']);
		if(is_array($favs)) {
			$hasFavorites = true;

			// add buttons if current user
			if ($currentUser) {
				foreach($favs as $k => $fav) {
					$favs[$k]['buttons'] = array(
						'delete' => array(
							'class' => 'favorite-remove',
							'url' => "favorites/remove/{$fav_type[type]}/{$fav[nid]}",
						),
					);
				}
			}
				
			$content[$fav_type['parent']][$fav_type['type'].'-favs'] = array(
				'#value' => theme('afo_display_grid', $favs, $fav_type['title'], $fav_type['grid op'], array('no rollover'=>true, 'buttons'=>$buttons, 'classes'=>array('tightGrid'))),
			);
		}
	}	
	
	if(!$hasFavorites) {
		if ($currentUser) {
			$top_message = t('You have not saved any favorites yet!');
			$suggestions = array(
				t('save your favorite artworks and exhibitions by clicking the ADD TO FAVORITES link under them'),
			);
			$top_message .= theme('item_list', $suggestions, t('Suggestions'), 'ul', array('class'=>'arrow tightList'));
		} else {
			$top_message = t('@user has not saved any favorites.', array('@user'=>$u->name));
		}		
	}
	
	if($top_message) {
		$content['top-message'] = array(
			'#value' => $top_message,
			'#weight' => -10,
		);
	}
	
	if ($currentUser) {
		// check box form for public/private favs
		if(!afo_favorite_access_favs($user->uid)) {
			$shareMessage = t('<strong>Share your favorites:</strong> Make your list of favorite artworks public by checking <br />the Share Your Favorites checkbox in !profileLink.', array('!profileLink'=>l(t('your profile settings'), 'user/me/edit/Profile')));
		}
		
		if ($shareMessage) {
			$content['left-col']['public-form'] = array(
				'#value' => $shareMessage,
				'#wegiht' => -10,
			);	
		}
	}
	
	if(module_exists('afo_newsfeed') && $currentUser) {
		$favorites_activity = module_invoke('afo_newsfeed', 'block', 'view', 2);
		if($favorites_activity) {
			$newsfeed = '<h2>'.$favorites_activity['subject'].'</h2>'.$favorites_activity['content'];
			$content['right-col']['favorites-activity'] = array(
				'#value' => $newsfeed,
				'#weight' => 5,
			);
		}
	}
	
	return drupal_render($content);
}

/**
 * Get user's full favorite data for grid.
 */
function afo_favorite_getdata($uid, $op='node') {
	if(!is_numeric($uid)) return null;

	if($op=='user') {
		$query = db_query('SELECT f.fav_uid AS uid, u.name
		FROM {afo_favorite_users} f 
		LEFT JOIN {users} u ON f.fav_uid = u.uid 
		WHERE f.uid = %d 
		ORDER BY updated DESC
		', $uid);
	}
	else {
		$query = db_query('SELECT f.nid AS nid, n.title, n.uid, u.name, files.filepath 
		FROM {afo_favorite_nodes} f
		LEFT JOIN {node} n ON n.nid = f.nid
		LEFT JOIN {users} u ON n.uid = u.uid 
		LEFT JOIN {content_type_artwork} artwork ON artwork.nid = n.nid
		LEFT JOIN {files} files ON files.fid = artwork.field_artwork_image_fid
		WHERE f.uid = %d 
		ORDER BY updated DESC
		', $uid);
	}
	
	while($row = db_fetch_array($query)) {
		$result[] = $row;
	}
	
	return $result;
}