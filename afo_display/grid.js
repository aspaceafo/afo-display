// $Id$

if(Drupal.jsEnabled) {

	$(document).ready(function(){
		
		afo_display_grid_init();
		
	});

	function afo_display_grid_init() {
	
		// hide non-image fields and add display functions
		$('.afo-grid:not(.no-roll) .grid-item .rollToggle:not(.no-roll)').hide();
	
		// non expanding grid: show other items on hover
		$('.afo-grid:not(.no-roll) .grid-item .item-thumbnail').hover(
			function () {
				$(this).siblings('.rollToggle').show();
			}, 
			function () {
				$(this).siblings('.rollToggle').hide();
			}
		);	
		
		// expanding grid: show items on hover and expand
		$('.afo-grid.expandable .grid-item .item-thumbnail img').mouseover(
			function () {
				
				// make sure no other items are expanded
				$(this).parents(".afo-grid").find(".grid-item.expanded").each(function() {
					afo_display_grid_contract(this);
				});
				
				var expanded = $(this).parents(".grid-item").find(".expanded-view");
				$(expanded).show();
				$(this).parents(".normal-view").hide();
				$(this).parents(".grid-item").addClass("expanded");
				$(expanded).find(".hit-area").mouseout(function () {
					$(this).parents(".grid-item").each(function() {
						afo_display_grid_contract(this);
					});
				});
			}
		);
	
	
		/** init grid item buttons **/
		$('.afo-grid .imageButtons img').click(function() {
			var buttonOp = $(this).attr('class');
			var gridItem = $(this).parents('.grid-item');
			var itemId = $(gridItem).attr('id').substr(5);
			
			// determine op
			if ($(gridItem).hasClass('op-artists')) var op = 'artists';
			else if ($(gridItem).hasClass('op-artwork')) var op = 'artwork';
			
			switch(buttonOp) {
			
				case 'favorite-remove':
					$.ajax({
					  type: 'POST',
					  url: Drupal.settings.basePath+'favorites/remove/'+op+'/'+itemId,
					  dataType: 'json',
					  data: 'js=1',
					  success: function() {
						// remove grid item
						afo_display_grid_removeItem(gridItem);
					  }
					});	
					break;
			
			}
			
			return false;
		});

	}
	
	function afo_display_grid_contract(item) {
		//if(!$(this).hasClass('grid-item'))
		$(item).find(".expanded-view").hide();
		$(item).removeClass("expanded");
		$(item).find(".normal-view").show();
	}	

	function afo_display_grid_removeItem(item) {
		$(item).slideUp('fast');
	}

}