<?php
// $Id$

/**
 * @file
 *
 * Editor functions for AFO site.
 */

/**
 * Editor main page.
 */
function afo_editor_page() {

	$content = array();

	$content['col-1'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#width' => '200px',
		'#weight' => -1,
	);
	$content['col-2'] = array(
		'#type' => 'afo_column',
		'#position' => 'center',
		'#width' => '200px',
		'#weight' => 0,
	);
	$content['col-3'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '200px',
		'#weight' => 1,
	);

	$sections = array(
		'Create Content' => 'col-1',
		'Administer Content' => 'col-1',
		'Users' => 'col-2',
		'Memberships' => 'col-2',
		'Site Settings' => 'col-3',
	);

	$links['Administer Content'][] = l(t('Top hits list'), 'editor/hits');
	$links['Administer Content'][] = l(t('Flagged content list'), 'editor/flagged');
	$links['Administer Content'][] = l(t('Feature types'), 'editor/featured/types');
	
	// edit home page link
	if ($home_nid = variable_get('afo_custommenu_homepage_node', null)) {
		if (node_access('update', array('nid'=>$home_nid))) {
			$links['Administer Content'][] = l(t('Edit Home Page Text'), "node/$home_nid/edit");
		}
	}
	if(user_access('merge terms')) $links['Administer Content'][] = l(t('Merge tags/terms'), 'editor/merge-terms');

	if(user_access('create artwork content')) $links['Create Content'][] = l(t('Create new artwork'), 'node/add/artwork');
	if(user_access('create exhibition content')) $links['Create Content'][] = l(t('Create new exhibition'), 'node/add/exhibition');
	if(user_access('create open_call content')) $links['Create Content'][] = l(t('Create new open call'), 'node/add/open-call');
	if(user_access('create story content')) $links['Create Content'][] = l(t('Create new blog post'), 'node/add/story');
	if(user_access('create event content')) $links['Create Content'][] = l(t('Create new event'), 'node/add/event');

	$links['Memberships'][] = l(t('Member payments'), 'editor/payments');

	if(user_access('administer users')) $links['Users'][] = l(t('Create new user'), 'admin/user/user/create');
	if(user_access('administer users')) $links['Users'][] = l(t('See user list'), 'editor/users-list');
	if(user_access('administer users')) $links['Users'][] = l(t('Drupal\'s user list'), 'admin/user/user/list');

	$links['Site Settings'][] = l(t('General settings'), 'editor/settings');
	$links['Site Settings'][] = l(t('Curating page'), 'editor/curating');

	if(is_array($links)) {
		foreach($links as $section => $section_links) {
			if(is_array($section_links)) {
				$section_id = str_replace(' ', '-', $section);
				$col_id = isset($sections[$section]) ? $sections[$section] : 'col-1';
				$content[$col_id][$section_id] = array(
					'#type' => 'afo_contentbox',
					'#title' => $section,
					'#value' => theme('item_list', $section_links, null, 'ul', array('class'=>'arrow')),
				);
			}
		}
	}

	return drupal_render($content);
}

/**
 * Editor general site settings.
 */
function afo_editor_settings() {

	$form['homepage'] = array(
		'#type' => 'fieldset',
		'#title' => t('Home page settings'),
	);

	$query = db_query('SELECT * FROM {afo_featured_types} ORDER BY feature_name DESC');
	$feature_select[''] = '<none>';
	while($row = db_fetch_array($query)) {
		$feature_select[$row['feature_id']] = $row['feature_name'];
	}	
	$form['homepage']['afo_featured_homepage_type'] = array(
		'#type' => 'select',
		'#title' => t('Feature type on AFO Home Page'),
		'#description' => t('Select which feature type to display on the site\'s home page. The most recent featured item of this type will appear there automatically.'),
		'#options' => $feature_select,
		'#default_value' => variable_get('afo_featured_homepage_type', ''),
	);

	return system_settings_form($form);
}

/**
 * Editor users list page
 */
