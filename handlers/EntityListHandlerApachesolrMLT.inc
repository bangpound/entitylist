<?php

use Symfony\Component\Yaml\Yaml;

class EntityListHandlerApachesolrMLT extends EntityListHandlerApachesolr {

  public function __construct(Array $config = array()) {
    parent::__construct($config);
    $defaults = array(
      'settings' => array(),
    );
    $this->config = array_merge($defaults, $config);
  }

  public function requiredArguments() {
    return array(
      'entity' => array(
        'label' => 'Entity',
        'required' => TRUE,
      ),
    );
  }

  protected function searchArguments() {
    if (empty($this->searchArguments)) {
      $arguments = $this->getArguments();
      $node = $arguments[0]->data;

      // Search keywords and filters.
      $params = $this->config['settings'];
      $params['num_results'] = $this->range['limit'];
      $this->searchArguments = array(
        $params,
        apachesolr_document_id($node->nid),
        $this->searchEnvironment(),
      );
    }
    return $this->searchArguments;
  }

  public function execute() {
    try {
      $entity_type = $this->entityType();
      // Get our specific environment for the MLT block
      $results = call_user_func_array('apachesolr_search_mlt_suggestions', $this->searchArguments());
      $ids = array();
      if ($results) {
        foreach ($results as $result) {
          $ids[] = $result->entity_id;
        }
      }
      if (!empty($ids)) {
        return entity_load($entity_type, $ids);
      }
    }
    catch (Exception $e) {
      watchdog_exception('entitylist', $e);
    }
    return array();
  }

  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);
    $form['settings'] = array(
      '#type' => 'yaml',
      '#title' => t('Settings'),
      '#default_value' => $this->config['settings'],
    );
    return $form;
  }
}
