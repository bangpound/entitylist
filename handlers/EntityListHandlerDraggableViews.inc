<?php

class EntityListHandlerDraggableViews extends EntityListHandlerViews {

  /**
   * @todo Set tags with arguments for better granularity.
   */
  public function getCacheTags() {
    $tags = array();
    if ($view = $this->getView()) {
      list($name, $display) = explode(':', $this->config['view']);
      $tags = array('views' => array($name, "$name:$display"));

      // Add tags for this view's order view as well.
      if ($order_view = draggableviews_get_draggable_view($view)) {
        list($order_name, $order_display) = explode(':', $order_view);
        $tags['views'][] = $order_name;
        $tags['views'][] = "$order_name:$order_display";
      }
    }
    return $tags;
  }

  public function adminPath() {
    if ($view = $this->getView()) {
      list($name, $display) = explode(':', $this->config['view']);
      $view->build($display);
      if ($path = draggableviews_get_draggable_path($view, TRUE)) {
        return $path;
      }
    }
    return FALSE;
  }
}