function afo_editor_userslist() {

	$rids = array(
		'artist' => 5,
		'curator' => 6,
		'member' => 7,
	);
	
	global $base_path;

	$order = isset($_GET['order']) ? $_GET['order'] : 'name';
	$sort = isset($_GET['sort']) ? $_GET['sort'] : 'asc';
	$name = isset($_GET['name']) ? $_GET['name'] : null;
	$email = isset($_GET['email']) ? $_GET['email'] : null;
	if($sort != 'asc' && $sort != 'desc') $sort = 'asc';

	$pager_num = 0;
	$per_page = 100;

	$q_args = array();

	$query['select'][] = 'u.uid, u.name, u.mail, u.login, u.created';
	$query['count select'] = 'COUNT(*) AS total';
	$query['from'] = '{users} u';

	// roles	
	$query['select'][] = 'artist_role.rid AS artist';
	$query['joins'][] = 'LEFT JOIN {users_roles} artist_role ON artist_role.uid = u.uid AND artist_role.rid = '.$rids['artist'];
	$query['select'][] = 'curator_role.rid AS curator';
	$query['joins'][] = 'LEFT JOIN {users_roles} curator_role ON curator_role.uid = u.uid AND curator_role.rid = '.$rids['curator'];
	$query['select'][] = 'member_role.rid AS member';
	$query['joins'][] = 'LEFT JOIN {users_roles} member_role ON member_role.uid = u.uid AND member_role.rid = '.$rids['member'];
	
	// name search
	if($name) {
		$name = filter_xss($name);
		$query['where'][] = 'u.name LIKE \'%%%s%%\' AND ';
		$q_args[] = trim($name);
	}

	// email search
	if($email) {
		$email = filter_xss($email);
		$query['where'][] = 'u.mail LIKE \'%s\' AND ';
		$q_args[] = trim($email);
	}
	
	$query['where'][] = 'u.uid != 0 AND u.uid != 1 AND ';

	switch($order) {
		case 'email':
			$query['order'][] = 'u.mail '.$sort;
			break;
			
		case 'login':
			$query['order'][] = 'u.login '.$sort;
			$query['where'][] = 'u.login > 0';
			break;

		case 'created':
			$query['order'][] = 'u.created '.$sort;
			break;
			
		case 'name':
			break;		
	}
	$query['order'][] = 'u.name asc';

	$q = afo_backend_buildquery($query);

	$result = pager_query($q['sql'], $per_page, $pager_num, $q['count sql'], $q_args);
	while($row = db_fetch_array($result)) {
		$data[] = $row;
	}
	
	$headers = array(
		'ID #',
		'',
		array(
			'data' => 'name',
			'field' => 'name',
			'sort' => 'asc',
		),
		array(
			'data' => 'email',
			'field' => 'email',
			'sort' => 'asc',
		),
		array(
			'data' => 'role(s)',
		),
		array(
			'data' => 'login',
			'field' => 'login',
			'sort' => 'desc',
		),
		array(
			'data' => 'created',
			'field' => 'created',
			'sort' => 'desc',
		),
		array(
			'data' => 'Edit',
		),
		array(
			'data' => 'Delete',
		),
	);
	
	if($data) {
		$rows = array();
		foreach($data as $row) {
			$new_row = array();
			$new_row[] = $row['uid'];
			$new_row[] = l('login', 'devel/switch/'.$row['name']);
			$new_row[] = l($row['name'], 'user/'.$row['uid']);
			$new_row[] = l($row['mail'], 'mailto:'.$row['mail'], array('target'=>'_blank'));
	
			$roles = array();
			if(isset($row['artist'])) $roles[] = 'artist';
			if(isset($row['curator'])) $roles[] = 'curator';
			if(isset($row['member'])) $roles[] = 'member';
			$new_row[] = implode(',', $roles);
	
			$new_row[] = $row['login']>0 ? date('j.M.y', $row['login']) : '';		
			$new_row[] = $row['created']>0 ? date('j.M.y', $row['created']) : '';		
			$new_row[] = "<a href='{$base_path}user/{$row[uid]}/edit'>edit</a>";
			$new_row[] = "<a href='{$base_path}editor/delete/user/{$row[uid]}'>delete</a>";

			$rows[] = $new_row;
		}

		$content['users-table'] = array(
			'#type' => 'afo_contentbox',
			'#value' => theme('table', $headers, $rows),
		);
	
		$content['pager'] = array(
			'#value' => theme('pager', null, $per_page, $pager_num),
			'#weight' => 2,
		);
		
	}
	
	$content['users-search'] = array(
		'#type' => 'afo_contentbox',
		'#value' => drupal_get_form('afo_editor_usersearch_form'),
		'#weight' => -3,
	);
	
	return drupal_render($content);

}

/**
 * User search form
 */
function _afo_editor_usersearch_form() {
	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('User name'),
		'#description' => t('Enter a portion of a name to search for.'),
		'#required' => false,
		'#default_value' => $_GET['name'],
	);
	$form['email'] = array(
		'#type' => 'textfield',
		'#title' => t('User email'),
		'#description' => t('Enter an email to search for.'),
		'#required' => false,
		'#default_value' => $_GET['email'],
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Search'),
	);
	
	return $form;
}
/**
 * User search form submit
 */
function _afo_editor_usersearch_form_submit($form, $form_state) {
	$name = $form_state['values']['name'];
	$email = $form_state['values']['email'];
	drupal_goto('editor/users-list', "name=$name&email=$email");
}

