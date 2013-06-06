/*

 Newspaper Columns Plugin
 Oliver Petznick - Webmeisterei GmbH

 @ container:String
 @ column:String
 @ item:String
 @ col:Int

*/

jQuery.fn.npColumns = function(settings) {
    settings = jQuery.extend({
        container: '#npContainer',
        column: '#npColumn',
        item: '.npItem',
        cols: 3
    }, settings);
    
    var count = 1;
    var items = $(settings.container + ' ' + settings.item);
    var sizeOfItems = items.length;
    
    items.each(function(i,n) {
        
        $(settings.column + count).append($(this).clone());
        $(this).remove();
        
        if (count == settings.cols) {
            count = 1;
        } else {
            count += 1;
        }
        
    });
}
