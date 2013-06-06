<?php
// $Id$

/**
 * @file
 *
 * AFO CV display functions.
 */
 
/**
 * Define CV page callback.
 */
function afo_cv_view($cv_user, $op = 'view') {

	if(!is_object($cv_user)) {
		$cv_user = user_load(array('name'=>$cv_user));
	}
	
	if(afo_cv_access($cv_user->uid)) $editable = TRUE; 

	drupal_add_js(drupal_get_path('module', 'afo_cv').'/afo_cv.js');
	drupal_add_css(drupal_get_path('module', 'afo_cv').'/afo_cv.css');
	
	// normal view
	if(!$editable || $op == 'view') {
		// CV
		$user_cv = afo_cv_getcv($cv_user->uid);
		if(is_array($user_cv)) {
			$content['cv'] = array(
				'#value' => theme('afo_cv_entries', $user_cv, $editable),
			);
		}
	}
	
	// editable view
	elseif($editable && $op == 'edit') {

		jquery_ui_add(array('ui.draggable', 'ui.resizable', 'ui.dialog'));

		/*
		$content['edit-link'] = array(
			'#value' => afo_cv_editbutton(),
		);	
		*/

		// CV
		$user_cv = afo_cv_getcv($cv_user->uid);
		if(!($user_cv)) $cv_content = t('Your CV is empty. Use the form above to add entries.');
		else $cv_content = theme('afo_cv_entries', $user_cv, $editable);		
		$content['cv-current'] = array(
			'#type' => 'fieldset',
			'#title' => t('CV'),
			'#weight' => 5,
			'#collapsible' => true,
		);
		$content['cv-current']['cv'] = array(
			'#value' => $cv_content,
		);

		// add entry form
		$content['add-new'] = array(
			'#type' => 'fieldset',
			'#title' => t('Add new entry'),
			'#weight' => -1,
			'#collapsible' => true,
			'#collapsed' => true,
		);
		$content['add-new']['add-form'] = array(
			'#value' => drupal_get_form('afo_cv_add'),
			'#weight' => 0,
		);	
		
		// edit/add text
		$content['edit-cv-text'] = array(
			'#type' => 'fieldset',
			'#title' => t('Text only'),
			'#weight' => 3,
			'#collapsible' => true,
			'#collapsed' => true,
		);
		$content['edit-cv-text']['edit-text-form'] = array(
			'#value' => drupal_get_form('afo_cv_edit_text'),
			'#weight' => 0,
		);	
		
	}
	
	if(is_array($content)) return drupal_render($content);
	else return '';
}

/**
 * Theme cv entry list for HTML display.
 */
function theme_afo_cv_entries($cv, $editable=FALSE) {
	if(is_array($cv)) {
		
		$entry_types = afo_cv_entrytypes('full');
		
		foreach($cv as $typeid => $type_entries) {
				
			$type_name = t($entry_types[$typeid]['multiple']);
		
			if(is_array($type_entries)) {
				$output .= "<fieldset><legend>$type_name</legend><ul class='afo-cv-list'>";

				foreach($type_entries as $entry) {
					// create venue link if browse op
					$venue_link = theme('afo_cv_create_link', $entry['venues_name'], $entry_types[$typeid]['browse op']);
					
					// add link if url
					if($entry['url']) $entry_link = l(t('link'), $entry['url']);
				
					$output .= '<li>';
					$output .= "<span class='entry-year'>{$entry[year]}</span> ";
					$output .= "<span class='entry-details'>{$entry[details]}</span>, ";
					if($venue_link) $output .= "<span class='entry-venue'>$venue_link</span>";
					if($entry_link) $output .= " <span class='entry-link'>($entry_link)</span>";
					if($editable) $output .= ' <span class="entry-edit-button">['.afo_cv_editbutton($entry['entryid']).']</span>';
					$output .= '</li>';
				}
			
				$output .= '</ul></fieldset>';
			
			}
			
			elseif($typeid == 'text') {
				
				//dsm($type_entries);
				$output .= '<div class="afo-cv-text">';
				$output .= theme('afo_display_text', $type_entries, array('size'=>'full'));
				$output .= '</div>';
				
			}
		
		}
	}
	
	return $output;
}

/**
 * Return link to Browse/term page for CV item.
 */
function theme_afo_cv_create_link($term, $op=null) {
	/*
	if($op && module_exists('afo_browsenav2')) {
		$options = array(
			'type=artists',
			'sort=popular',
			'textsearch='.$term,
			'textsearchtype='.$op,
		); //browse/type=artists/op=inclusive/sort=popular/textsearch=gallery/textsearchtype=galleries
		$pathargs = 'browse/'.implode('/', $options);
		$alttext = t('click to see all artists with this term');
		return l($term, $pathargs, array('attributes'=>array('title'=>$alttext, 'alt'=>$alttext)));
	}
	*/
	
	return $term; // return unaltered term if no link
}