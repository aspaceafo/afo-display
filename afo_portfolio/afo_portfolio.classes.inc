<?php
// $Id$

/**
 * @file
 *
 * AFO Portfolio module class definitions.
 */
 
/** ********************************************** SUPER CLASS **/
class portfolioItem
{
	public $itemType;
	public $nid;
	public $structure = array('folder_nid' => null, 'weight' => null);
	public $data = array('title' => null);
	
	public function __construct($args = array()) {
		if(is_array($args)) {
			// nid
			$this->nid = $args['nid'];
			
			// get structure values from args
			foreach($this->structure as $k => $v) {
				$this->structure[$k] = $args[$k];
			}
			// get data values from args
			foreach($this->data as $k => $v) {
				$this->data[$k] = $args[$k];
			}
			
			// see if structure values null
			if($this->structure['folder_nid'] === null && $this->structure['weight'] === null) {
				$this->getStructure();
			}
		}
	}
	
	protected function getStructure() {
		if(!is_numeric($this->nid)) return null;

		$q = db_query('SELECT folder_nid, weight FROM {afo_portfolio_structure} WHERE nid = %d', $this->nid);
		if($result = db_fetch_object($q)) {
			$this->structure['folder_nid'] = $result->folder_nid;
			$this->structure['weight'] = $result->weight;
		}
	}
	
	public function printItem($op = 'normal') {
		if(!is_numeric($this->nid)) return null;

		return $this->nid;
	}

	public function hierarchyCollapse(&$collapsed = array(), $current_depth = 0) {
		$collapsed[] = $this;
		if(is_array($this->children)) {
			foreach($this->children as $child) {
				$child->structure['depth'] = $current_depth;
				$next_level = $child->hierarchyCollapse($collapsed, $current_depth + 1);
				array_merge($collapsed, $next_level);
			}
		}
		//unset($this->children);
		return $collapsed;		
	}
	
}


/** ********************************************** ARTWORK CLASS **/
class portfolioArtwork extends portfolioItem
{
	public $itemType = 'artwork';
	public $data = array(
		'title' => null,
		'created' => null,
	);
	
	public function __construct($args = array()) {
		parent::__construct($args);
	}

	public function printItem($op='normal', $options = array()) {
		if(!is_numeric($this->nid)) return null;
		
		global $base_path;

		// variables used in all types
		$dest = 'node/'.$this->nid;
		
		// determine imagecache preset
		$imagecache_preset = isset($options['imagecache_preset']) ? $options['imagecache_preset'] : variable_get('afo_portfolio_thumbnail_default_preset', null); 

		// output
		$output = '';

		if($op == 'portfolio thumbnail') {
			$uid = $options['uid'];
			$output .= "<div class='portfolio-artwork'><div class='inner'>";
			$output .= theme('afo_display_node_thumbnail', $this->nid, $imagecache_preset, 'user/'.$uid, $options);			
			$output .= '</div></div>';
		}

		else {
			$output .= "<div class='portfolio-artwork'><div class='inner'>";
			$output .= theme('afo_display_node_thumbnail', $this->nid, $imagecache_preset, $options);
			//$output .= '<div class="portfolio-title">'.l($this->data['title'], $dest).'</div>';
			$output .= '<div class="portfolio-title"><a href="'.$base_path.$dest.'">'.$this->data['title'].'</div>';
			$output .= '</div></div>';
		}
		
		return $output;
	}
}


/** ********************************************** FOLDER CLASS **/
class portfolioFolder extends portfolioItem
{
	public $itemType = 'folder';
	public $data = array('title' => null, 'body' => null);
	public $children = array();
	
	public function __construct($args = array()) {
		parent::__construct($args);
		$this->loadChildren();
	}
	
