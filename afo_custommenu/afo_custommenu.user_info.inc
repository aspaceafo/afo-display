<?php
// $Id$

/**
 * @file
 *
 * Create user info block.
 */
 
 /**
 * Build the User Info block.
 */
function custommenu_buildblock_userinfo($uid) {

	$account = user_load($uid);
	if(!$account) return null;

	// title
	$block['subject'] = $account->name;

	// city & region
	$city_term = afo_custommenu_getterms('city', $uid, 'user');
	$region_term = afo_custommenu_getterms('region', $uid, 'user');

	if($city_term || $region_term) $block['content'] .= '<div class="user-info-region">';
	if($city_term) $block['content'] .= '<div>' . l($city_term->name, 'city/'.$city_term->tid) . '</div>';
	if($region_term) $block['content'] .= '<div>' . l($region_term->name, 'browse/type=artists/regions='.$region_term->tid) . '</div>';
	if($city_term || $region_term) $block['content'] .= '</div>';

	// links
	$links = custommenu_output_userlinks($account);
	if(is_array($links)) $block['content'] .= theme('item_list', $links, null, 'ul', array('class'=>'tightList'));

	// statement teaser
	if($account->profile_statement) {
		$statement = theme('afo_display_text', $account->profile_statement, array(
			'output'=>'html', 
			'size'=>'small', 
			'read more'=>"user/$uid/profile",
		));
//		$statement = substr($statement, 0, 200) . '...';
//		$statement .= l(t(' » read more'), "user/$uid/profile", array('attributes'=>array('class'=>'moreLink')));
		$block['content'] .= "<div class='user-info-statement'>$statement</div>";

	}
	
	// user events
	if(user_access('show events', $account)) {
		$events = afo_custommenu_events_list(array('args'=>array('uid'=>$uid)));
		if($events) {
			$block['content'] .= $events; 
		}
	}

	return $block;
}

/**
 * Output the user links as an array.
 */
function custommenu_output_userlinks($account) {

	$uid = $account->uid;
	if(!is_numeric($uid)) return null;

	if($account) {
	
		if(in_array('artist', $account->roles)) $displayType = 'artist';
		elseif(in_array('curator', $account->roles)) $displayType = 'curator';

		// user page
		if($displayType == 'artist') $userpagename = t('Portfolio');
		elseif($displayType == 'curator') $userpagename = t('Curating');
		else $userpagename = t('User page');
		$links[] = l($userpagename, "user/$uid");
		
		$links[] = l(t('Profile'), "user/$uid/profile");

		// user's favorites link if public
		if (module_exists('afo_favorite')) {
			if (afo_favorite_access_favs($account->uid)) {
				$links[] = l(t('@userName\'s Favorites', array('@userName'=>$account->name)), 'favorites/'.$account->uid);
			}
		}
				
		// cv
		if(module_exists('afo_cv')) {
			// check if any cv items
			if(afo_cv_checkcv($uid)) $links[] = l(t('CV'), "user/$uid/cv");
		}
		
		if($account->profile_website) $links[] = l(t('Web site'), $account->profile_website, array('attributes'=>array('target'=>'_blank')));
		
		// follow artist link
		/*
		if(module_exists('afo_favorite')) {
			global $user;
			if($uid != $user->uid && !afo_favorite_get($user->uid, 'user', $uid)) {
				if($displayType == 'artist') $followtext = t('Follow Artist');
				elseif($displayType == 'curator') $followtext = t('Follow Curator');
				if($followtext) $links[] = l($followtext, 'favorites/add/user/'.$uid);
			}
		}
		*/
		
		/*
		// Normal drupal contact form
		if(_contact_user_tab_access($account)) {
			$links[] = l(t('Contact'), "user/$uid/contact");
		}
		*/
		
		// Contact form created by Anonymous Contact module; finer controls:
		if(afo_profile_contactaccess($uid)) {
			$links[] = l(t('Contact'), "user/$uid/contact_form");
		}
		
		// Share (addthis)
		if(module_exists('addthis')) {
			if (user_access('view addthis')) {
				$links[] = _addthis_create_button($node, $teaser);
			}
		}

	}
	
	return $links;
}

/**
 * Output the user links as an array.
 */
function theme_custommenu_userlinks($links) {
	if(is_array($links)) {
		$output = '<ul class="UserLinks">';
		foreach($links as $link) {
			$output .= '<li>';
			$output .= l(t($link['text']), $link['path']);
			$output .= '</li>';
		}
		$output .= '</ul>';
	}
	
	return $output;
}