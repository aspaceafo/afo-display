// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){
		
		afo_backend_hierarchyInit();

	});
	
	function afo_backend_hierarchyInit() {
		//$('.afo-hierarchy select#level-0').after('<br /><select id="level-1"></select><br /><select id="level-2"></select>');
		
		$('.afo-hierarchy select').change(function() {
			var vid = afo_backend_hierarchy_getVid(this);
			var lev = afo_backend_hierarchy_getLevel(this);
			if(lev < 2) {
				afo_backend_hierarchy_rebuildSelect(vid, $(this).val(), lev+1);
			}
		});
	}

	function afo_backend_hierarchy_getVid(elem) {
		var vid = $(elem).parents('div.afo-hierarchy').attr('id').substr(18);
		return vid;
	}

	function afo_backend_hierarchy_getLevel(elem) {
		var lev = Number(elem.id.substr(6));
		return lev;
	}
	
	function afo_backend_hierarchy_getVals(vid) {
		vals = new Array();
		vals[0] = $('#afo-hierarchy-vid-'+vid+' #level-0').val();
		vals[1] = $('#afo-hierarchy-vid-'+vid+' #level-1').val();
		vals[2] = $('#afo-hierarchy-vid-'+vid+' #level-2').val();
		return vals;
	}
	
	function afo_backend_hierarchy_rebuildSelect(vid, val, lev) {
		var tree = Drupal.settings.afo_backend_hierarchy[vid];

		// find current level
		if(lev == 1) {
			var level = tree[val].children;
		} 
		else if(lev == 2) {
			var vals = afo_backend_hierarchy_getVals(vid);
			var level = tree[vals[0]].children[vals[1]].children;
		}
		
		// build select options
		var options = '<option value=""></option>';
		for(var key in level) {
			options += '<option value="'+key+'">'+level[key].name+'</option>';
		}

		var theSelect = $('#afo-hierarchy-vid-'+vid+' #level-'+lev);
		$(theSelect).html(options);
		afo_backend_hierarchy_clearLevels(theSelect);
	}
	
	function afo_backend_hierarchy_clearLevels(elem) {
		$(elem).nextAll('select').html('');	
	}
}