	public function loadChildren() {
		if(!is_numeric($this->nid)) return null;

		$artwork_type = afo_portfolio_gettypes('artwork');
		$folder_type = afo_portfolio_gettypes('folder');

		$q = db_query('SELECT 
		ps.*, n.type, n.title, r.body
		FROM {afo_portfolio_structure} ps
		INNER JOIN {node} n ON ps.nid = n.nid
		INNER JOIN {node_revisions} r ON n.vid = r.vid
		WHERE ps.folder_nid = %d AND n.status = 1
		ORDER BY ps.weight ASC
		', $this->nid);

		while($result = db_fetch_array($q)) {
			if($result['type'] == $artwork_type) 		$this->children[] = new portfolioArtwork($result);
			elseif($result['type'] == $folder_type) 	$this->children[] = new portfolioFolder($result);
		}		
	}

	public function printItem($op = 'normal', $showThumbsTotal = 4) {
		if(!is_numeric($this->nid)) return null;

		// variables used in all types
		$dest = 'node/'.$this->nid;		
		global $base_path; 
		$images_folder = $base_path.drupal_get_path('module', 'afo_display').'/images';

		// output
		$output = '';
		
		// open
		$output .= "<div class='portfolio-folder'><div class='inner'>";
		// button and title
		$titleName = $this->data['title'] . ' <span class="artwork-count">(' . count($this->children) . ')</span>';
		$output .= '<div class="portfolio-title">'.l("<img src='$images_folder/Arrow-Right.png' alt='>' title='Show Options' />&nbsp;&nbsp;".$titleName, $dest, array('attributes'=>array('class'=>'show-folder'), 'html'=>true)).'</div>';
		// description
		$output .= '<div class="portfolio-description">'.$this->data['body'].'</div>';
		// visible children
		$output .= '<div class="portfolio-preview-thumbnails">';
		for($i=0; $i<$showThumbsTotal; $i++) {
			if(is_object($this->children[$i])) $output .= $this->children[$i]->printItem($op);
		}
		$output .= '</div>';
		// hidden children
		if($i<count($this->children)-1) {
			$output .= '<div class="portfolio-other-thumbnails">';
			for($i=$showThumbsTotal; $i<count($this->children); $i++) {
				if(is_object($this->children[$i])) $output .= $this->children[$i]->printItem($op);
			}
			// clear div
			$output .= '<div class="horiz-clear">&nbsp;</div>';
			$output .= '</div>';
		}
		// clear div
		$output .= '<div class="horiz-clear">&nbsp;</div>';
		// close
		$output .= '</div></div>';
		
		return $output;
	}

}


/** ********************************************** PORTFOLIO CLASS **/
class portfolioPortfolio
{
	public $itemType = 'portfolio';
	public $uid;
	public $children = array();
	public $loadType;

	public function __construct($args = array()) {
		if(is_array($args)) {
			// uid
			$this->uid = $args['uid'];

			// uid
			$this->loadType = isset($args['load type']) ? $args['load type'] : 'small';
			
			// children
			$this->loadChildren();
		}
	}

	public function loadChildren() {
		if(!is_numeric($this->uid)) return null;

		$artwork_type = afo_portfolio_gettypes('artwork');
		$folder_type = afo_portfolio_gettypes('folder');

		if($this->loadType == 'full') {
			$sql = "SELECT n.nid AS nid, n.type, n.title, n.created, r.body, ps.weight, ps.folder_nid
			FROM {node} n
			LEFT JOIN {node_revisions} r ON n.vid = r.vid
			LEFT JOIN {afo_portfolio_structure} ps ON n.nid = ps.nid
			WHERE n.status = 1 AND n.uid = %d AND (n.type = '%s' OR n.type = '%s') AND (ps.folder_nid = 0 OR ps.folder_nid IS NULL)
			ORDER BY ps.weight ASC, n.created DESC
			";
		} else {
			$sql = "SELECT n.nid AS nid, n.type, n.title, n.created, ps.weight, ps.folder_nid
			FROM {node} n
			LEFT JOIN {afo_portfolio_structure} ps ON n.nid = ps.nid
			WHERE n.status = 1 AND n.uid = %d AND (n.type = '%s' OR n.type = '%s') AND (ps.folder_nid = 0 OR ps.folder_nid IS NULL)
			ORDER BY ps.weight ASC, n.created DESC
			";
		}
		
		$q = db_query($sql, $this->uid, $artwork_type, $folder_type);

		while($result = db_fetch_array($q)) {
			if($result['type'] == $artwork_type) 		$this->children[] = new portfolioArtwork($result);
			elseif($result['type'] == $folder_type) 	$this->children[] = new portfolioFolder($result);
		}

		// reorder children
		$this->reorder();
		
	}
	
