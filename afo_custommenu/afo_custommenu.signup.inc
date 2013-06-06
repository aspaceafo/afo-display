<?php
// $Id$

/**
 * @file
 *
 * Sign up info page
 */

/**
 * Build sign up page.
 */
function afo_custommenu_signup_page() {

	/** top **/

	//$mission = variable_get('site_mission', null);
	//$message = afo_custommenu_site_statement();
	$message .= t('
		<p>local-artists.org will allow you to present your work effectively, initiate contact from peer to peer and from artist to curator, and offers many new networking possibilities and enhanced search features.</p>
		
		<p>local-artists.org offers you a choice of account type: <b>artist</b> or <b>curator</b>. As an artist, you can post your artwork in an online portfolio, submit work to curated open calls, and be featured in exhibitions. As a curator, you can create exhibitions of artists\' work and put out open calls for artwork submissions.</p>
	
		<p>Furthermore, local-artists.org now offers artists an upgraded <b>Membership option</b>, with enhanced features. Please see the !membership for more details. To upgrade to full Artist Membership, you must first create a normal artist account below.</p>
	', array('!membership'=>l(t('Membership page'), 'membership')));
	
	if($message) {
		$content['intro'] = array(
			'#type' => 'afo_contentbox',
			'#value' => $message,
			'#weight' => -5,
		);	
	}
		

	/**************************** column left **/
	$content['col-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#weight' => -1,
	);
	$content['col-left']['about-curator'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('As an Artist...'),
		'#value' => t('Register as an artist to build a portfolio of work. After creating your artist account, you will have the option to upgrade it to a paid membership.') . '<p>' . l(t('Create new account'), 'user/register', array('attributes'=>array('class'=>'buttonLink'))),
		'#weight' => -1,
	);

	/**************************** column right **/
	$content['col-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#weight' => 1,
	);
	$content['col-right']['about-curator'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('As a Curator...'),
		'#value' => t('local-artists.org offers curatorial tools to users from galleries, museums, and arts organizations.') . '<p>' . l(t('Create new account'), 'user/register', array('attributes'=>array('class'=>'buttonLink'))),
	);

	return drupal_render($content);
	
}

/**
 * Create sign up & membership info block
 */
function afo_custommenu_signup_infoblock() {
	
	global $user;

	$block_title = t('Your Account');

	if($user->uid > 0) {
		// welcome
		$content_message = t('Welcome, @name.', array('@name'=>$user->name));
		$content_links[] = l(t('» see your Control Panel'), 'control-panel');
		$content_links[] = l(t('» Membership Status'), 'membership');
	}
	else {
		// login / register
		$content_message = t('Welcome to local-artists.org.');
		$content_links[] = l(t('User log in'), 'user', array('query'=>'target=#login-box&noscroll=1', 'attributes'=>array('class'=>'opener buttonLink'))) . '<div id="login-box" class="hide">' . drupal_get_form('user_login') . '</div>';
		$content_links[] = l(t('» Click Here to Register'), 'sign-up');
		$content_links[] = l(t('» Membership Options'), 'membership');
	}	

	$content .= '<div>'.$content_message.'</div>';
	$content .= theme('item_list', $content_links);
	
	$block['subject'] = $block_title;
	$block['content'] = $content;
	
	return $block;
}