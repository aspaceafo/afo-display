<?php
/**
 * Field handler for portfolio thumbnail.
 */
class afo_newsfeed_views_handler_event extends views_handler_field {
  /**
   * Override init function to provide generic option to link to user.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
	$this->additional_fields['uid'] = 'uid';
  }

  /**
   * Define default values for options.
   */
  function option_definition() {
    $options = parent::option_definition();
//    $options['imagecache_setting'] = array('default' => 'artwork_thumbnail', 'translatable' => FALSE);
    return $options;
  }

  /**
   * Extend the field's basic options with more image specific options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }
  
  function render_event($data, $values) {
  		$event_id = $values->{$this->field_alias};
  		if($event_id) {
			$newsfeed = array(afo_newsfeed_get_event($event_id));
			return theme_newsfeed($newsfeed, 'short');
		}
		return NULL;
  }

  function render($values) {
    return $this->render_event(check_plain($values->{$this->field_alias}), $values);
  }
}