/**
 * Editor: top hits list page
 */
function afo_editor_hitslist() {

	$q_args = array();

	$query['select'][] = 'counter.nid, counter.totalcount, counter.daycount, counter.timestamp, n.title, n.type, u.uid, u.name';
	$query['count select'] = 'COUNT(*) AS total';
	$query['from'] = '{node_counter} counter';

	// roles	
	$query['joins'][] = 'LEFT JOIN {node} n ON n.nid = counter.nid';
	$query['joins'][] = 'LEFT JOIN {users} u ON u.uid = n.uid';

	$query['where'][] = 'n.nid != 84820 AND n.nid != 84821 AND ';
	
	//switch($order) {
	//}
	$query['order'][] = 'counter.totalcount desc';

	$q = afo_backend_buildquery($query);

	$pager_num = 0;
	$per_page = 100;

	$result = pager_query($q['sql'], $per_page, $pager_num, $q['count sql'], $q_args);
	while($row = db_fetch_array($result)) {
		$data[] = $row;
	}
	
	$headers = array(
		'ID #',
		array(
			'data' => 'Title',
		),
		array(
			'data' => 'User',
		),
		array(
			'data' => 'type',
		),
		array(
			'data' => 'Total',
		),
		array(
			'data' => 'Day',
		),
		array(
			'data' => 'Updated',
		),
	);

	if($data) {
	
		global $base_path;
	
		$rows = array();
		foreach($data as $row) {
			$new_row = array();
			$new_row[] = $row['nid'];
			$new_row[] = "<a href='{$base_path}node/{$row[nid]}'>{$row[title]}</a>";
			$new_row[] = "<a href='{$base_path}user/{$row[uid]}'>{$row[name]}</a>";
			$new_row[] = $row['type'];
			$new_row[] = $row['totalcount'];
			$new_row[] = $row['daycount'];
			$new_row[] = date('j-M-y', $row['timestamp']);
	
			$rows[] = $new_row;
		}

		$content['hits-table'] = array(
			'#type' => 'afo_contentbox',
			'#value' => theme('table', $headers, $rows),
		);
	
		$content['pager'] = array(
			'#value' => theme('pager', null, $per_page, $pager_num),
			'#weight' => 2,
		);
		
	}
	
	return drupal_render($content);

}

/**
 * Delete user confirm form
 */
function afo_editor_deleteuser($form_state, $uid) {
	if(!is_numeric($uid)) {
		drupal_set_message('No user id found.', 'error');
		return null;
	}
	if($uid == 1) {
		drupal_set_message('You cannot delete user #1.', 'error');
		return null;
	}

	$account = user_load($uid);
	
	$message[] = t('User\'s email address: @email', array('@email'=>$account->mail));
	$message[] = l('See user\'s page in a new tab', 'user/'.$account->uid, array('attributes'=>array('target'=>'_blank')));
	
	$q = db_query('SELECT nid, title FROM {node} WHERE uid = %d', $uid);
	while($row = db_fetch_array($q)) {
		$nodes[] = l($row['title'], 'node/'.$row['nid'], array('attributes'=>array('target'=>'_blank')));
	}
	if(is_array($nodes)) $message[] = theme('item_list', $nodes, 'User\'s posts:', 'ul', array('class'=>'arrow'));
	
	$form['message'] = array(
		'#type' => 'afo_contentbox',
		'#title' => t('Are you sure you want to delete user %name?', array('%name' => $account->name)),
		'#value' => theme('item_list', $message, null, 'ul', array('class'=>'arrow')),
		'#weight' => -5,
	);
	
	$form['uid'] = array(
		'#type' => 'value',
		'#value' => $uid,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete'),
	);

	$form['#redirect'] = 'editor/users-list';

	return $form;
}
/**
 * Delete user form submit
 */
function _afo_editor_deleteuser_submit($form, $form_state) {
	$uid = $form_state['values']['uid'];

	if(!is_numeric($uid)) {
		drupal_set_message('No user id found.', 'error');
		return null;
	}
	if($uid == 1) {
		drupal_set_message('You cannot delete user #1.', 'error');
		return null;
	}
	
	if(!user_access('delete users')) return null;
	
	/************************ go ahead with delete: **/

	// delete nodes
	$q = db_query('SELECT nid FROM {node} WHERE uid = %d', $uid);
	while($row = db_fetch_array($q)) {
		if(is_numeric($row['nid'])) node_delete($row['nid']);
	}
	
	// call drupal's user_delete function: deletes from {users}, {users_roles}, {authmap}, invokes user_delete hook, records in watchdog
	user_delete(array(), $uid);
}