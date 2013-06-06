<?php
// $Id$

/**
 * @file
 *
 * Import classes
 */
 
/******************************************** CLASSES **/

/******************************************** base import class **/
class ImportObject
{
	public $obj_type = 'generic';
	public $in_db = false;
	public $obj_data = array();
	protected $obj_schema = array();
	protected $required_data = array();
	protected $import_record_table = null;
	protected $import_record_key = null;
	protected $import_record_source_table = null;
	protected $import_record_source_id = null;
	
	public function __construct($data = array()) {
		if(count($data) > 0) {
			$this->add_src_data($data);
			$this->check_exists();
		}
	}
	
	// add array to object data
	public function add_src_data($data) {
		if(is_array($data)) {
			foreach($data as $k => $v) {
				$this->process_src_data($k, $v);
			}
		}
	}
	// process new data
	protected function process_src_data($key, $value) {
		foreach($this->obj_schema as $schema_key => $scheme) {
			if(!is_numeric($key) && $scheme['source'] == $key) {
			
				$existing_value = $this->obj_data[$scheme['field']];
			
				switch($scheme['op']) {
					
					case 'clean':
						$new_value = $value;						
						break;

					case 'combine name':
						if(strlen($value) > 0) $new_value = $existing_value . ' ' . $value;
						else $new_value = $existing_value;
						break;
						
					case 'combine city':
						if(strlen($value) > 0 && $value != 'Q') $new_value = $existing_value . ' ' . $value;
						else $new_value = $existing_value;
						break;

					case 'timestamp':
						$new_value = strtotime($value);
						break;
						
					case 'strip tags':
						$new_value = strip_tags($value);
						break;
						
					case 'convert year':
						$new_value = substr($value, 0, 4);
						break;
						
					case 'check url':
						if(strlen($value) > 1) {
							$new_value = url($value, array('absolute' => true, 'external' => true));
							if(!strpos($new_value, '//')) $new_value = 'http://'.$new_value;
						}
						break;
						
					default:
						$new_value = $value;
				}
				
				// set source data value
				$this->obj_schema[$schema_key]['source value'] = $value;

				if(!$new_value && $scheme['default']) $new_value = $scheme['default'];

				// set object data value
				if($new_value) {
					//if(is_string($new_value)) $new_value = utf8_encode($new_value); // this is now being done by mysql call
					$new_value = trim($new_value);
					$this->obj_data[$scheme['field']] = $new_value;
					return true;
				}

				break;
			}
		}
		
		return false;
	}
	
	// output object data
	public function get_data() {
		return $this->obj_data;
	}
	
	// check if exists in Drupal db
	public function check_exists() {
		return false;
	}

	// find import history record
	public function check_import_records() {
		$source_id = $this->obj_data['source_id'];
		if(is_numeric($source_id) && $this->import_record_table && $this->import_record_key) {
			$result = db_result(db_query("SELECT %s FROM {%s} WHERE source_id = %d ORDER BY timestamp DESC", $this->import_record_key, $this->import_record_table, $source_id));
			if($result === 0) return false;
			return $result;
		}
		return false;
	}
	// record import in history table
	public function record_import() {
		if($this->import_record_table) {
		
			$imported_key = $this->import_record_key;
			$imported_value = $this->obj_data[$imported_key];

			if(is_numeric($imported_value)) {
				$record = new stdClass;
				$record->$imported_key = $imported_value;
				$record->source_table = $this->import_record_source_table; 
				$record->source_field = $this->import_record_source_id;
				$record->source_id = $this->obj_data['source_id'];
				$record->timestamp = time();
				
				return drupal_write_record($this->import_record_table, $record);
			}
		}
		return false;
	}
	
	// check if ready for import
	public function import_ready() {
		if($this->in_db) {
			$pkey = $this->obj_schema['primary key'];
			if(is_numeric($this->obj_data[$pkey])) return true;
		}
		elseif(count($this->required_data) > 0 ) {
			foreach($this->required_data as $reqkey) {
				if($this->obj_data[$reqkey] === null || $this->obj_data[$reqkey] == '') {
					return false;
				}
			}
			return true;
		}
		
		return false;	
	}
	
	
}

