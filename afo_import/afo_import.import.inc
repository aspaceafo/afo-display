<?php
// $Id$

/**
 * @file
 *
 * Import functions
 */
 
/******************************************** IMPORT FORM **/
function afo_import_page() {

	global $user;
	if(!$user->uid == 1) die('You are not allowed to access this page');

	// get data
	/*
	$q = "SELECT a.a_id, a.firstname, a.middlename, a.lastname, a.pass, a.user, a.country
		FROM artists a
		WHERE a.a_id >= 13900
		ORDER BY a.a_id ASC
		LIMIT 5";
	$a_data = afo_import_getExternalData($q);
	*/
	/*
		
			WHERE a.a_id >= 0
			WHERE a.country LIKE '%uk%'
	
	*/

	/*
	// create form
	$form = array();
	$form['#tree'] = true;
	
	if(!is_array($a_data)) {
		drupal_set_message('Artist query returned no results.');
		return null;
	}
	
	foreach($a_data as $a) {

		$rowId = $a['a_id'];
		
		// get image count
		$q = "SELECT COUNT(i.image_id) AS image_total FROM images i WHERE i.a_id = $rowId";
		$images = afo_import_getExternalData($q);
		$images_total = $images[0]['image_total'];
		
		// check if in db based on import records
		$record_exists = db_result(db_query('SELECT uid FROM {afo_import_record_users} WHERE source_id = %d', $rowId));
		
		// determine whether checked or not
		$check_import = true;
		if(!is_numeric($images_total) || $images_total == 0) $check_import = false;
		elseif(strlen($a['firstname']) == 0 && strlen($a['middlename']) == 0 && strlen($a['lastname']) == 0) $check_import = false; 
		elseif($record_exists) $check_import = false;
		
		$form['import-table'][$rowId]['import'] = array(
			'#type' => 'checkbox',
			'#return_value' => $rowId,
			'#default_value' => $check_import,
			'#attributes' => array('class'=>'import-selected'),
		);
		if(is_numeric($record_exists)) $exists_link = l($record_exists, 'user/'.$record_exists);
		else $exists_link = null;
		$form['import-table'][$rowId]['in-db'] = array(
			'#value' => $exists_link,
		);
		$form['import-table'][$rowId]['a_id'] = array(
			'#value' => $a['a_id'],
		);
		$form['import-table'][$rowId]['name'] = array(
			'#value' => implode(' ', array($a['firstname'], $a['middlename'], $a['lastname'], '('.$a['country'].')')),
			//'#value' => $a['lastname'] . ', ' . $a['firstname'] . ' ' . $a['user'] . ' [' . $a['pass'] .']', // show password
		);
		$form['import-table'][$rowId]['image-total'] = array(
			'#value' => $images_total,
		);
		$form['import-table'][$rowId]['import-link'] = array(
			'#value' => l('import', '', array('query'=>'a_id='.$a['a_id'], 'attributes'=>array('class'=>'import-do'))),
			'#suffix' => '<div class="import-do-results"></div>',
		);
		$form['import-table'][$rowId]['preview'] = array(
			'#value' => l('preview', '', array('query'=>'a_id='.$a['a_id'], 'attributes'=>array('class'=>'import-preview'))),
			'#suffix' => '<div class="import-preview-results"></div>',
		);
	
	}
	

	$form['import-batch'] = array(
		'#value' => l('Batch Import Selected', '', array('attributes'=>array('class'=>'import-batch'))),
	);
	*/
	/*
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Import selected',
	);
	*/
	
	$form['import-start'] = array(
		'#type' => 'textfield',
		'#title' => 'a_id start',
	);
	$form['import-end'] = array(
		'#type' => 'textfield',
		'#title' => 'a_id end',
	);
	$form['begin'] = array(
		'#value' => l('start import', '#', array('attributes'=>array('class'=>'start-import'))),
	);
	$form['results'] = array(
		'#prefix' => '<div id="import-status"></div>',
		'#value' => '<div id="import-results"></div>',
	);
	
	return $form;

}

function theme_afo_import_page($form) {	

	drupal_add_js(drupal_get_path('module', 'afo_import').'/afo_import.js');
	drupal_add_css(drupal_get_path('module', 'afo_import').'/afo_import.css');

	/*
	$rows = array();

	foreach (element_children($form['import-table']) as $rowId) {

		$element = &$form['import-table'][$rowId];

		$row = array();

		$row[] = drupal_render($element['import']);
		$row[] = drupal_render($element['in-db']);
		$row[] = drupal_render($element['a_id']);
		$row[] = drupal_render($element['name']);
		$row[] = drupal_render($element['image-total']);
		$row[] = drupal_render($element['import-link']);
		$row[] = drupal_render($element['preview']);

		if($element['in-db']['#value']) $class = 'in-db';
		else $class = '';
		
		$rows[] = array(
			'data' => $row,
			'id' => 'a_id-' . $rowId,
			'class' => $class,
		);
	}
	
	$header = array(t('?'), t('uid'), t('a_id'), t('Name'), t('imgs'), '', '');
	$output = theme('table', $header, $rows, array('class'=>'afo-import-table'));
	*/
	$output .= drupal_render($form);

	return $output;
			
}

