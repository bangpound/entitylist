<?php

class EntityList {

  public $name = '';

  public $handler_plugin = NULL;

  public $handler_config = array();

  public $arguments = array();

  public $entities = array();

  protected $handler = NULL;

  protected $executed = FALSE;

  /**
   * Load method that loads the configuration into the object itself.
   *
   * Since the CTools Export API is declarative by nature, we can't rely on
   * constructor injection and entitylist_list_create() as the only factory.
   */
  public function load() {
    $schema = drupal_get_schema('entitylist_lists');
    // Unserialize our serialized params.
    foreach ($schema['fields'] as $field => $info) {
      if (!empty($info['serialize']) && !is_array($this->{$field})) {
        $this->{$field} = (array) unserialize($this->{$field});
      }
    }
    $this->handler = new $this->handler_plugin($this->handler_config);
    return $this;
  }

  public function entityType() {
    return $this->handler->entityType();
  }

  public function argumentInput() {
    return $this->handler->argumentInput();
  }

  public function setArguments($arguments = array()) {
    $this->handler->setArguments($arguments);
    return $this;
  }

  public function getArguments() {
    return $this->handler->getArguments();
  }

  /**
   * @todo Set tags with arguments for better granularity.
   */
  public function getCacheTags($execute = FALSE) {
    $tags = $this->handler->getCacheTags();
    // Always add the list it self as a cache tag.
    $tags['entitylist'][] = $this->name;
    if ($execute) {
      $entity_type = $this->entityType();
      $entities = $this->execute();
      foreach ($entities as $id => $entity) {
        $tags[$entity_type][] = $id;
      }
    }
    return $tags;
  }

  public function execute() {
    if (!$this->executed) {
      $indexed_entities = array();
      $entities = $this->handler->execute();
      if (!empty($entities)) {
        foreach ($entities as $entity) {
          $indexed_entities[] = $entity;
        }
        $this->entities = $indexed_entities;
      }
      $this->executed = TRUE;
    }
    return $this->entities;
  }

  protected function setEntities($result) {
    $this->entities = $result;
  }

  public function getEntities() {
    if (!$this->executed) {
      $this->execute();
    }
    return $this->entities;
  }

  public function getEntity($index) {
    $entities = $this->getEntities();
    if (isset($entities[$index])) {
      return $entities[$index];
    }
    else {
      return FALSE;
    }
  }

  public function adminPath() {
    return $this->handler->adminPath();
  }

  public function configForm(&$form_state) {
    return $this->handler->configForm($form_state);
  }
}
