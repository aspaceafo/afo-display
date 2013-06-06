<?php
// $Id$

/**
 * @file
 *
 * AFO Membership benefits table.
 */

function _afo_member_comparison_table() {

	$t['artists']['#title'] = t('All Artists');
	$t['artists']['#description'] = t('The basic Artists account is an easy-to-navigate update on the former Irving Sandler Artists File Artists Account and remains free for anyone to join.');
	$t['artists']['#list-header'] = t('Features Include:');
	
	$t['artists'][] = array(
		'label' => t('Upload and tag up to 12 artworks in your portfolio.'),
		'desc' => t("The 'Tagging Feature' links your artwork to a search engine based on region, medium and categories such as 'appropriation' or 'documentary,' allowing a browser to find your work easily through a specified search."),
	);
	$t['artists'][] = array(
		'label' => t("Present your work in the comprehensive, organized 'Grid Format'."),
	);
	$t['artists'][] = array(
		'label' => t('Upload a Personal Statement and CV.'),
	);
	$t['artists'][] = array(
		'label' => t("'Share' feature allows browsers to link your Portfolio to Facebook, Google, and other social networks."),
	);
	$t['artists'][] = array(
		'label' => t('Submit your work directly to Open Calls.'),
		'desc' => t("The 'Open Calls' section is a bulletin where Registered Curators post Open Calls for international exhibitions, competitions, residencies, and other opportunities for Artists and Artist Members. You can submit your application directly to the Open Calls post, rather than applying through separate websites."),
	);
	$t['artists'][] = array(
		'label' => t("Link your Portfolio Page to your own website"),
	);
	$t['artists'][] = array(
		'label' => t("Allow browsers to safely contact you."),
	);
	$t['artists'][] = array(
		'label' => t("New Browsing Tools include: create your own 'Queue' and save your 'Favorites.'"),
		'desc' => t("The viewing 'Queue', which you can see at the bottom of the browser, allows you to save relevant artworks and artists in the same place during the course of your search, eliminating the need to bookmark individual pages. <br /><br /> The 'Favorites' tool allows you to save favorite artworks and artists to your profile permanently, allowing you to track them as they update their profiles."),
	);
	
	
	$t['members']['#title'] = t('Artist Membership');
	//$t['members']['#description'] = t("The 'New Artist Membership Account' contains all the basic features in the 'Artist Account', plus many more exclusive features.  As a Member, you can upload unlimited images and embed unlimited videos to their Portfolios, announce your events on the 'Public Community Calendar', embed your 'Twitter Feed' onto your profile, have the opportunity to be selected for our monthly 'Curated Section', and apply for exclusive Artist Membership Production, Research, and Travel grants. If you join by August 31, 2010 you can take advantage of the 'New Artist Membership Account' introductory rate for $10 only, - valid for 6 months - after this date the New Annual Membership Account will be $38, valid for one year (365 days).");
	$t['members']['#description'] = t("The 'Artist Membership Account' contains all the basic features in the 'Artist Account', plus many more exclusive features.");
	$t['members']['#list-header'] = t('Exclusive Artist Membership Account features include:');
	
	$t['members'][] = array(
		'label' => t("Upload unlimited artworks to your 'Portfolio'."),
	);
	$t['members'][] = array(
		'label' => t("Embed unlimited videos into your 'Portfolio'."),
	);
	$t['members'][] = array(
		'label' => t("Organize and divide your portfolio into 'Folders'."),
	);
	$t['members'][] = array(
		'label' => t("'3 Portfolio Display Options' (Slideshow, Grid, and Full Screen)"),
		'desc' => t("The 'Full Screen Option' allows browsers to view your images at their highest resolution."),
	);
	$t['members'][] = array(
		'label' => t("Embed your 'Twitter Feed' to your profile."),
	);
	$t['members'][] = array(
		'label' => t("Track your 'Page Traffic' with google analytics."),
	);
	$t['members'][] = array(
		'label' => t("Publicize your events."),
		'desc' => t("Display your events on your portfolio and on the 'Community Events Calendar' to an audience of over 39 million hits annually."),
	);
	$t['members'][] = array(
		'label' => t("A chance to be featured in the monthly 'Curated Section.'"),
		'desc' => t("Every month, local-artists.org invites an international curator to select his/her Top 20 among the members of local-artists.org.  Their 'Top 20' list will be featured together with the member profiles on the home page slideshow, and will be permanently archived."),
	);
	$t['members'][] = array(
		'label' => t("The opportunity to apply for exclusive Artist Membership Production, Research, and Travel grants (details coming soon)."),
	);

	$current_weight = 0;
	
	foreach($t as $tname => $td) {
	
		$content[$tname] = array(
			'#weight' => $current_weight,
		);
		$current_weight ++;

		$content[$tname]['title'] = array(
			'#prefix' => '<h3>',
			'#value' => $td['#title'],
			'#suffix' => '</h3>',
			'#weight' => -4,
		);

		$content[$tname]['description'] = array(
			'#prefix' => '<div style="margin-bottom: 10px;">',
			'#value' => $td['#description'],
			'#suffix' => '</div>',
			'#weight' => -3,
		);

		$content[$tname]['header'] = array(
			'#prefix' => '<h5>',
			'#value' => $td['#list-header'],
			'#suffix' => '</h5>',
			'#weight' => -2,
		);

		$features_list = array();		
		foreach(element_children($td) as $c) {
			$elem = $td[$c];
			
			if(isset($elem['desc'])) 
			{
				/*
				$content[$tname]['list'][$c] = array(
					'#prefix' => '<div class="member-benefits-list-item">',
					'#value' => l($elem['label'], '#', array('query'=>"target=#benefit-desc-$tname-$c&noscroll=1", 'attributes'=>array('class'=>'opener'))) . "<div id='benefit-desc-$tname-$c' class='hide secondaryText'>" . $elem['desc'] . '</div>',
					'#suffix' => '</div>',				
				);
				*/
				
				$features_list[] = l($elem['label'], '#', array('query'=>"target=#benefit-desc-$tname-$c&noscroll=1", 'attributes'=>array('class'=>'opener'))) . "<div id='benefit-desc-$tname-$c' class='hide secondaryText'>" . $elem['desc'] . '</div>';
				
			} 
			else 
			{
				/*
				$content[$tname]['list'][$c] = array(
					'#prefix' => '<div class="member-benefits-list-item">',
					'#value' => $elem['label'],
					'#suffix' => '</div>',				
				);			
				*/
				
				$features_list[] = $elem['label'];
			}
		}
		$content[$tname]['list'][$c] = array(
			'#prefix' => '<div class="member-benefits-list">',
			'#value' => theme('item_list', $features_list, null, 'ul', array('class'=>'arrow')),
			'#suffix' => '</div>',				
		);			
		
	}

	return drupal_render($content);
	
}