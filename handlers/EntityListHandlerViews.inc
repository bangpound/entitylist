<?php

class EntityListHandlerViews implements EntityListHandler {

  protected $view = NULL;

  protected $config = array();

  protected $entityType = NULL;

  protected $argumentInput = array();

  protected $arguments = array();

  public function __construct(Array $config = array()) {
    $defaults = array(
      'name' => '',
    );
    $this->config = array_merge($defaults, $config);
    // Load view to make it available to this handler.
    // TODO: Can we lazy load this to save some memory?
    if (!empty($this->config['name'])) {
      list($name, $display) = explode(':', $this->config['name']);
      $this->view = views_get_view($name);
      if (!empty($this->view)) {
        $this->view->set_display($display);
      }
    }
  }

  public function entityType() {
    if ($this->entityType === NULL) {
      $table = views_fetch_data($this->view->base_table);
      if (!isset($table['table']['base']['entity type'])) {
        throw new EntityListException(t('Could not determine entity type of view @view', array('@view' => $this->view->name)));
      }
      $this->entityType = $table['table']['base']['entity type'];
    }
    return $this->entityType;
  }

  public function argumentInput() {
    if (empty($this->argumentInput) && !empty($this->view)) {
      // This code is pretty much taken directly from views_content.module.
      // TODO: Simplify when we've got our own stripped down Views display type.
      $arguments = $this->view->display_handler->get_argument_input();
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

  public function execute() {
    $arguments = $this->getArguments();
    $this->view->set_arguments($arguments);
    $this->view->execute();
    $entities = $this->view->query->get_result_entities($this->view->result);

    if (empty($entities[1])) {
      return array();
    }
    return $entities[1];
  }

  public function adminPath() {
    list($name, $display) = explode(':', $this->config['name']);
    return 'admin/structure/views/view/' . check_plain($name) . '/edit/' . $display;
  }

  public function configForm(&$form_state) {
    $form = array();
    $options = views_get_views_as_options(FALSE, 'enabled', NULL, TRUE);
    $form['name'] = array(
      '#type' => 'select',
      '#title' => t('View'),
      '#description' => t('Select which view to use for this handler.'),
      '#options' => $options,
      '#default_value' => array($this->config['name']),
    );
    return $form;
  }
}