/******************************************** import user class **/

class ImportUser extends ImportObject
{
	/** define user attributes **/
	public $obj_type = 'user';
	protected $required_data = array('name', 'pass', 'mail',);
	protected $import_record_table = 'afo_import_record_users';
	protected $import_record_key = 'uid';
	protected $import_record_source_table = 'artists';
	protected $import_record_source_id = 'a_id';
	public $artwork = array();

	public function __construct($data = array()) {	
		/** define user schema **/
		$this->obj_schema['base table'] = 'users';	
		$this->obj_schema['primary key'] = 'uid';	
		
		/******** user fields *********/		
		$this->obj_schema['user id'] = array(
			'field' => 'uid',
		);
		$this->obj_schema['original id'] = array(
			'source' => 'a_id',
			'field' => 'source_id',
		);
		$this->obj_schema['email address'] = array(
			'source' => 'user',
			'field' => 'mail',
		);
		$this->obj_schema['first name'] = array(
			'source' => 'firstname',
			'field' => 'name',
			'op' => 'strip tags',
		);
		$this->obj_schema['middle name'] = array(
			'source' => 'middlename',
			'field' => 'name',
			'op' => 'strip tags',
		);
		$this->obj_schema['last name'] = array(
			'source' => 'lastname',
			'field' => 'name',
			'op' => 'strip tags',
		);
		$this->obj_schema['password'] = array(
			'source' => 'pass',
			'field' => 'pass',
			'op' => 'encode password',
		);
		$this->obj_schema['create date'] = array(
			'source' => 'create_date',
			'field' => 'created',
			'op' => 'timestamp',
		);

		/******** artwork info fields *********/
		$this->obj_schema['highlight'] = array(
			'source' => 'highlight_image',
			'field' => 'highlight',
		); 

		/******** taxonomy fields *********/
		$this->obj_schema['region'] = array(
			'source' => 'country',
			'destination' => 'taxonomy',
			'field' => 'Region',
			'op' => 'taxonomy',
		);
		$this->obj_schema['city'] = array(
			'source' => 'city',
			'destination' => 'taxonomy',
			'field' => 'City',
		);
		$this->obj_schema['state'] = array(
			'source' => 'state',
			'destination' => 'taxonomy',
			'field' => 'City',
			'op' => 'combine city',
		);

		/******** profile fields *********/
		$this->obj_schema['statement'] = array(
			'source' => 'statement_of_work',
			'destination' => 'profile',
			'field' => 'statement',
			'profile fid' => 2,
			'op' => 'clean',
		);
		$this->obj_schema['website'] = array(
			'source' => 'site',
			'destination' => 'profile',
			'profile fid' => 1,
			'field' => 'website',
			'op' => 'check url',
		);

		/******** cv fields *********/
		$this->obj_schema['cv'] = array(
			'source' => 'bio',
			'destination' => 'afo_cv_text',
			'field' => 'body',
			'op' => 'clean',
		);
		
		parent::__construct($data);
		
		// compile name
		if(strlen($this->obj_schema['first name']['source value'])>0) $full_name[] = $this->obj_schema['first name']['source value'];
		if(strlen($this->obj_schema['middle name']['source value'])>0) $full_name[] = $this->obj_schema['middle name']['source value'];
		if(strlen($this->obj_schema['last name']['source value'])>0) $full_name[] = $this->obj_schema['last name']['source value'];
		$full_name = implode(' ', $full_name);
		$this->obj_data['name'] = $full_name;		
		
		// create artwork import objects for this user
		$this->get_artwork();

		// get term data for this user
		$this->get_terms();
	}
	
