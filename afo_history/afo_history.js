// $Id$

/**
 * AFO History JS functions.
 */
if(Drupal.jsEnabled) {

	$(document).ready(function() {
	
		$('a.afo-history-remove').click(function() {

			$.ajax({
			  type: 'POST',
			  url: this.href,
			  dataType: 'json',
			  data: 'js=1',
			  success: function(key) {
				afo_history_erase(key);
			  }
			});
			
			return false;

		});
		
		$(".block-afo_history .viewing-history .item-display").fadeTo('fast', .2);
		$(".block-afo_history .viewing-history .thumbnail img").mouseover(function() {
			//$(this).parents('.item').find('.item-type-label').show();
			$(this).parents('.item-display').fadeTo('fast', 1);
		}).mouseout(function() {
			//$(this).parents('.item').find('.item-type-label').hide();
			$(this).parents('.item-display').fadeTo('fast', .2);		
		});
		
	});
	
	function afo_history_erase(key) {
	
		if(key=='all') var item = $(".viewing-history .item");
		else var item = $(".viewing-history #item-"+key);
		
		if(item.length > 0) {
			item.slideToggle('slow', function() {
    			// Animation complete.
    			item.remove();
  			});
		}
	
	}
	
}