	public function printItem($op = 'normal', $args = array()) {	
		if(!count($this->children) > 0) return null;

		if($op == 'level' and is_numeric($args['nid'])) {
			// print only items in same folder or base level as argument nid
			$collapsed = array();
			$collapsed = $this->hierarchyCollapse($collapsed);
			foreach($collapsed as $k => $item) {
				if($item->nid == $args['nid']) {
					if($item->structure['folder_nid'] > 0) {
						// item in folder: print just that folder
						foreach($this->children as $child_item) {
							if($child_item->nid == $item->structure['folder_nid']) {
								if(!$args['show button label']) {
									$args['show button label'] = 'All artworks in ';
									$args['show button label'] .= isset($child_item->data['title']) ? $child_item->data['title'] : 'folder';
								}
								$content = $child_item->printItem();
								break;
							}
						}
					}
					else {
						// item not in folder: print top level portfolio
						if(!$args['show button label']) $args['show button label'] = 'All artworks in portfolio';
						$content = $this->printItem();
					}
					break;
				}
			}
			if($output) return $output;
			
		}
		
		else {
			// normal print
			foreach($this->children as $child) $content .= $child->printItem($op);
		}
		
		if($content) {
			// add css and js for portfolio
			drupal_add_css(drupal_get_path('module', 'afo_portfolio').'/afo_portfolio.css');	
			drupal_add_js(drupal_get_path('module', 'afo_portfolio').'/afo_portfolio.js');

			$portfolio_id = 'portfolio-'.$this->uid;
			
			if($args['collapsed']) {
				// print as hidden div with show/hide button
				$args['classes'][] = 'hide';
				$show_button_label = isset($args['show button label']) ? $args['show button label'] : t('All artworks in portfolio');
				$output .= l(t($show_button_label), 'user/'.$this->uid, array('attributes'=>array('class'=>'afo-portfolio-show', 'id'=>'show-'.$portfolio_id)));				
			}
			
			if(is_array($args['classes'])) $portfolio_classes = implode(' ', $args['classes']);
	
			$output .= "<div id='$portfolio_id' class='portfolio portfolio-$op $portfolio_classes'>";
			$output .= $content;
			$output .= '<div class="horiz-clear">&nbsp;</div>';
			$output .= '</div>';
			return $output;
		}
		
		return null;
	}
	
	public function getSlides() {
		$collapsed = $this->hierarchyCollapse($collapsed);
		if(is_array($collapsed)) {
			foreach($collapsed as $k => $item) {
				if($item->itemType == 'artwork') {
					$slides[] = $item->nid;
				}
			}
		}
		return $slides;
	}
	
	public function hierarchyCollapse(&$collapsed = array(), $current_depth = 0) {
		if(is_array($this->children)) {
			foreach($this->children as $child) {
				$child->structure['depth'] = $current_depth;
				$next_level = $child->hierarchyCollapse($collapsed, $current_depth + 1);
				array_merge($collapsed, $next_level);
			}
		}
		return $collapsed;		
	}
	
	public function reorder() {
		// set null weights to 0 & create array of weights
		foreach($this->children as &$child) {
			if($child->structure['weight'] === null) $child->structure['weight'] = 0;
			$weights[] = $child->structure['weight'];
			$createds[] = $child->data['created']; 
		}
		if(is_array($weights) && is_array($createds) && is_array($this->children)) {
			array_multisort($weights, $createds, $this->children);
		}
	}
	
	public function printThumbnail($options = array()) {
		$options['uid'] = $this->uid;
		$options['user thumbnail'] = true;
		$collapsed = array();
		$collapsed = $this->hierarchyCollapse($collapsed);
		foreach($collapsed as $k => $item) {
			if($item->itemType == 'artwork') {
				if($options['return'] == 'nid') return $item->nid;
				else return $item->printItem('portfolio thumbnail', $options);
			}
		}
		return null;
	}

	public function printPrevNext($nid) {
		$collapsed = array();
		$collapsed = $this->hierarchyCollapse($collapsed);
		foreach($collapsed as $k => $item) {
			if($item->nid == $nid) {
				// look backwards for previous artwork
				for($f = $k-1; $f >= 0; $f--) {
					if($collapsed[$f]->itemType == 'artwork') {
						$prevArtwork = $collapsed[$f];
						break;
					}
				}
				// look forwards for next artwork
				for($f = $k+1; $f < count($collapsed); $f++) {
					if($collapsed[$f]->itemType == 'artwork') {
						$nextArtwork = $collapsed[$f];
						break;
					}
				}
				
				if($prevArtwork) $links[] = l(t('Previous'), 'node/'.$prevArtwork->nid).' «';
				$links[] = l(t('Portfolio'), 'user/'.$this->uid);
				if($nextArtwork) $links[] = '» '.l(t('Next'), 'node/'.$nextArtwork->nid);
				return implode('&nbsp;', $links);
			}
		}
		return null;
	}
	
}
