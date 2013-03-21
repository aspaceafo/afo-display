// $Id$

if(Drupal.jsEnabled) {

	/************************************ Object types **/
	function browse_param(type) {
		this.type = type;
		this.required = true;
		this.multiple = false;
		this.values = new Array();
		
		this.addValues = function(vals) {
			if(isString(vals)) {
				vals = vals.split(',');
			}
			for(var k in vals) {
				this.updateValues(vals[k]);
			}
		}
		
		this.updateValues = function(value) {

			var pass = true;
			
			for (var k in this.values) {
				if (this.values[k] == value) {
					pass = false;
					
					if(this.required === false || this.multiple == true) {
						this.values.splice(k, 1);
					}
				}
			}
			
			if(pass == true) {
				if (this.multiple == true) {
					if(pass == true) this.values.push(value);		
				}
				else {
					this.values = new Array(value);
				}
			}
			
			if((this.values.length === 0 || this.values[0] === undefined) && this.required == true) {
				this.updateValues(this.defaultvalue);
			}
			
		}
		
		this.getValues = function() {
			this.cleanValues();
			if(this.multiple == true) return this.values;
			else return this.values[0];
		}
		
		this.getarg = function() {
			if(this.values.length > 0) return this.type + '=' + this.values.join(',');
			else return null;
		}
		
		this.cleanValues = function() {
			for(var k in this.values) {
				if(this.values[k] === null || this.values[k] === '') {
					this.values.splice(k, 1);
				}
			}
		}
		
		this.reset = function() {
			this.values = new Array();
			if(this.defaultvalue) this.updateValues(this.defaultvalue);
		}
	}

	/************************************ Init functions **/
	$(document).ready(function(){

		afo_browsenav2_init_search();
		afo_browsenav2_init_search_buttons();
		afo_browsenav2_update_search_terms();
		afo_browsenav2_init_page_buttons();
		afo_browsenav2_init_links();
		afo_browsenav2_init_textsearch();		

	});
	
	$(window).load(function() {
		// collapse window on load
		//setTimeout("afo_browsenav2_widget_toggle('collapse')", 400);
	});

	function afo_browsenav2_init_search() {
		$('.browsenav-widget div.current-search').hide();
	
		var args = Drupal.settings.afo_browsenav2.args;
//		var params = Drupal.settings.afo_browsenav2.params;
		var params = afo_browsenav2_href_to_params(args);
		
		Drupal.settings.afo_browsenav2.search = new Object();
		
		Drupal.settings.afo_browsenav2.search.type = new browse_param('type');
		Drupal.settings.afo_browsenav2.search.type.defaultvalue = 'artwork';
		Drupal.settings.afo_browsenav2.search.type.updateValues(params.type);

		Drupal.settings.afo_browsenav2.search.op = new browse_param('op');
		Drupal.settings.afo_browsenav2.search.op.defaultvalue = 'inclusive';
		Drupal.settings.afo_browsenav2.search.op.updateValues(params.op);

		Drupal.settings.afo_browsenav2.search.sort = new browse_param('sort');
		Drupal.settings.afo_browsenav2.search.sort.defaultvalue = 'popular';
		Drupal.settings.afo_browsenav2.search.sort.updateValues(params.sort);
		
		Drupal.settings.afo_browsenav2.search.tags = new browse_param('tags');
		Drupal.settings.afo_browsenav2.search.tags.multiple = true;
		Drupal.settings.afo_browsenav2.search.tags.required = false;
		if(params.tags) { Drupal.settings.afo_browsenav2.search.tags.addValues(params.tags); }

		Drupal.settings.afo_browsenav2.search.regions = new browse_param('regions');
		Drupal.settings.afo_browsenav2.search.regions.multiple = true;
		Drupal.settings.afo_browsenav2.search.regions.required = false;
		if(params.regions) { Drupal.settings.afo_browsenav2.search.regions.addValues(params.regions); }

		Drupal.settings.afo_browsenav2.search.textsearch = new browse_param('textsearch');
		Drupal.settings.afo_browsenav2.search.textsearch.required = false;
		if(params.textsearch) { Drupal.settings.afo_browsenav2.search.textsearch.updateValues(params.textsearch); }

		Drupal.settings.afo_browsenav2.search.textsearchtype = new browse_param('textsearchtype');
		Drupal.settings.afo_browsenav2.search.textsearchtype.required = false;
		if(params.textsearchtype) { Drupal.settings.afo_browsenav2.search.textsearchtype.updateValues(params.textsearchtype); }

		afo_browsenav2_rebuild_current();
		//setTimeout("afo_browsenav2_widget_toggle('collapse')", 400);

	}
	
	function afo_browsenav2_init_search_buttons() {

		// execute search button
		$('.browsenav-widget #go-search').click(function () {
			var queryURL = afo_browsenav2_compile_search_url();
			//if(queryURL !== null) window.location = 'http://local-artists.org/browse/'+queryURL;
			if(queryURL !== null) window.location = Drupal.settings.afo_browsenav2.browsepage+queryURL;
			return false;
		});

		// clear search button
		$('.browsenav-widget #clear-search').click(function () {
			afo_browsenav2_reset_search();
			return false;
		});
		
		// hide button
		$('.browsenav-widget #hide-widget').click(function () {
			afo_browsenav2_widget_toggle('collapse');
			return false;
		});
	}
	
	function afo_browsenav2_init_page_buttons() {
		$("#browsenav-page-options").show();
		$("#browsenav-page-options .change-page a").click(function () {
			// get page value
			var showPage = afo_browsenav2_class_to_change(this.id);
			var showPage = showPage.value;

			// switch active link
			$("#browsenav-page-options .change-page a.active-page").removeClass('active-page');
			$(this).addClass('active-page');

			// switch active page
			$(".widget-page.active-page").toggle(500, function() {
			//$(".widget-page.active-page").toggle("slide", { direction: "left" }, 500, function() {
				$(".widget-page.active-page").removeClass('active-page');			
				
				$("#browsenav-page-"+showPage).toggle(500);
			    //$("#browsenav-page-"+showPage).toggle("slide", { direction: "right" }, 500);
				
				$("#browsenav-page-"+showPage).addClass('active-page');			              
				
			});
			
			return false;
		});		
	}	
		
	function afo_browsenav2_init_links() {

		$('.browsenav-widget .widget-page a').click( function() {
			afo_browsenav2_link_process(this);
			return false;
		});
		
		$('.browsenav-widget .columnize ul').treeview({
			persist: "location",
			collapsed: true,
			unique: true
		});

		afo_browsenav2_rebuild_links();
	}

	function afo_browsenav2_init_textsearch() {
		
		$('.browsenav-widget input.browsenav-text-search-option').change(function() {
			Drupal.settings.afo_browsenav2.search.textsearchtype.updateValues($(this).val());			
		});
		
		$('.browsenav-widget #browsenav-text-search-add').click(function() {
			afo_browsenav2_textsearch_process();
			var queryURL = afo_browsenav2_compile_search_url();
			queryURL += '/start=text';
			if(queryURL !== null) window.location = Drupal.settings.afo_browsenav2.browsepage+queryURL;
			return false;
		});
		
	}


	/************************************ Widget functions **/
	/**
	 * link clicked
	 */
	function afo_browsenav2_link_process(link) {
		var cls = $(link).attr('class');
		if(cls.length > 0) {
			var change = afo_browsenav2_class_to_change(cls);
			if(change !== null) {
				afo_browsenav2_alter_search(change);
				afo_browsenav2_rebuild_links();
			}
		}
		afo_browsenav2_update_search_terms();
	}
	
	/**
	 * search clicked
	 */
	function afo_browsenav2_textsearch_process() {
		var change = new Object;
		change.type = "textsearch";
		change.value = $("#browsenav-text-search").val();
		afo_browsenav2_alter_search(change);
		//afo_browsenav2_rebuild_links();
		afo_browsenav2_update_search_terms();
	}
	
	/**
	 * alter search params
	 */
	function afo_browsenav2_alter_search(change) {
		if(Drupal.settings.afo_browsenav2.search[change.type]) {
			Drupal.settings.afo_browsenav2.search[change.type].updateValues(change.value);
			afo_browsenav2_results_preview();
		}
	}
	
	/**
	 * Results preview
	 */
	function afo_browsenav2_results_preview() {
		/*
		var previewUrl = Drupal.settings.basePath + 'browse/preview/' + afo_browsenav2_compile_search_url();
		$(".browsenav-widget #results-preview .results-total").text('...');
		$.ajax({
			type: 'POST',
			url: previewUrl,
			timeout: 4000,
			dataType: 'json',
			success: function(data) {
				$(".browsenav-widget #results-preview .results-total").text(data.total);
				$(".browsenav-widget #results-preview .results-label").show();
			},
			error: function(XMLHttpRequest, textStatus, errorThrown) {
				$(".browsenav-widget #results-preview .results-total").text('');
				$(".browsenav-widget #results-preview .results-label").hide();
			}
		});
		*/
	}
	
	/**
	 * reset search
	 */
	function afo_browsenav2_reset_search() {
		for(var param in Drupal.settings.afo_browsenav2.search) {
			Drupal.settings.afo_browsenav2.search[param].reset();
		}
		afo_browsenav2_rebuild_links();
		afo_browsenav2_update_search_terms();
	}
	
	/**
	 * Update list of search terms
	 */
	function afo_browsenav2_update_search_terms() {
		var termList = new Array();

		// tag labels
		var termIds = Drupal.settings.afo_browsenav2.search.tags.getValues();		
		for(var k in termIds) {			
			var termLabel = $(".widget-page ."+afo_browsenav2_param_to_class('tags', termIds[k])).text();
			var termLink = "<a href='#' class='tagsZ"+termIds[k]+"'>"+termLabel+"</a>";
			termList.push(termLink);
		}

		// region labels
		var termIds = Drupal.settings.afo_browsenav2.search.regions.getValues();		
		for(var k in termIds) {
			var termLabel = $(".widget-page ."+afo_browsenav2_param_to_class('regions', termIds[k])).text();
			var termLink = "<a href='#' class='regionsZ"+termIds[k]+"'>"+termLabel+"</a>";
			termList.push(termLink);
		}

		// text search
		var textSearchValue = Drupal.settings.afo_browsenav2.search.textsearch.getValues();
		if(textSearchValue) {
			termList.push('"<a href="#" class="textsearch">'+textSearchValue+'</a>"');
		}		

		if(Drupal.settings.afo_browsenav2.search.op.getValues() == 'exclusive') joiner = ' + ';
		else var joiner = ', ';
		$('#selected-search-terms').html(termList.join(joiner));
		$('#selected-search-terms a').click(function() {
			afo_browsenav2_link_process(this);
			return false;
		});
			
	}
	
	/**
	 * convert link href into parameter changes
	 */
	function afo_browsenav2_href_to_params(url) {
	
		// select and clean href
		var b = url.indexOf('browse2/');
		if(b > 0) var url = url.substr(b+8);
		url = url.replace(/%3D/gi, '=');
		url = url.replace(/%2C/gi, ',');

		var params = new Object;
		params.url = url;
		
		// convert to array
		url = url.split("/");
		for(var i=0; i<url.length; i++) {
			var v = url[i].split("=");
			params[v[0]] = v[1];
			//params[v[0]] = v[1].split(",");
			
		}
		
		return params;
	}
	
	/**
	 * convert link class to params change
	 */
	function afo_browsenav2_class_to_change(cls) {
		if(cls.length > 0) {
			cls = cls.split(" ");
			for(var key in cls) {
				if(cls[key].indexOf("Z")) {
					Zclass = cls[key].split("Z");
					for(var i=0; i < Zclass.length; i++) {
						Zclass[i] = Zclass[i].replace(/_/gi, ' ');
					}
					
					var change = new Object;
					change.type = Zclass[0];
					change.value = Zclass[1];
		
					return change;
				}
			}
		}
		return null;
	}
	
	/**
	 * convert params to link class
	 */
	function afo_browsenav2_param_to_class(type, value) {
		if(value) {
			var t = type.replace(/ /g, "_");
			var v = value.replace(/ /g, "_");
			return t + 'Z' + v;
		}
		return null;
	}

	/**
	 * compile search into query URL
	 */
	function afo_browsenav2_compile_search_url() {
		var searchTerms = new Array('type', 'op', 'sort', 'tags', 'regions', 'textsearch', 'textsearchtype');
		var url = new Array();
		
		for(var s in searchTerms) {
			var args = Drupal.settings.afo_browsenav2.search[searchTerms[s]].getarg();
			if(args !== null) {
				url.push(args);
			}
		}
		
		if(url.length > 0) return url.join('/');
		else return null;
	}
	
	/**
	 * return dispay value (e.g. term name) given raw value
	 */
	function afo_browsenav2_get_link_display(val) {
		return $('.browsenav-widget .link-for_'+val+' a').html();
	}

	/************************************ Display functions **/
	
	/**
	 * Modify display properties of links to reflect current search params.
	 */
	function afo_browsenav2_rebuild_links() {
		$('.browsenav-widget a.active').removeClass('active');
		for(var p in Drupal.settings.afo_browsenav2.search) {		
			for(var v in Drupal.settings.afo_browsenav2.search[p].values) {
				var linkClass = afo_browsenav2_param_to_class(Drupal.settings.afo_browsenav2.search[p].type, Drupal.settings.afo_browsenav2.search[p].values[v]);
				$('.browsenav-widget a.'+linkClass).addClass('active');
			}
		}
		afo_browsenav2_rebuild_textsearch();
	}
	
	/**
	 * Modify text search to reflect current search params;
	 */
	function afo_browsenav2_rebuild_textsearch() {
		var textSearchValue = Drupal.settings.afo_browsenav2.search.textsearch.getValues();
		if(!textSearchValue) textSearchValue = '';
		$('.browsenav-widget #browsenav-text-search').val(textSearchValue);
	}
	
	/**
	 * Collapse or expand widget.
	 */
	function afo_browsenav2_widget_toggle(op) {
		if(op=='collapse') {
			$('.browsenav-widget div.search-options').slideUp('slow', function() {
				$('.browsenav-widget div.current-search').slideDown('fast');
			});
		}
		else if(op=='expand') {
			$('.browsenav-widget div.current-search').slideUp('fast', function() {
				$('.browsenav-widget div.search-options').slideDown('slow');
			});
		}
	}
	
	/**
	 * Rebuild current selected div (collapsed widget).
	 */
	function afo_browsenav2_rebuild_current() {
		var cur = $('.browsenav-widget div.current-search');

		var type = Drupal.settings.afo_browsenav2.search.type.getValues();
		var op = Drupal.settings.afo_browsenav2.search.op.getValues();
		var tags = new Array();
		tags = tags.concat(Drupal.settings.afo_browsenav2.search.tags.getValues());
		tags = tags.concat(Drupal.settings.afo_browsenav2.search.regions.getValues());
		
		var tags_list = new Array();
		
		if(tags.length > 0) {
			for(var t in tags) {
				tags_list.push('<span class="current-option">'+afo_browsenav2_get_link_display(tags[t])+'</span>');
			}
		}

		// text search
		var textSearchValue = Drupal.settings.afo_browsenav2.search.textsearch.getValues();
		if(textSearchValue) {
			tags_list.push('"<span class="current-option">'+textSearchValue+'</span>"');
		}

		var search_list = 'Searching <span class="current-option">' + type + '</span>';

		if(tags_list.length > 0) {
			if(op=='exclusive') var sep = ' + ';
			else var sep = ', ';
			tags_list = tags_list.join(sep);
			search_list += ' for tags: ' + tags_list;
		}
		
		search_list += "<div id='current-hover-top' style='display:none; float:right;'>click to edit search</div>"; 

		cur.find('#current-search-options').html(search_list);

		// click function
		cur.click(function () {
			afo_browsenav2_widget_toggle('expand');
			return false;
		});

		// hover tip
		cur.hover(function () {
			$("#current-hover-top").show();
		}, function () {
			$("#current-hover-top").hide();		
		});
	}

	
	/************************************ Page search functions **/



	/************************************ helper functions **/

	// Return a boolean value telling whether // the first argument is a string. 	
	function isString() {
		if (typeof arguments[0] == 'string') return true;
		if (typeof arguments[0] == 'object') {  
			var criterion = arguments[0].constructor.toString().match(/string/i); 
	 		return (criterion != null);  
	 	}
	 	return false;
	 }

}