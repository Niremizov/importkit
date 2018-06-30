<?php

/**
 * @uses views_handler_field_numeric
 */
class gf_stock_handler_filter_stock extends views_handler_filter_numeric{

  use gf_stock_handler_trait; 

  /**
   * {@inheritdoc}
   */
  public function admin_summary() {
    if ($this
      ->is_a_group()) {
      return t('grouped');
    }
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }
    $output = strtoupper($this->options['gf_stock_region']) . ' ';

    $options = $this
      ->operator_options('short');
    $output .= check_plain($options[$this->operator]);
    if (in_array($this->operator, $this
      ->operator_values(2))) {
      $output .= ' ' . t('@min and @max', array(
        '@min' => $this->value['min'],
        '@max' => $this->value['max'],
      ));
    }
    elseif (in_array($this->operator, $this
      ->operator_values(1))) {
      $output .= ' ' . check_plain($this->value['value']);
    }
    return $output;
  }


  public function query() {
    $table_alias = $this->ensure_my_table();
    $params = $this->options['group_type'] != 'group' ?
      array('function' => $this->options['group_type']) : array();
    $nid = 'node.nid';
    $region = $this->options['gf_stock_region'];
    $field_alias = $table_alias . '_region_stock_' . $region;
    $expression = "(SELECT SUM(stock) FROM {importkit_cml_stock} 
      WHERE nid = {$nid} AND region = '$region')";

    $info = $this->operators();
        if (!empty($info[$this->operator]['method'])) {
          $this->{$info[$this->operator]['method']}($expression);
        }
  }


  /**
   * Сравнение по вхождение(или невхождение) в диапазон значений.
   */
  function op_between($expression) {
    $op = ($this->operator == 'between') ? 'BETWEEN' : 'NOT BETWEEN';
    $this->query->add_where_expression($this->options['group'], 
      "$expression $op :min AND :max",
      [
        ':min' => $this->value['min'],
        ':max' => $this->value['max']
      ]);
  }

  /**
   * Простое сравнение (типа больше/меньше).
   */
  function op_simple($expression) {
    $this->query->add_where_expression($this->options['group'], 
      "$expression $this->operator :value",
      [':value' => $this->value["value"]]);                              
  }

  /**
   * Пустое значение.
   */
  function op_empty($expression) {
    $op = ($this->operator == 'empty') ? 'IS NULL' : 'IS NOT NULL';

    $this->query->add_where_expression($this->options['group'], 
      "$expression $op");
  }

  /**
   * Соответствие по регулярке.
   */
  function op_regex($expression) {
    $this->query->add_where_expression($this->options['group'], 
      "$expression RLIKE :value",
      [':value' => $this->value["value"]]);                              
  }
}