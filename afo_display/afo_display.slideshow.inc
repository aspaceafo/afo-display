<?php
// $Id$

/**
 * @file
 *
 * Create slide show.
 */

/**
 * Assemble and return slide show
 */
function theme_afo_display_slideshow($nids, $title=null, $op='artwork', $options=array()) {
	if(!is_array($nids)) return null;

	// get slideshow data
	module_load_include('inc', 'afo_display', 'afo_display.data');
	$nodes = afo_display_data_get_slides($nids, $op);
	if(!is_array($nodes)) return null;
	
	// add includes for slide show
	if(count($nids) > 1) {

//		drupal_add_js(drupal_get_path('module', 'afo_display').'/slideshow.js');


		drupal_add_js(drupal_get_path('module', 'afo_display').'/galleria/galleria-1.2.8.min.js');
        

	   /* 
		$galleriascript = "		
		   Galleria.loadTheme('http://local-artists.org/dev/sites/all/modules/custom/afo_display/galleria/themes/azur/galleria.azur.min.js');
		    Galleria.run('#galleria', {
				showinfo: false,
				_showTooltip: false,
				_locale: {
					show_captions: ' ',
				},


				extend: function(options) {
		            var gallery = this; // this is the gallery instance
		            this.bind(Galleria.IMAGE, function(e) {
		                var current = gallery.getData(gallery.getIndex());
		                var currImg = current.original;
		                var imgclass = $(currImg).attr('class'); //imgclass retreives current image's orignal classes
						imgclass += ' ui-draggable';
		                console.log(imgclass);
						$(e.imageTarget).addClass(imgclass) //adds current image's original classes to current image 
		            });
		        },		
		 	});
		";
         
		drupal_add_js($galleriascript, $type = 'inline', $scope = 'footer'); //change the 2nd param to 'theme' if $myscript points to a theme .js file
        
	   
	
		drupal_add_js(drupal_get_path('module', 'afo_queue').'/afo_queue.js', $type='module', $scope = 'footer');//nh. this needs to go last
		
		//hidden
		*/
	    drupal_add_js('sites/all/modules/custom/afo_display/galleriaload.js', $type='module', $scope = 'footer'); //new idea
				
		jquery_ui_add(array('ui.core','ui.draggable', 'ui.droppable', 'ui.sortable'));
		
		drupal_add_js(drupal_get_path('module', 'afo_queue').'/afo_queue.js', $type='module', $scope = 'footer');
		
		
		
		drupal_add_js('sites/all/libraries/jquery-ui/ui/minified/ui.core.min.js', $type='module', $scope = 'footer');
		drupal_add_js('sites/all/libraries/jquery-ui/ui/minified/ui.draggable.min.js', $type='module', $scope = 'footer');
		drupal_add_js('sites/all/libraries/jquery-ui/ui/minified/ui.droppable.min.js', $type='module', $scope = 'footer');
  	    drupal_add_js('sites/all/libraries/jquery-ui/ui/minified/ui.sortable.min.js', $type='module', $scope = 'footer');
		/*
		//end test
		

	//drupal_add_js($galleriascript, array('type' => 'inline', 'scope' => 'footer', 'weight' => 0);

/*	 drupal_add_js(
		'jQuery(document).ready(function () { alert("Hello!"); });',
	    array('type' => 'inline', 'scope' => 'footer', 'weight' => 5)
	);
*/

	}
	drupal_add_css(drupal_get_path('module', 'afo_display').'/slideshow.css');
	jquery_ui_add(array('effects.core', 'effects.slide'));
	
	// build slide show
	$content = afo_display_build_slideshow($nodes, $title, $options);

	return $content;
}
 
/**
 * Build slide show.
 */

function afo_display_build_slideshow($nodes, $title=null, $options=array()) {
	if(!is_array($nodes)) return null;

	$images_folder = drupal_get_path('module', 'afo_display').'/images/';

	// if no imagecache_preset, set default
	if(!isset($options['imagecache_preset'])) $options['imagecache_preset'] = 'artwork_full';

	if(isset($options['id'])) $addId = 'id="'.$options['id'].'"';
	if(isset($options['imagecache_preset'])) $options['classes'][] = 'afo-slideshow-'.$options['imagecache_preset']; 
	if(is_array($options['classes'])) $addClasses = implode(' ', $options['classes']);
	$content .= '<div id="galleria" class="afo-slideshow '.$addClasses.'" style="display:none;">';
//	$content .= '<div '.$addId.' class="afo-slideshow '.$addClasses.'"><div class="afo-slideshow-inner">';

	
	if($title) {
		$content .= '<div class="afo-slideshow-title">'.$title.'</div>';
	}

/*	$content .= '<div class="afo-slide-controls">';
		$content .= '<div class="afo-slide-button afo-slide-button-previous"><a href="#">';
		$content .= theme('image', $images_folder.'icon_bigarrow_left.png', t('previous'), t('previous slide'));
		$content .= '</a></div>';
		$content .= '<div class="afo-slide-button afo-slide-button-play"><a href="#">';
		$content .= theme('image', $images_folder.'icon_bigarrow_pause.png', t('pause'), t('pause slide show'), array('class'=>'pause'));
		$content .= theme('image', $images_folder.'icon_bigarrow_play.png', t('play'), t('play slide show'), array('class'=>'play hide'));
		$content .= '</a></div>';
		$content .= '<div class="afo-slide-button afo-slide-button-next"><a href="#">';
		$content .= theme('image', $images_folder.'icon_bigarrow_right.png', t('next'), t('next slide'));
		$content .= '</a></div>';
	$content .= '</div>';
*/
	
	$nodes[0]['classes'][] = 'current';

	foreach($nodes as $n) {
		$content .= afo_display_build_slide($n, $options);
	}

/*	$content .=afo-portfolio-slides '</div>';
	$content .= '<div class="horiz-clear">&nbsp;</div>';
*/	$content .= '</div>';

	return $content;
}

/**
 * Build slide
 */
function afo_display_build_slide($n, $options=array()) {
	if(!is_array($n)) return null;

	$images_folder = drupal_get_path('module', 'afo_display').'/images/';
	
	// open slide
	if(is_array($n['classes'])) $classes = implode(' ', $n['classes']);
//	$slide .= "<div class='afo-slide $classes'><div class='afo-slide-inner'>";

		if($n['image']) {
			// image
			//$slide .= '<div class="afo-slide-image">';

				// uncomment to set image link to node id 
				//if(!$n['path'] && is_numeric($n['nid'])) $n['path'] = 'node/'.$n['nid'];
				
				// queue class
				if(module_exists('afo_queue')) $queue_class = afo_queue_queueable_classes($n['nid'], 'node');
				
		// info
		if(module_exists('afo_portfolio')) {
			// artist name
			if(is_numeric($n['uid'])) $n['artist'] = l($n['name'], 'user/'.$n['uid']);
			else $n['artist'] = $n['name'];
			
			// title
			if(is_numeric($n['nid'])) $n['title'] = l($n['title'], 'node/'.$n['nid']);
			
			$alt .= theme('afo_portfolio_artwork_listing', $n); /* runs thru lisitng function */
		}
		//end info

				
						
				$image = theme('imagecache', $options['imagecache_preset'], $n['image'], $alt, '', array('class'=>$queue_class));
				

				      // theme('imagecache', $preset, $image['filepath'], $alt, $title,  $attributes);
				
				
				if($n['path']) $slide .= l($image, $n['path'], array('html'=>'true'));
				else $slide .= $image."\n";

//			$slide .= '</div>';
		}
		
		
	// close slide
//	$slide .= '</div></div>';

	return $slide;
}