/******************************************** EXTERNAL DB OPERATIONS **/

// the query
function afo_import_build_query() {
	
		$query =
			"SELECT a.a_id, a.u_id, a.user, a.pass, a.create_date, a.firstname, a.middlename, a.lastname, a.city, a.state, a.country, a.bio, a.statement_of_work, a.site, a.highlight_image 
			FROM artists a
			WHERE a_id > 21
			ORDER BY a.a_id ASC 
			LIMIT 1";
	
	/*
		laszlo a.firstname = 'laszlo'
		x betts a.a_id = 2337
		x Azadeh a_id = 1733
		x Barbara bernstein a_id=9889
		x Jhina Alvarado a_id=9178
		Edward Lee a_id=12484
		x Aerschmann a_id=3255
		x evi abeler a_id=3026
		x Hronn Axelsdottir = 5063
		x Brea Souders = 6684
		
		a.state = 'Q'
		
			WHERE a_id > 10
			ORDER BY a.a_id ASC 
			LIMIT 10";
		Imported 1-20

	*/
	
	
	return $query;
}

/******************************************** EXTERNAL DB OPERATIONS **/
function afo_import_do() {

	$skip_existing = true;

	// make sure user 1
	global $user;
	if(!$user->uid == 1) return null;

	// get id from POST
	$a_id = $_POST['a_id'];
	if(!is_numeric($a_id)) return null;
		
	// set options from POST or default 
	$op = isset($_POST['op']) ? $_POST['op'] : 'preview';

	// connect to db
	$link = mysql_connect ('66.178.176.123', 'cleardev2_afo', 'letha38');
	if(!$link) return("Couldn't connect to MySql");
	mysql_select_db ('cleardev2_afo');
	mysql_set_charset('utf8', $link); 

	// get external artist data
	$q = "SELECT a.* FROM artists a WHERE a_id = $a_id LIMIT 1";
	$query = mysql_query($q);
	while ($row = mysql_fetch_array($query)) {
		$a_data[] = $row;
	}
	
	if(!is_array($a_data)) {
		$return_data['result'] = $a_id . ' not imported: no artist info found';
		$return_data['imported'] = false;
	}
	else {
		
		// image count
		$q = "SELECT COUNT(i.image_id) AS image_total FROM images i WHERE i.a_id = $a_id";
		$i_query = mysql_query($q);
		$images_total = mysql_fetch_array($i_query);
		$images_total = $images_total['image_total'];
		
		// check existing
		$record_exists = db_result(db_query('SELECT uid FROM {afo_import_record_users} WHERE source_id = %d', $a_id));
		
		if(!$images_total || $images_total === 0) {
			$return_data['result'] = 'not imported: no images';
			$return_data['imported'] = false;
		}
		elseif(is_numeric($record_exists) && $skip_existing) {
			$return_data['result'] = 'not imported: uid '.$record_exists;
			$return_data['imported'] = false;
		}
		else {	
			// create user object
			module_load_include('inc', 'afo_import', 'afo_import.classes');
			$u = new ImportUser($a_data[0]);
			if(is_object($u)) {
			
				// perform op on object
				switch($op) {
				
					case 'preview':
						$return_data['preview'] = $u->print_data();
						break;
						
					case 'import':
						$u->import_do(true);
						$return_data['result'] = 'imported';
						$return_data['imported'] = true;
						break;
				
				}
			}	
			else {
				$return_data['result'] = 'not imported: import object failure';
				$return_data['imported'] = false;
			}
			
		}

	}

	/************************* DB close **/	
	mysql_close($link);		
		
	// return data
	if(is_array($return_data)) {
		// add artist data
		$return_data['summary'][] = $a_data[0]['a_id'];
		$return_data['summary'][] = implode(' ', array($a_data[0]['firstname'], $a_data[0]['middlename'], $a_data[0]['lastname']));
		$return_data['summary'][] = $a_data[0]['country'];
		$return_data['summary'][] = 'imgs: '.$images_total;
		$return_data['summary'][] = '<b>'.$return_data['result'].'</b>';
		$return_data['summary'] = implode(' | ', $return_data['summary']);
		if($return_data['imported']) $return_data['summary'] = '<span style="color:blue">'.$return_data['summary'].'</span>';
		drupal_json($return_data);
	}
	
}




