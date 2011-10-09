<?php

class EntityListException extends Exception {}

interface EntityListHandler {

  /**
   * Constructor to setup and configure the handler.
   */
  public function __construct(Array $config = array());

  /**
   * Returns what entity type this list lists.
   *
   * @return string
   */
  public function entityType();

  /**
   * Returns the argument input needed for this list. Mostly used by the CTools
   * integration to build and set required contexts.
   *
   * It should be in the following form:
   * @code
   *   return array(
   *     'node.nid' => array(
   *       'label' => t('Node NID'),
   *       'required' => TRUE,
   *     ),
   *   );
   * @endcode
   *
   * The key 'node.nid' is a ctools context key. It can be in any of the
   * following formats:
   *   - node
   *   - node.nid
   *   - entity:taxonomy_term
   *   - entity:taxonomy_term.tid
   *
   * @return array
   */
  public function argumentInput();

  /**
   * Setter for the list's arguments.
   *
   * @return $this
   */
  public function setArguments(Array $arguments = array());

  /**
   * Getter for the list's arguments. Mostly used internally.
   *
   * @return array
   */
  public function getArguments();

  /**
   * Execute the handler. This function should return a numeric array of
   * entities, simply keyed by the result index.
   *
   * @see EntityFieldQuery::execute()
   * @return array
   */
  public function execute();

  /**
   * Returns the path to the administrative interface for this list.
   *
   * @return string
   */
  public function adminPath();

  /**
   * Form to configure the handler.
   */
  public function configForm(&$form_state);
}