	// check if exists in Drupal db
	public function check_exists() {
	
		// try import records first
		$found_uid = $this->check_import_records();
	
		// otherwise search db
		if(!is_numeric($found_uid)) {
			$email = $this->obj_data['mail'];
			$name = $this->obj_data['name'];
			$firstname = $this->obj_schema['first name']['source value'];
			$middlename = $this->obj_schema['middle name']['source value'];
			$lastname = $this->obj_schema['last name']['source value'];
	
			$found_user = user_load(array('mail' => $email)); // try email
			if(!$found_user) $found_user = user_load(array('name' => $name)); // try name
			if(!$found_user) $found_user = user_load(array('name' => "$firstname $lastname")); // try full name
			if(!$found_user) $found_user = user_load(array('name' => "$firstname $middlename $lastname")); // try full name
	
			// change to uid if object
			if(is_object($found_user)) $found_uid = $found_user->uid;
			// set array to value if only one
			if(count($found_uids)==1) $found_uid = $found_uids[0];
		}
		
		// return uid if user found
		if(is_numeric($found_uid) && $found_uid > 0) {
			$this->obj_data['uid'] = $found_uid;
			$this->in_db = true;
			return true;
		}	
	
		return false;
	}
	
	// get user artwork
	public function get_artwork() {
		$a_id = $this->obj_data['source_id'];
		if(is_numeric($a_id)) {
			$q = "SELECT * FROM images WHERE a_id = $a_id ORDER BY image_id ASC LIMIT 20";
			$this->artwork = afo_import_process_query($q, 'ImportArtwork');
		}
	}

	// get user terms
	public function get_terms() {
		$a_id = $this->obj_data['source_id'];
		if(is_numeric($a_id)) {
			// tags
			$q = "SELECT category_name AS name FROM artist_genre WHERE a_id = $a_id";
			$tags = afo_import_process_query($q, 'TaxonomyTerm');
			// add synonyms
			$synonyms = array(
				'gender/sexuality' => 'gender focused',
				'process oriented' => 'process based',
				'popular imagery' => 'pop culture',
				'minimal' => 'minimalism',
			);
			$tags = $this->add_term_synonyms($tags, $synonyms);
			$this->terms['tags'] = $tags;
			
			// medium
			$q = "SELECT medium_name AS name FROM artist_medium WHERE a_id = $a_id";
			$mediums = afo_import_process_query($q, 'TaxonomyTerm');
			// add synonyms
			$synonyms = array(
				'computer' => 'digital',
				'prints' => 'print making',
				'film-video' => 'film - video',
				'books' => 'book',
				'mural' => 'painting',
				'urban' => 'urbanism',
			);
			$mediums = $this->add_term_synonyms($mediums, $synonyms);
			$this->terms['medium'] = $mediums;
			
		}
	}
	
	// add synonyms to tags list
	public function add_term_synonyms($terms, $synonyms) {
		if(!is_array($terms) || !is_array($synonyms)) return false;
		
		$syn_terms = array();
		foreach($terms as $term) {
			$term_name = $term['name'];
			if(isset($synonyms[$term_name])) $syn_terms[] = array('name' => $synonyms[$term_name]); 
		}
		
		if(count($syn_terms) > 0) $terms = array_merge($terms, $syn_terms);
		
		return $terms;
	}

	// translate term
	public function translateTerm($term) {
		$term = strtolower($term);
		$term = trim($term);
		
		$t = array(
		
			'us' => 'United States of America', 
			'u.s.' => 'United States of America', 
			'usa' => 'United States of America', 
			'u.s.a.' => 'United States of America', 
			'united states' => 'United States of America', 
			'united states of america' => 'United States of America',
			'eeuu' => 'United States of America',
			'e.e.u.u.' => 'United States of America',
			'us of a' => 'United States of America',
			'u.s. of a.' => 'United States of America',
			
			'méxico' => 'Mexico',
			'brasil' => 'Brazil',
			
			'england' => 'United Kingdom',
			'uk' => 'United Kingdom',
			'u.k.' => 'United Kingdom',
			'scotland' => 'United Kingdom',
			'wales' => 'United Kingdom',
			
			'deutschland' => 'Germany',
			'gernany' => 'Germany',
			'danmark' => 'Denmark',
			'the netherlands' => 'Netherlands',
			'nederland' => 'Netherlands',
			'nederlands' => 'Netherlands',
			'holland' => 'Netherlands',
			'italia' => 'Italy',
			'italiy' => 'Italy',
			'españa' => 'Spain',
			'espana' => 'Spain',
			'slovenija' => 'Slovenia',
			'slovenija-eu' => 'Slovenia',
			'belgique' => 'Belgium',
						
			'rsa' => 'South Africa',
			'r.s.a.' => 'South Africa',
			'republic of south africa' => 'South Africa',
			
			'日本' => 'Japan',
			'south korea' => 'Republic of Korea',
			'north korea' => "Democratic People's Republic of Korea",
			'vietnam' => 'Viet Nam',
			
		);
		// try direct translation
		if($t[$term]) return $t[$term];
		
		// try included in translation
		$included = array(
			'ireland' => 'Ireland',
		);
		foreach($included as $i => $val) {
			if(strpos($term, $i) !== false) {
				return $val;
			}
		}
		
		return $term;
	}

