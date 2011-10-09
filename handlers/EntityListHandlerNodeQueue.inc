<?php

class EntityListHandlerNodeQueue implements EntityListHandler {

  protected $config = array();

  function __construct(Array $config = array()) {
    $defaults = array(
      'name' => '',
    );
    $this->config = array_merge($defaults, $config);
  }

  public function entityType() {
    return 'node';
  }

  public function argumentInput() {
    return NULL;
  }

  public function setArguments(Array $arguments = array()) {
    return $this;
  }

  public function getArguments() {}

  public function execute() {
    $qid_map = nodequeue_get_qid_map();
    $qid = $qid_map[$this->config['name']];
    $nodes = nodequeue_load_nodes($qid);
    return $nodes;
  }

  public function adminPath() {
    
  }

  public function configForm(&$form_state) {

  }
}
