<?php
// $Id$

/**
 * @file
 *
 * Blog page functions
 */

/**
 * Build blog page
 */
function afo_custommenu_blogpage($options = array()) {

	// defaults
	if(!isset($options['display'])) $options['display'] = 'normal';
	if(!is_numeric($options['pager num'])) $options['pager num'] = 0;
	if(!is_numeric($options['per page'])) $options['per page'] = 10;
	
	$sql = 'SELECT DISTINCT(n.nid) FROM {node} n WHERE n.status = %d AND n.type = \'%s\' ORDER BY n.created DESC';
	$sql_count = 'SELECT COUNT(n.nid) FROM {node} n WHERE n.status = %d AND n.type = \'%s\' ORDER BY n.created DESC';
	$args = array(1, 'story');
	
	$query = pager_query($sql, $options['per page'], $options['pager num'], $sql_count, 1, 'story');
	
	while($result = db_fetch_object($query)) {
		$blog_entries[] = node_load($result->nid);	
	}
	
	if(!is_array($blog_entries)) return '';

	foreach($blog_entries as $b) {
	
		switch($options['display']) {
			case 'short':
				$blog_entry = '';
				$blog_entry .= '<div class="blog-image">' . theme('imagecache', 'artwork_tiny', $b->field_images[0]['filepath']) . '</div>';
				$blog_entry .= l($b->title, 'community/blog');
				$blog_entry .= '<div class="secondaryText">' . substr($b->body, 0, 75) . '...</div>';
				$blog[] = $blog_entry;
				break;
			
			default:
				$blog[] = node_view($b, false, true, false);
				break;
		}
		
	}
	
	if(user_access('create story content')) {
		$content['create-new'] = array(
			'#value' => l(t('Add new blog post'), 'node/add/story'),
			'#weight' => -5,
		);
	}
	
	$content['blog'] = array(
		'#value' => theme('item_list', $blog, null, 'ul', array('class'=>'afo-blog-list blog-display-'.$options['display'])),
	);
	
	if(!$options['no pager']) {
		$content['pager'] = array(
			'#value' => theme('pager', $sql_count, $per_page, $pager_num),
		);
	}
	
	return drupal_render($content);
	
}

/**
 * Modify blog node content for display
 */
function afo_custommenu_modify_blog_content(&$node) {	
	// left column
	$node->content['col-left'] = array(
		'#type' => 'afo_column',
		'#position' => 'left',
		'#width' => '250px',
		'#weight' => -1,
	);
	$node->content['col-left']['field_images'] = $node->content['field_images']; unset($node->content['field_images']);

	// right column
	$node->content['col-right'] = array(
		'#type' => 'afo_column',
		'#position' => 'right',
		'#width' => '300px',
		'#weight' => 1,
	);
	$node->content['col-right']['title'] = array(
		'#value' => '<h1 class="title">'.$node->title.'</h1>',
		'#weight' => -10,
	);
	$node->content['col-right']['post-date'] = array(
		'#prefix' => '<div class="secondaryText">',
		'#value' => t('Posted by ').$node->name.', '.format_date($node->created, 'small'),
		'#suffix' => '</div>',
		'#weight' => -9,
	);
	if(node_access('update', $node)) {
		// if access, add edit link
		$node->content['col-right']['edit-link'] = array(
			'#value' => l(t('(edit entry)'), 'node/'.$node->nid.'/edit'),
			'#weight' => -8,
		);
	}
	$node->content['col-right']['body'] = $node->content['body']; unset($node->content['body']);

	$node->content['clear'] = array(
		'#value' => '<div class="horiz-clear">&nbsp;</div>',
		'#weight' => 15,
	);
}