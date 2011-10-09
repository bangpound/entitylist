<?php

class EntityListHandlerEFQ implements EntityListHandler {

  protected $config = array();

  protected $arguments = array();

  public function __construct(Array $config = array()) {
    $defaults = array(
      'class_name' => '',
    );
    $this->config = array_merge($defaults, $config);
  }

  public function entityType() {
    return call_func_array(array($this->config['class'], 'entityType'));
  }

  public function argumentInput() {
    return call_func_array(array($this->config['class'], 'argumentInput'));
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
    $class_name = $this->config['class_name'];
    // TODO: Check that $class_name implements EntityListEFQ.
    $query = new $class_name($this->config, $arguments);
    return $query->execute();
  }

  public function adminPath() {

  }

  public function configForm(&$form_state) {
    
  }
}

interface EntityListEFQ {

  public function __construct(Array $config, Array $arguments = array());

  static function entityType();

  static function argumentInput();
}
