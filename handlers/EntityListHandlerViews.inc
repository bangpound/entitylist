<?php

class EntityListHandlerViews implements EntityListHandler {

  protected $config = array();

  protected $entityType = NULL;

  protected $argumentInput = array();

  protected $arguments = array();

  public function __construct(Array $config = array()) {
    $defaults = array(
      'view' => '',
    );
    $this->config = array_merge($defaults, $config);
  }

  /**
   * Helper method to return the view this handler is dealing with. We can't
   * save it in the object, because of wierdness (?) when a list context is
   * passed around.
   */
  protected function getView() {
    if (!empty($this->config['view'])) {
      list($name, $display) = explode(':', $this->config['view']);
      $view = views_get_view($name);
      if (!empty($view)) {
        $view->set_display($display);
        return $view;
      }
    }
    return FALSE;
  }

  protected function getViewOptions() {
    $infos = views_get_applicable_views('returns context');
    $options = array();
    foreach ($infos as $info) {
      $view = $info[0];
      $display_id = $info[1];
      $options[$view->name . ':' . $display_id] = $view->human_name . ' (' . $view->display_handler->display->display_title . ')';
    }
    return $options;
  }

  public function entityType() {
    if ($this->entityType === NULL && $view = $this->getView()) {
      $table = views_fetch_data($view->base_table);
      if (!isset($table['table']['entity type'])) {
        throw new EntityListException(t('Could not determine entity type of view @view', array('@view' => $view->name)));
      }
      $this->entityType = $table['table']['entity type'];
    }
    return $this->entityType;
  }

  public function argumentInput() {
    if (empty($this->argumentInput) && $view = $this->getView()) {
      // This code is pretty much taken directly from views_content.module.
      // TODO: Simplify when we've got our own stripped down Views display type.
      $arguments = $view->display_handler->get_argument_input();
      ctools_include('views');
      $contexts = array();
      foreach ($arguments as $argument) {
        if ($argument['type'] == 'context') {
          list($context, $converter) = explode('.', $argument['context'], 2);
          if ($context == 'term' || $context == 'vocabulary') {
            $context = 'entity:taxonomy_' . $context;
          }
          elseif ($entity = entity_get_info($context)) {
            $context = 'entity:' . $context;
          }
          $this->argumentInput[$context . '.' . $converter] = array(
            'label' => $argument['name'] ? $argument['name'] : '',
            'required' => empty($argument['context_optional']) ? TRUE : FALSE,
          );
        }
      }
    }
    return $this->argumentInput;
  }

  public function setArguments(Array $arguments = array()) {
    $this->arguments = $arguments;
    return $this;
  }

  public function getArguments() {
    return $this->arguments;
  }

  /**
   * @todo Set tags with arguments for better granularity.
   */
  public function getCacheTags() {
    list($name, $display) = explode(':', $this->config['view']);
    return array('views' => array($name, "$name:$display"));
  }

  public function execute() {
    $arguments = $this->getArguments();
    if (!$view = $this->getView()) {
      return array();
    }
    $view->set_arguments($arguments);
    $view->execute();
    $entities = $view->query->get_result_entities($view->result);

    if (empty($entities[1])) {
      return array();
    }
    return $entities[1];
  }

  public function adminPath() {
    if (!user_access('administer views')) {
      return FALSE;
    }

    list($name, $display) = explode(':', $this->config['view']);
    return 'admin/structure/views/view/' . check_plain($name) . '/edit/' . $display;
  }

  public function configForm(&$form_state) {
    $form = array();
    $form['view'] = array(
      '#type' => 'select',
      '#title' => t('View'),
      '#description' => t('Select which view to use for this handler. Only view display types that returns a context will work.'),
      '#options' => $this->getViewOptions(),
      '#default_value' => array($this->config['view']),
    );
    return $form;
  }
}