	// print object page listing
	public function print_data() {
		$listing_classes[] = 'import-'.$this->obj_type;

		$listing[] = $this->obj_data['name'];
		$listing[] = 'Pass: '.$this->obj_data['password'];
		$listing[] = $this->obj_data['mail'];
		$listing[] = $this->obj_data['City'];
		$listing[] = $this->obj_data['Region'];
		//$listing[] = $this->obj_data['body'];
		
		if($this->in_db) $listing[] = 'In DB; uid = '.$this->obj_data['uid'];
		else $listing[] = 'Not in DB';
		
		if($this->import_ready()) $listing_classes[] = 'import-ready';
		else $listing_classes[] = 'no-import-ready';
		
		$listing_classes = implode(' ', $listing_classes);
		$listing_text = implode(' | ', $listing);
		$listing_lines[] =  "<div class='$listing_classes'>$listing_text</div>";
		
		foreach($this->artwork as $a) {
			$listing_lines[] = $a->print_data();
		}
		
		return implode('', $listing_lines);
	}

	// import user to drupal
	public function import_do($commit = false) {
		if($this->import_ready()) {

			// if existing user, set uid
			if(is_numeric($this->obj_data['uid'])) {
				$uid = $this->obj_data['uid'];
			}
			// if not existing, create new
			else {
				if($commit) {
					// add role
					$artist_role = variable_get('afo_custommenu_roles_artist', null);
					$this->obj_data['roles'] = array($artist_role => 'artist');
					
					// add status
					$this->obj_data['status'] = 1;
					
					$user = user_save('', $this->obj_data);
					$uid = $user->uid;
				}
			}

			// if we have a $uid, continue with import:
			if(is_numeric($uid) && $uid > 0) {
			
				// add to obj_data if not there
				if(!isset($this->obj_data['uid'])) $this->obj_data['uid'] = $uid; 
			
				// record import
				$this->record_import($uid);
			
				foreach($this->obj_schema as $elem) {

					/******** modifications *********/
					// add US if no region and recognizable US city
					if($elem['field'] == 'Region' && !$this->obj_data['Region']) {
					
						if(isset($this->obj_schema['state']['source value']) && $this->obj_schema['state']['source value'] != 'Q') {
							$this->obj_data['Region'] = 'United States of America';
						}
						
						else {
							$US_cities = array('new york', 'los angeles', 'chicago', 'boston', 'san francisco',);
							foreach($US_cities as $US_city) {
								if(strstr(strtolower($this->obj_data['City']), $US_city)) {
									$this->obj_data['Region'] = 'United States of America';
									break;
								}
							}
						}						
					}

					/******** import *********/
					// cv
					if($elem['destination'] == 'afo_cv_text' && strlen($this->obj_data[$elem['field']]) > 0) {
						// create record
						$record = new stdclass;
						$record->uid = $uid;
						$record->body = $this->obj_data[$elem['field']];
					
						// check existing
						$exists = db_result(db_query('SELECT uid FROM {afo_cv_text} WHERE uid = %d', $uid));
						if($commit) {
							if($exists) drupal_write_record('afo_cv_text', $record, 'uid');
							else drupal_write_record('afo_cv_text', $record);
						}
					}

					// profile
					if($elem['destination'] == 'profile' && is_numeric($elem['profile fid'])) {
						$field_value = isset($this->obj_data[$elem['field']]) ? $this->obj_data[$elem['field']] : null;
						if($field_value) {
							// check if profile value already exists:
							$exists = db_result(db_query('SELECT fid FROM {profile_values} WHERE fid = %d AND uid = %d', $elem['profile fid'], $uid));
							
							$record = new stdclass;
							$record->uid = $uid;
							$record->fid = $elem['profile fid'];
							$record->value = $field_value;
							if($commit) {
								if($exists) drupal_write_record('profile_values', $record, array('uid', 'fid'));
								else drupal_write_record('profile_values', $record);
							}
						}
					}					
					// region & city
					else if($elem['destination'] == 'taxonomy' && strlen($this->obj_data[$elem['field']]) > 0) {
	
						// get vid
						$vid = db_result(db_query('SELECT vid FROM {vocabulary} WHERE name = \'%s\'', $elem['field']));
						if(is_numeric($vid)) {
	
							// check if already term assigned
							$existing = db_result(db_query('SELECT u.tid FROM {term_user} u 
							LEFT JOIN {term_data} d ON d.tid = u.tid 
							WHERE d.vid = %d AND u.uid = %d', $vid, $uid));
							if(!$existing) {
								
								$term_name = $this->obj_data[$elem['field']];
								
								// special conversions
								if($elem['field'] == 'Region') {
									// convert US term
									//$usa_names = array('us', 'u.s.', 'usa', 'u.s.a.', 'united states', 'united states of america');
									//if(in_array(strtolower($term_name), $usa_names)) $term_name = 'United States of America';
									
									$term_name = $this->translateTerm($term_name);
								}
	
								// try to find tid
								$term_tid = db_result(db_query('SELECT tid FROM {term_data} WHERE name LIKE \'%%%s%%\' AND vid = %d', $term_name, $vid));
	
								// if no tid, create if allowed
								$create_new_allowed = array('City');
								if(!is_numeric($term_tid) && in_array($elem['field'], $create_new_allowed)) {
									// create new term
									$newTerm = array(
										'vid' => $vid, 
										'name' => $term_name,
									);
									taxonomy_save_term($newTerm);
									if(is_numeric($newTerm['tid'])) $term_tid = $newTerm['tid'];
								}
								
								// tid exists, so inset into term data
								if(is_numeric($term_tid)) {
									$record = new stdclass;
									$record->tid = $term_tid;
									$record->uid = $uid;
									if($commit) drupal_write_record('term_user', $record);
								}

							} // end if a term already assigned for this vocab
						} // end if vid						
					} // end if destination = taxonomy
				} // end if foreach obj_schema
				
				// add terms
				foreach($this->terms as $term_type => $term_set) {
					if(is_array($term_set)) {
						foreach($term_set as $term) {
							$term_name = $term['name'];
							if(strlen($term_name) > 0) {
								// load matching term data
								
								// get term data
								$query = db_query('SELECT tid, vid FROM {term_data} WHERE name LIKE \'%%%s%%\'', $term_name);
								while($term_data = db_fetch_object($query)) {
									if(is_numeric($term_data->tid)) {
										
										// check if existing already
										$exists = db_result(db_query('SELECT tid FROM {term_user} WHERE tid = %d AND uid = %d', $term_data->tid, $uid));
										
										// if doesn't already exist
										if(!$exists) {
											$allowed_per_vid = array(
												3 => 3,
												5 => 5,
												7 => 20,
											);
											
											// see how many terms user has of current vocabulary
											$check_vid = db_result(db_query('SELECT COUNT(u.tid) total 
												FROM {term_user} u 
												LEFT JOIN {term_data} d ON d.tid = u.tid  
												WHERE d.vid = %d AND u.uid = %d
												', $term_data->vid, $uid));
													
											if($check_vid <= $allowed_per_vid[$term_data->vid]) {
												$record = new stdclass;
												$record->tid = $term_data->tid;
												$record->uid = $uid;
												if($commit) drupal_write_record('term_user', $record);
												$added_terms[] = $term_data->name;
											}
										}	
									}
								}
							}
						}
					}
				} // end add terms
				
				// add artwork
				foreach($this->artwork as $artwork) {
					$result = $artwork->import_do($uid);
				}
				
				
				/****************************** secondary operations **/
				
				// assign weight to highlight image so it is first in portfolio
				if(is_numeric($this->obj_data['highlight']) && module_exists('afo_portfolio')) {
					$artwork_nid = db_result(db_query('SELECT nid FROM {afo_import_record_artwork} WHERE source_id = %d', $this->obj_data['highlight']));
					if(is_numeric($artwork_nid)) {
						afo_portfolio_putstructure($artwork_nid, -1, 0, 'check');
					}
				}

				
				// finished with import	

			} // end if $uid
			
		}
	}
	
}

/******************************************** import artwork class **/

class ImportArtwork extends ImportObject
{
	/** define artwork attributes **/
	public $obj_type = 'artwork';
	protected $required_data = array('title');
	protected $import_record_table = 'afo_import_record_artwork';
	protected $import_record_key = 'nid';
	protected $import_record_source_table = 'images';
	protected $import_record_source_id = 'image_id';

	public function __construct($data = array()) {	
		/** define artwork schema **/
		$this->obj_schema['base table'] = 'node';	
		$this->obj_schema['primary key'] = 'nid';	
		$this->obj_schema['node id'] = array(
			'field' => 'nid',
		);
 		$this->obj_schema['original id'] = array(
			'source' => 'image_id',
			'field' => 'source_id',
		);		
 		$this->obj_schema['artist id'] = array(
			'source' => 'a_id',
			'field' => 'a_id',
		);		
		$this->obj_schema['title'] = array(
			'source' => 'title',
			'field' => 'title',
			'node field' => true,
			'default' => 'untitled',
			'required' => true,
		);
		$this->obj_schema['type'] = array(
			'field' => 'type',
			'value' => 'artwork',
		);
		$this->obj_schema['user id'] = array(
			'field' => 'uid',
			'node field' => true,
		);
		$this->obj_schema['body'] = array(
			'source' => 'description',
			'field' => 'body',
			'node field' => true,
		);
		$this->obj_schema['create date'] = array(
			'source' => 'date_updated',
			'field' => 'created',
			'op' => 'timestamp',
			'node field' => true,
		);
		$this->obj_schema['medium'] = array(
			'source' => 'media',
			'field' => 'medium',
			'node field' => 'artwork_details_materials',
		);
		$this->obj_schema['dimensions'] = array(
			'source' => 'size',
			'field' => 'dimensions',
			'node field' => 'artwork_details_dimensions',
		);
		$this->obj_schema['year'] = array(
			'source' => 'image_date',
			'field' => 'year',
			'op' => 'convert year',
			'node field' => 'artwork_details_year',
		);
		
		parent::__construct($data);
	}
	
	// check if exists in Drupal db
	public function check_exists() {

		// try import records first
		$found_nid = $this->check_import_records();
		
		// return nid if node found
		if(is_numeric($found_nid)) {
			$exists = db_result(db_query('SELECT status FROM {node} WHERE nid = %d', $found_nid));
			if($exists) {
				$this->obj_data['nid'] = $found_nid;
				$this->in_db = true;
				return true;
			}
		}	
	
		return false;
	}

	// print object page listing
	public function print_data() {
		$listing_classes[] = 'import-'.$this->obj_type;

		$listing[] = $this->obj_data['title'];

		$listing[] = 'Source id: ' . $this->obj_data['source_id'];

		if($this->in_db) $listing[] = 'In DB; nid = '.$this->obj_data['nid'];
		else $listing[] = 'Not in DB';

		//$listing[] = $this->get_image_location();
		
		if($this->import_ready()) $listing_classes[] = 'import-ready';
		else $listing_classes[] = 'no-import-ready';
		
		$listing_classes = implode(' ', $listing_classes);
		$listing_text = implode(' | ', $listing);		
		return "<div class='$listing_classes'>$listing_text</div>";
	}
	
	// get image file
	public function get_image_location() {
		$a_id = $this->obj_data['a_id'];
		$image_id = $this->obj_data['source_id'];
		if(is_numeric($a_id) && is_numeric($image_id)) {
			return "/usr/home/aspace/domains/artistsspace.org/public_html/afonline/artist/{$a_id}/images/{$image_id}.jpg"; 			
		}
	}

	// import artwork to drupal
	public function import_do($uid) {
		if(!is_numeric($uid)) return false;
	
		$this->obj_data['uid'] = $uid;
		$user_name = db_result(db_query('SELECT name FROM {users} WHERE uid = %d', $uid));
	
		$artworks_count = db_result(db_query('SELECT COUNT(nid) AS total FROM {node} WHERE uid = %d AND type = \'%s\'', $uid, 'artwork'));
	
		if($this->import_ready() && !$this->in_db && strlen($user_name) > 0 && $artworks_count < 12) {

			// move image file and create file record
			$oldFile = $this->get_image_location();

			if(is_file($oldFile)) {
	
				// old path:
				//$full_dir = '/usr/home/aspace/domains/artistsspace.org/public_html/afonline2/sites/default/files/';				
				
				//global $base_path;
				//$full_dir = '/usr/home/aspace/domains/local-artists.org/public_html' . $base_path . 'sites/default/files/';

				$full_dir = '/usr/home/aspace/domains/local-artists.org/public_html/prod/sites/default/files/';
				
				$images_folder_relative = "users/$uid/images";
				$images_folder = $full_dir.$images_folder_relative;

				if(!file_check_directory($images_folder)) {
					$dir = mkdir($images_folder, 0755, true);
				}
				if(file_check_directory($images_folder, 0, 'error')) {
					$newFile = $images_folder_relative . '/' . basename($oldFile);
					$source = $oldFile;
					$copy_result = file_copy($source, $newFile, FILE_EXISTS_REPLACE);
				}

				if($copy_result && $source != $oldFile) {
				
					$newFile = $source;

					// check if file record exists
					$fid = db_result(db_query('SELECT fid FROM {files} WHERE filepath = \'%s\'', $newFile));
					if(!$fid) {
						// write file record to {files}
						$file->filename = basename($newFile);
						$file->filepath = $newFile;
						$file->filemime = file_get_mimetype($newFile);
						$file->filesize = filesize($newFile);
						$file->uid = $this->obj_data['uid'];
						$file->status = FILE_STATUS_PERMANENT;
						$file->timestamp = time();
						$success = drupal_write_record('files', $file);
		
						$fid = $file->fid;
					}
					
					if(is_numeric($fid)) {

						// check if node exists
						$exists = db_result(db_query('SELECT nid FROM {content_type_artwork} WHERE field_artwork_image_fid = %d', $fid));
						if(!$exists) {

							if(!is_object($file)) {
								$query = db_query('SELECT * from {files} WHERE fid = %d', $fid);
								$file = db_fetch_object($query);
							}
							
							if(is_object($file)) {

								// Create a new artwork node
								$form_state = array();
								module_load_include('inc', 'node', 'node.pages');
								$node = new stdclass; 
								$node->type = 'artwork';
								
								// add values based on schema
								foreach($this->obj_schema as $elem) {
									if($elem['node field']) {
										$node_field = is_bool($elem['node field']) ? $elem['field'] : $elem['node field'];
										if(isset($this->obj_data[$elem['field']])) {
											$form_state['values'][$node_field] = $this->obj_data[$elem['field']];
										}
									}
								}
											
								//add image field
								$form_state['values']['field_artwork_image'] = array(
									array(
									  'fid' => $file->fid,
									  'title' => basename($file->filename),
									  'filename' => $file->filename,
									  'filepath' => $file->filepath,
									  'filesize' => $file->filesize,
									  'mimetype' => $file->filemime,
									  'description' => 'Image imported from old AFOnline',
									  'list' => 1,
									),
								);
			
								// additional necessary fields
								$form_state['values']['type'] = 'artwork';
								$form_state['values']['name'] = $user_name;
								
								if(strlen($form_state['values']['title']) > 0) {

									// ugh! create node manually
									$node->title = $this->obj_data['title'];
									$node->created = $this->obj_data['created'];
									//$node->body = $this->obj_data['body'];
									$node->uid = $uid;
									$node->status = 1;
									$node->is_new = true;
									node_save($node);
									
									if(is_numeric($node->nid)) {
									
										// add nid to obj_data
										$this->obj_data['nid'] = $node->nid;
										$this->obj_data['vid'] = $node->vid;
									
										$artwork_details = new stdclass;
										$artwork_details->vid = $node->vid;
										$artwork_details->nid = $node->nid;
										$artwork_details->field_artwork_image_fid = $file->fid;
										$artwork_details->field_artwork_image_list = 1;
										$artwork_details->field_artwork_image_data = 'a:3:{s:11:"description";s:0:"";s:3:"alt";s:0:"";s:5:"title";s:0:"";}';
										$content_success = drupal_write_record('content_type_artwork', $artwork_details, 'vid');

										// record import
										$this->record_import();
										
									} // end if node success
									
								} // end if title exists

							} // end if is object ($file)

						} // end if node doesn't exist
					
					} // end if $fid
					
				} // end if $copy_result

			} // end if is_file
		
		} // end if import_ready
		
		/****************************** secondary operations **/
		// if we have an nid (either node was already in db or it was successfully created) 
		if($this->import_ready() && $this->obj_data['nid'] && $this->obj_data['vid']) {
		
			$nid = $this->obj_data['nid'];
			$vid = $this->obj_data['vid'];
			
			/*************** NODE DATA BODY revisions table **/
			$bodyText = trim(strip_tags($this->obj_data['body']));
			$rev = new stdclass;
			$rev->nid = $nid;
			$rev->vid = $vid;
			$rev->body = $bodyText;
			$rev->teaser = $bodyText;
			$rev_success = drupal_write_record('node_revisions', $rev, 'vid');

			
			/*************** PORTFOLIO ARTWORK DATA **/
			if(isset($this->obj_data['medium'])) $artwork_data['materials'] = $this->obj_data['medium'];
			if(isset($this->obj_data['year'])) $artwork_data['year'] = $this->obj_data['year'];
			if(isset($this->obj_data['dimensions'])) $artwork_data['dimensions'] = $this->obj_data['dimensions'];
			if(is_array($artwork_data) && module_exists('afo_portfolio')) {
				module_load_include('inc', 'afo_portfolio', 'afo_portfolio.artwork');
				afo_portfolio_setartworkdata($nid, $artwork_data);
			}
		
			/*************** TAXONOMY TERMS **/
			// old db doesn't assign terms to artwork, so we'll have to copy from artist
			
			$artwork_vids = array(3, 4, 5);
			
			// get user's terms
			$query = db_query('SELECT u.tid, d.vid FROM {term_user} u 
			LEFT JOIN {term_data} d ON d.tid = u.tid
			WHERE u.uid = %d AND (d.vid = %d OR d.vid = %d OR d.vid = %d)
			', $uid, $artwork_vids[0], $artwork_vids[1], $artwork_vids[2]);
			$user_terms = array();
			while($row = db_fetch_object($query)) { 
				$user_terms[] = $row;
			}
			
			if(is_array($user_terms)) {
				foreach($user_terms as $term_data) {
					// check if term already assigned
					$existing = db_result(db_query('SELECT tid FROM {term_node} WHERE nid = %d AND tid = %d', $nid, $term_data->tid));
					// if not:
					if(!$existing) {
						// check how many terms assigned for this vocabulary
						$vid_count = db_result(db_query('SELECT COUNT(t.tid) AS total FROM {term_node} t LEFT JOIN {term_data} d ON d.tid = t.tid WHERE t.nid = %d AND d.vid = %d', $nid, $term_data->vid));
						// if under limit
						if($vid_count < 3) {
							// add term_node
							$record = new stdclass;
							$record->tid = $term_data->tid;
							$record->nid = $nid;
							$record->vid = $nid;
							$success = drupal_write_record('term_node', $record);
						}
					}
				}			
			}
		
		}
		
	} // end function import_do
	
} // end class definition