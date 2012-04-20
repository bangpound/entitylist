<?php

class EntityListHandlerSearchApi implements EntityListHandler {

  protected $config = array();

  protected $arguments = array();

  public function __construct(Array $config = array()) {
    $defaults = array(
      'id' => NULL,
      'sort' => array(
        'field' => '',
        'order' => 'DESC',
      ),
    );
    $this->config = array_merge($defaults, $config);
  }

  public function entityType() {
    // TODO: Load the index and inspect entity type settings.
    return 'node';
  }

  public function argumentInput() {
    return array(
      'string' => array(
        'label' => 'Keywords',
        'required' => FALSE,
      ),
    );
  }

  public function setArguments(Array $arguments = array()) {
    $this->arguments = $arguments;
    return $this;
  }

  public function getArguments() {
    return $this->arguments;
  }

  public function getCacheTags() {
    return array();
  }

  public function execute() {
    try {
      // This handler doesn't deal with filter options because it's too complex
      // and Facet API is doing a good job providing that context through GET
      // parameters. So that will be taken care of further down the stack
      // inside search_api_query(). It actually works.
      // However, we allow keywords and sort, because that's easy.
      $options = array();
      $arguments = $this->getArguments();
      // We only have one argument, so the first one is the keywords.
      $keys = reset($arguments);
      // Sort config.
      if (!empty($this->config['sort']['field'])) {
        // At this point, we only support sorting on one field at the time. We'd
        // have to implement some AJAX magic in the config form to support
        // multiple fields.
        $options['sort'] = array(
          $this->config['sort']['field'] => $this->config['sort']['order'],
        );
      }
      // Execute the query and load the entities.
      $results = search_api_query($this->config['id'], $options)
        ->keys($keys)
        ->execute();
      $ids = array();
      foreach ($results['results'] as $id => $result) {
        $ids[] = $result['id'];
      }
      $entities = entity_load($this->entityType(), $ids);
      return $entities;
    }
    catch (Exception $e) {
      watchdog_exception('entitylist', $e);
    }
    return array();
  }

  public function adminPath() {
    if (!user_access('administer search_api')) {
      return FALSE;
    }
    return 'admin/config/search/search_api/index/' . $this->config['id'] . '/edit';
  }

  public function configForm(&$form_state) {
    $form = array();
    $indices = search_api_index_load_multiple(FALSE);

    $options = array();
    foreach ($indices as $id => $index) {
      $options[$id] = check_plain($index->name);
    }
    $form['id'] = array(
      '#type' => 'select',
      '#title' => t('Index'),
      '#description' => t('Choose what index the query should be made against.'),
      '#options' => $options,
      '#default_value' => array($this->config['id']),
      '#required' => TRUE,
    );

    $form['sort'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Sorting'),
    );

    // At this point, we only support sorting on one field at the time. We'd
    // have to implement some AJAX magic here to support multiple fields.
    $options = array('' => t('Search score'));
    if (!empty($this->config['id']) && !empty($indices[$this->config['id']])) {
      foreach ($indices[$this->config['id']]->options['fields'] as $field_name => $info) {
        $options[$field_name] = $field_name;
      }
    }
    $form['sort']['field'] = array(
      '#type' => 'select',
      '#title' => t('Field'),
      '#options' => $options,
      '#description' => t('Configure what field to sort on. Choosing none will sort on the default <em>score</em> field. Sorting on multiple fields is not supported at the moment. <em>Configuring (and saving) the Index setting first might give you more available fields.</em>'),
      '#default_value' => $this->config['sort']['field'],
    );
    $form['sort']['order'] = array(
      '#type' => 'select',
      '#title' => t('Order'),
      '#options' => array(
        'ASC' => t('Ascending'),
        'DESC' => t('Descending'),
      ),
      '#default_value' => $this->config['sort']['order'],
    );
    return $form;
  }
}
