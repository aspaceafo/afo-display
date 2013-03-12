<?php
// $Id$

/**
 * @file
 *
 * Thumbnail theme functions
 */
 
/**
 * Node thumbnail
 */
function theme_afo_display_node_thumbnail($data, $imagecache_preset=null, $path=null, $options=array()) {
	if(!$imagecache_preset) $imagecache_preset = variable_get('afo_display_thumbnail_default_imagecache', 'artwork_thumbnail');
	
	global $user;
	
	if(is_object($data) && is_numeric($data->nid)) {
		$node_data = $data;
		$nid = $node_data->nid;
	}
	elseif(is_array($data) && is_numeric($data['nid'])) {
		$node_data = (object) $data;
		$nid = $node_data->nid;
	}
	elseif(is_numeric($data)) {
		$nid = $data;
	
		$node_type = db_result(db_query('SELECT type FROM {node} WHERE nid = %d', $nid));
		
		if($node_type == 'exhibition') {
			// get exhibition node data
			$query = db_query('
				SELECT n.title, u.name
				FROM {node} n
				LEFT JOIN {users} u ON u.uid = n.uid
				WHERE n.nid = %d
			', $nid);	
			$node_data = db_fetch_object($query);
	
			// get first artwork nid
			$items = afo_exhibition_getitems($nid);
			$first_nid = $items[0]->item_nid;
			if(!is_numeric($first_nid)) return null;
			$query = db_query('
				SELECT files.filepath
				FROM {content_type_artwork} artwork
				INNER JOIN {files} files ON artwork.field_artwork_image_fid = files.fid
				WHERE artwork.nid = %d
			', $first_nid);	
			$node_data->nid = $first_nid;
			$node_data->filepath = db_result($query);
			
			$altText = $node_data->title . t(' (exhibition)');		
			$label = t('Exhibition');
	
		} else if($node_type == 'open_call' || $node_type == 'open call') {
	
			$query = db_query('
				SELECT n.title, u.name, files.filepath
				FROM {node} n
				LEFT JOIN {users} u ON u.uid = n.uid
				LEFT JOIN {content_field_image} im ON im.nid = n.nid
				LEFT JOIN {files} files ON im.field_image_fid = files.fid
				WHERE n.nid = %d
			', $nid);
	
			$node_data = db_fetch_object($query);
			$node_data->nid = $nid;
	
			if(!$node_data->filepath) {
				$node_data->filepath = theme('afo_display_icon', 'open call', array('path only' => true));
			}
	
			$altText = $node_data->title . t(' (open call)');
			$label = t('Open Call');
	
		} else if($node_type == 'event') {
	
			$query = db_query('
				SELECT n.title, u.name, files.filepath
				FROM {node} n
				LEFT JOIN {users} u ON u.uid = n.uid
				LEFT JOIN {content_field_image} im ON im.nid = n.nid
				LEFT JOIN {files} files ON im.field_image_fid = files.fid
				WHERE n.nid = %d
			', $nid);
	
			$node_data = db_fetch_object($query);	
			$node_data->nid = $nid;
			
			if(!$node_data->filepath) {
				$node_data->filepath = theme('afo_display_icon', 'event', array('path only' => true));
			}
			
			$altText = $node_data->title . t(' (event)');
			$label = t('Event');
	
		} else {
			// artwork type
			$query = db_query('
				SELECT files.filepath, artwork.field_artwork_video_embed AS video, n.title, u.name
				FROM {content_type_artwork} artwork
				INNER JOIN {files} files ON artwork.field_artwork_image_fid = files.fid
				INNER JOIN {node} n ON artwork.nid = n.nid
				INNER JOIN {users} u ON u.uid = n.uid
				WHERE artwork.nid = %d
			', $nid);
			$node_data = db_fetch_object($query);
			$node_data->nid = $nid;
	
			$altText = '\''.$node_data->title.'\', '.t('by ').$node_data->name;
	
			if($options['user thumbnail']) {
				$label = $node_data->name;
			}
	
		}

	}
		
	if(isset($node_data->image) and !isset($node_data->filepath)) $node_data->filepath = $node_data->image;
	if(isset($node_data->label)) $label = $node_data->label;
	
	if($node_data->filepath) {
		// classes
		$addClasses = array('thumbnail');
		
		// path default
		if(!$path) $path = "node/$nid";
		
		// queue class
		if(module_exists('afo_queue')) $queue_class = afo_queue_queueable_classes($nid, 'node');
		
		// get image
		$image = theme('imagecache', $imagecache_preset, $node_data->filepath, $altText, $altText, array('class'=>$queue_class));

		// video icon
		if($node_data->video && !$label) {
			//$addClasses[] = 'video-thumbnail';
			//global $base_path; 
			//$images_path = $base_path.drupal_get_path('module', 'afo_display').'/images';
			//$icon = "<div class='icon-video'><img src='$images_path/Arrow-Play.png' /></div>";
			//$image = $icon . $image;

			$label = 'video';
		}
		
		// label/icon
		if(!$options['no label'] && $label) {
			$image = _afo_display_thumbnail_label($label) . $image;
		}
		
		// make link
		global $base_path;		
		if($path == 'none') $return_thumbnail = '<span class="'.implode(' ', $addClasses).'">'.$image.'</span>';
		//else $data = l($image, $path, array('html'=>'true', 'attributes' => array('class' => implode(' ', $addClasses))));
		else $return_thumbnail = "<a href='{$base_path}$path' class='".implode(' ', $addClasses)."'>$image</a>";
	
	}
	
	return $return_thumbnail;
}

/**
 * User thumbnail
 */
function theme_afo_display_user_thumbnail($user_data, $imagecache_preset=null, $path=null, $options=array()) {
	if(is_numeric($user_data)) $uid = $user_data;
	elseif(is_numeric($user_data['uid'])) $uid = $user_data['uid'];
	
	global $base_path;
	
	if(!is_numeric($uid)) return null;
	
	if(!$imagecache_preset) $imagecache_preset = variable_get('afo_display_thumbnail_default_imagecache', 'artwork_thumbnail');
	
	if(module_exists('afo_portfolio')) $data = afo_portfolio_thumbnail($user_data, $imagecache_preset, $options);
	
	if(!$data) {

		// get user name
		if(isset($user_data['name'])) $user_name = $user_data['name'];
		else $user_name = db_result(db_query('SELECT name FROM {users} WHERE uid = %d', $uid));
		
		// default thumbnail
		$img = theme('afo_display_icon', 'user', array('alt' => $user_name));
		//$data = l($img, 'user/'.$uid, array('html'=>true, 'attributes'=>array('class'=>'user thumbnail')));
		$data = "<a href='{$base_path}user/$uid' class='user thumbnail'>$img</a>";
	}

	return $data;
}

/**
 * Create label div
 */
function _afo_display_thumbnail_label($text) {
	$text = substr($text, 0, 60);
	$label = '<div class="thumbnail-label">'.$text.'</div>';
	return $label;
}