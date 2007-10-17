<?php

/**
 * @return int
 * @desc returns microseconds
*/
function k_getmicrotime()
{
        list($usec, $sec) = explode(" ",microtime());
        $result = (string)floor($usec * 10000);
        $result = str_pad($result, 4, '0', STR_PAD_LEFT);
        return $result;
}

class db_driver_mysql {

    public $obj = array ( "sql_database"     => ""         ,
                       "sql_user"         => "root"     ,
                       "sql_pass"         => ""         ,
                       "sql_host"         => "localhost",
                       "sql_port"         => ""         ,
                       "persistent"       => "0"        ,
                       "sql_tbl_prefix"   => ""         ,
                       "cached_queries"   => array()    ,
                       'debug'            => 0          ,
                       'hooks'            => array()    ,
                     );
     public  $debug_html                = ""; // debug message
     public  $error             = "";
     private $sql               = "";
     public  $cur_query         = ""; // current query. Can be hooked by query hook
     private $query_id          = "";
     private $connection_id     = "";
     public  $query_count       = 0; // selfeplanatory
     public  $return_die        = 0;
     public  $failed            = 0;
     public  $manual_addslashes = 0;
     private $prefix_changed    = 0;
     private $hooks             = array(); // data and query hooks
     public  $cur_data          = array(); // current data. Can be hooked by data hook
     public  $record_row        = array();
     private $loaded_classes    = array();
     public  $connect_vars          = array();
     public  $force_data_type   = array();
     private $database = '';

        /**
         * Object constructor
         * Deprecated!!!
         */
        public function __construct($settings, $database = '')
        {
                //--------------------------
                // Set up any required connect
                // vars here:
                // Will be populated by obj
                // caller
                //--------------------------

                $this->database = $database;

             $this->connect_vars = array();

             if (isset($settings) && is_array($settings) && !empty($settings))
                     $this->init($settings);
        }

        public function getSyncDatabase()
        {
                return $this->database;
        }

        public function init($settings)
        {
                foreach ($settings as $key=>$value)
                        if (isset($this->obj[$key]))
                                $this->obj[$key] = $value;
        }

    /*========================================================================*/
    // Connect to the database
    /*========================================================================*/

        public function connect()
        {
                foreach ($this->obj['hooks'] as $hook)
                     $this->load_hook_file($hook[0], $hook[1]);

             //--------------------------
             // Connect
             //--------------------------

            if ($this->obj['persistent'])
                $this->connection_id = mysql_pconnect( $this->obj['sql_host'] ,
                                                                                                   $this->obj['sql_user'] ,
                                                                                                   $this->obj['sql_pass'],
                                                                                                   true
                                                                                                );
        else
                        $this->connection_id = mysql_connect( $this->obj['sql_host'] ,
                                                                                                  $this->obj['sql_user'] ,
                                                                                                  $this->obj['sql_pass'],
                                                                                                  true
                                                                                                );
                if ( ! $this->connection_id )
                {
                        $this->fatal_error('wrong connection id');
                        return FALSE;
                }

        if ( ! mysql_select_db($this->obj['sql_database'], $this->connection_id) )
        {
                $this->fatal_error('wrong db');
                return FALSE;
        }

        return TRUE;
    }

        /*========================================================================*/
        // Set debug mode
        /*========================================================================*/

        public function set_debug_mode( $int=0 )
        {
                $this->obj['debug'] = intval($int);
        }

        /*========================================================================*/
        // Load hook file
        /*========================================================================*/

    public function load_hook_file( $filepath, $classname )
    {
            global $config;
            if ( ! file_exists( $filepath ) )
                    $this->fatal_error("Cannot locate $filepath - exiting!");
            else
            {
                    if (!class_exists($classname))
                            require_once( $filepath );
                    $this->$classname       = new $classname( &$this );
                    $this->loaded_classes[] = $classname;
                    $this->hooks[] = new $classname( $this );
            }
    }

    private function perform_data_hook($place)
    {
            foreach ($this->hooks as $hook){
                    if (($hook->kind == 'data') && ($hook->place == $place))
                            $hook->perform();
            }
    }

    private function perform_query_hook($place)
    {
            foreach ($this->hooks as $hook){
                    if (($hook->kind == "query") && ($hook->place == $place))
                            $hook->perform();
            }
    }

    /*========================================================================*/
    // Simple elements
    /*========================================================================*/

        public function construct( $a )
        {
                if ( isset($a['select']) or ($a['action'] == 'select'))
                {
                        $this->select( $a['table'], $a['fields'], @$a['where'] );
                }

                if (isset($a['insert']) or ($a['action'] == 'insert') )
                {
                        $this->insert( $a['table'], $a['data']);
                }

                if (isset($a['update'])  or ($a['action'] == 'update'))
                {
                        $this->update( $a['table'], $a['set'], $a['where'], isset($a['lowpro'])?$a['lowpro']:'' );
                }

            if ( isset($a['delete']) or ($a['action'] == 'delete'))
            {
                    $this->delete( $a['table'], $a['where'] );
            }

            if ( isset($a['order']) )
            {
                    $this->order( $a['order'] );
            }

            if (isset($a['limit']) and is_array( $a['limit'] ) )
            {
                    $this->limit( $a['limit'][0], @$a['limit'][1] );
            }
    }

        //------------------------------------
        // INSERT
        //------------------------------------

        public function insert($table, $array)
        {
                // hooking data
                $this->cur_data = array('action' => 'insert',
                                                                'table' => $table,
                                                                'data' => $array);

                if (is_array($table))
                        $table = join(', ', $table);

                $dba = $this->compile_db_insert_string($array);

                $this->cur_query = 'INSERT INTO ' . $this->addPrefixes($table) . ' (' .
                                                        $dba['FIELD_NAMES'] . ') VALUES( ' . $dba['FIELD_VALUES'] . ')';
        }

    //------------------------------------
    // UPDATE
    //------------------------------------

        public function update($table, $set, $where, $low_priority = "" )
        {
                // hooking data
                $this->cur_data = array('action' => 'update',
                                                                'table' => $table,
                                                                'set' => $set);

                if (is_array($set))
                        $set = $this->compile_db_update_string($set);

                if (!empty($low_priority))
                {
                        $low_priority = 'LOW_PRIORITY';
                        $this->cur_data['lowpro'] = $low_priority;
                }

                if (is_array($table))
                        $table = join(', ', $table);

                $this->cur_query = 'UPDATE ' . $low_priority . ' ' . $this->addPrefixes($table) . ' SET ' . $set;

                if ( $where )
                {
                        if (is_array($where))
                                $where = $this->compile_where_string($where);

                        $this->cur_query .= ' WHERE ' . $where;
                        $this->cur_data['where'] = $where;
                }
        }

    //------------------------------------
    // DELETE
    //------------------------------------

        public function delete($table, $where)
        {
                // hooking data
                $this->cur_data = array('action' => 'delete',
                                                                'table' => $table);

                if (is_array($table))
                        $table = join(', ', $table);

                $this->cur_query = 'DELETE FROM ' . $this->obj['sql_tbl_prefix'] . $table;

                if ( $where )
                {
                        if (is_array($where))
                            $where = $this->compile_where_string($where);

                        $this->cur_query .= ' WHERE ' . $where;
                        $this->cur_data['where'] = $where;
                }
        }

    //------------------------------------
    // EXEC QUERY
    //------------------------------------

    /**
     * @return resoutce or boolran
     */

        public function exec()
        {
        	//if (isset($_REQUEST['debug']))
             //  echo "<br>=>".$this->cur_query;
                if ( $this->cur_query != "" )
                        $ci = $this->query();

                // hooking data

                $this->cur_query   = "";
                $this->cur_data    = array();
                return $ci;
        }

    //------------------------------------
    // Exec and return simple row
    //------------------------------------

        public function exec_query( $a )
        {
                $this->construct( $a );

                $ci = $this->exec();

                if ( $a['select'] )
                        return $this->fetch_row( $ci );
        }

        public function exec_complex_query($query)
        {
                return $this->query($query);
        }

    //------------------------------------
    // ORDER
    //------------------------------------

        public function order( $a )
        {
                // hooking data
                $this->cur_data["order"] = $a;
                if ( $a )
                        $this->cur_query .= ' ORDER BY '.$a;
        }

    //------------------------------------
    // LIMIT
    //------------------------------------

        public function limit_with_check( $offset, $limit="" )
        {
                if ( ! preg_match( "#LIMIT\s+?\d+,#i", $this->cur_query ) )
                        $this->limit( $offset, $limit );
        }

        public function limit( $offset, $limit="" )
    {
            // hooking data
            $this->cur_data["limit"] = array($offset, $limit);
            if ( $limit )
                    $this->cur_query .= ' LIMIT '.$offset.','.$limit;
            else
                    $this->cur_query .= ' LIMIT '.$offset;
    }

    //------------------------------------
    // SELECT
    //------------------------------------

    public function addPrefixes($tables)
    {
            if (!empty($tables))
            {
                    $tbls = explode(',', $tables);
                    foreach ($tbls as $index=>$value)
                    {
                            $tbls[$index] = $this->obj['sql_tbl_prefix'] . trim($value);
                    }
                    return join(', ', $tbls);
            }
            else
                    return '';
    }

        public function select($table, $fields, $where="" )
    {
            // hooking data
            $this->cur_data = array("action" => "select",
                                                            "table" => $table,
                                                            "fields" => $fields,
                                                            "where" => $where);

            if (is_array($where))
                    $where = $this->compile_db_update_string($where);

            if (is_array($fields))
                    $fields = join(', ', $fields);

            if (is_array($table))
                    $table = join(', ', $table);

            $this->cur_query = 'SELECT ' . $fields . ' FROM ' . $this->addPrefixes($table);

            if (!empty($where))
            {
                    $this->cur_query .= ' WHERE ' . $where;
                    $this->cur_data['where'] = $where;
            }
    }

	
	public function queryCollect()
	{
		$this->construct($this->cur_data);
		return $this->cur_query;
	}

    /*========================================================================*/
    // Process a manual query
    /*========================================================================*/

    public  function query($the_query = '', $bypass=0)
    {

            //--------------------------------------
        // Change the table prefix if needed
        //--------------------------------------

        if ($this->obj['debug'])
               $start_time = k_getmicrotime();

            // hooking query
            if (empty($the_query ))
            {
            	$this->perform_data_hook('preexec');
            	$this->construct($this->cur_data);
                    $the_query = $this->cur_query;
            }
            $this->perform_query_hook('preexec');
            //echo $the_query.'<br>';
        	$this->query_id = mysql_query($the_query, $this->connection_id);

        // hooking query
            $this->perform_data_hook('postexec');
            $this->perform_query_hook('postexec');

              //--------------------------------------
              // Reset array...
              //--------------------------------------

              $this->force_data_type = array();

        if (! $this->query_id )
            $this->fatal_error("mySQL query error: $the_query");

        //--------------------------------------
        // Debug?
        //--------------------------------------

        if ($this->obj['debug'])
        {
                $duration = k_getmicrotime() - $start_time;

                if ($this->cur_data['action'] === 'select')
                {
                        $eid = mysql_query("EXPLAIN $the_query", $this->connection_id);

                        $this->debug_html .= "<table width='95%' border='1' cellpadding='6' cellspacing='0' bgcolor='#FFE8F3' align='center'>
                                                                                   <tr>
                                                                                            <td colspan='8' style='font-size:14px' bgcolor='#FFC5Cb'><b>Select Query</b></td>
                                                                                   </tr>
                                                                                   <tr>
                                                                                    <td colspan='8' style='font-family:courier, monaco, arial;font-size:14px;color:black'>$the_query</td>
                                                                                   </tr>
                                                                                   <tr bgcolor='#FFC5Cb'>
                                                                                         <td><b>table</b></td><td><b>type</b></td><td><b>possible_keys</b></td>
                                                                                         <td><b>key</b></td><td><b>key_len</b></td><td><b>ref</b></td>
                                                                                         <td><b>rows</b></td><td><b>Extra</b></td>
                                                                                   </tr>\n";
                                while( $array = mysql_fetch_array($eid) )
                                {
                                        $type_col = '#FFFFFF';

                                        if ($array['type'] == 'ref' or $array['type'] == 'eq_ref' or $array['type'] == 'const')
                                        {
                                                $type_col = '#D8FFD4';
                                        }
                                        else if ($array['type'] == 'ALL')
                                        {
                                                $type_col = '#FFEEBA';
                                        }

                                        $this->debug_html .= "<tr bgcolor='#FFFFFF'>
                                                                                         <td>$array[table]&nbsp;</td>
                                                                                         <td bgcolor='$type_col'>$array[type]&nbsp;</td>
                                                                                         <td>$array[possible_keys]&nbsp;</td>
                                                                                         <td>$array[key]&nbsp;</td>
                                                                                         <td>$array[key_len]&nbsp;</td>
                                                                                         <td>$array[ref]&nbsp;</td>
                                                                                         <td>$array[rows]&nbsp;</td>
                                                                                         <td>$array[Extra]&nbsp;</td>
                                                                                   </tr>\n";
                                }

                                $this->sql_time += $endtime;

                                if ($endtime > 0.1)
                                {
                                        $endtime = "<span style='color:red'><b>$endtime</b></span>";
                                }

                                $this->debug_html .= "<tr>
                                                                                  <td colspan='8' bgcolor='#FFD6DC' style='font-size:14px'><b>MySQL time</b>: $endtime</b></td>
                                                                                  </tr>
                                                                                  </table>\n<br />\n";
                        }
                        else
                        {
                          $this->debug_html .= "<table width='95%' border='1' cellpadding='6' cellspacing='0' bgcolor='#FEFEFE'  align='center'>
                                                                                 <tr>
                                                                                  <td style='font-size:14px' bgcolor='#EFEFEF'><b>Non Select Query</b></td>
                                                                                 </tr>
                                                                                 <tr>
                                                                                  <td style='font-family:courier, monaco, arial;font-size:14px'>$the_query</td>
                                                                                 </tr>
                                                                                 <tr>
                                                                                  <td style='font-size:14px' bgcolor='#EFEFEF'><b>MySQL time</b>: $endtime</span></td>
                                                                                 </tr>
                                                                                </table><br />\n\n";
                        }
                }
                else {
                        $this->debug_html = "";
            }

                $this->query_count++;

        $this->obj['cached_queries'][] = $the_query;

        return $this->query_id;
    }


    /*========================================================================*/
    // Fetch a row based on the last query
    // Result type: num, assoc, both
    /*========================================================================*/

    private function fetch_row($query_id = "", $result_type = "assoc")
    {

            if ($query_id == "")
                    $query_id = $this->query_id;

            switch (strtolower($result_type))
            {
                    case 'assoc':
                        $this->record_row = mysql_fetch_array($query_id, MYSQL_ASSOC);
                        break;
                case 'num':
                        $this->record_row = mysql_fetch_array($query_id, MYSQL_NUM);
                        break;
                default:
                        $this->record_row = mysql_fetch_array($query_id, MYSQL_BOTH);
                                break;
            }

        return $this->record_row;
    }

        public function getAssocRow($query_id = "")
        {
                return $this->fetch_row($query_id, 'assoc');
        }

        public function getNumRow($query_id = "")
        {
                return $this->fetch_row($query_id, 'num');
        }

        public function getRow($query_id = "")
        {
                return $this->fetch_row($query_id, 'both');
        }

        public function take()
        {
                $result = @mysql_result($this->query_id, 0);
                //echo "<br>=><b>".$this->cur_query."</b>";
                mysql_free_result($this->query_id);
                $this->query_id = 0;
                return $result;
        }

        private function fetch_result($query_id = "", $result_type = "assoc")
        {
                $result = array();
                while (($row = $this->fetch_row($query_id, $result_type)) !== false)
                {
                        $result[] = $row;
                }

                return $result;
        }

        public function getArray($query_id = "")
        {
                return $this->fetch_result($query_id, "both");
        }

        public function getAssocArray($query_id = "")
        {
                return $this->fetch_result($query_id, "assoc");
        }

        public function getNumArray($query_id = "")
        {
                return $this->fetch_result($query_id, "num");
        }




        /*========================================================================*/
    // Test to see if a field exists by forcing and trapping an error.
    // It ain't pretty, but it do the job don't it, eh?
    // Posh my ass.
    // Return 1 for exists, 0 for not exists and jello for the naked guy
    // Fun fact: The number of times I spelt 'field' as 'feild'in this part: 104
    /*========================================================================*/

    function field_exists($field, $table)
    {

                $this->return_die = 1;
                $this->error = "";

                $this->query("SELECT COUNT($field) as count FROM $table");

                $return = 1;

                if ( $this->failed )
                        $return = 0;

                $this->error = "";
                $this->return_die = 0;
                $this->error_no   = 0;
                $this->failed     = 0;

                return $return;
        }

        /*========================================================================*/
        // DROP TABLE
        /*========================================================================*/

        function sql_drop_table( $table )
        {
                $this->query( "DROP TABLE if exists ".SQL_PREFIX."{$table}" );
        }

        /*========================================================================*/
        // DROP FIELD
        /*========================================================================*/

        function sql_drop_field( $table, $field )
        {
                $this->query( "ALTER TABLE ".SQL_PREFIX."{$table} DROP $field" );
        }

        /*========================================================================*/
        // ADD FIELD
        /*========================================================================*/

        function sql_add_field( $table, $field_name, $field_type, $field_default="''" )
        {
                $this->query( "ALTER TABLE ".SQL_PREFIX."{$table} ADD $field_name $field_type default {$field_default}" );
        }

        /*========================================================================*/
        // OPTIMIZE FIELD
        /*========================================================================*/

        function sql_optimize_table( $table )
        {
                $this->query( "OPTIMIZE TABLE ".SQL_PREFIX."{$table}" );
        }

        /*========================================================================*/
        // ADD FULLTEXT INDEX
        /*========================================================================*/

        function sql_add_fulltext_index( $table, $field )
        {
                $this->query( "alter table ".SQL_PREFIX."{$table} ADD FULLTEXT({$field})" );
        }

        /*========================================================================*/
        // GET TABLE SCHEMATIC
        /*========================================================================*/

        function sql_get_table_schematic( $table )
        {
                $this->query( "SHOW CREATE TABLE ".SQL_PREFIX."{$table}" );
                return $this->fetch_row();
        }

        /*========================================================================*/
        // IS ALREADY TABLE FULLTEXT?
        /*========================================================================*/

        function sql_is_currently_fulltext( $table )
        {
                $result = $this->sql_get_table_schematic( $table );

                if ( preg_match( "/FULLTEXT KEY/i", $result['Create Table'] ) )
                        return TRUE;
                else
                        return FALSE;
        }

        /*========================================================================*/
        // Return the version number of the SQL server
        // Should return 'true' version string (ie: 3.23.0)
        // And formatted string (ie: 3230 )
        /*========================================================================*/

        function sql_get_version()
        {
                if ( ! $this->mysql_version and ! $this->true_version )
                {
                        $this->query("SELECT VERSION() AS version");

                        if ( ! $row = $this->fetch_row() )
                        {
                                $this->query("SHOW VARIABLES LIKE 'version'");
                                $row = $this->fetch_row();
                        }

                        $this->true_version = $row['version'];

                        $no_array = explode( '.', preg_replace( "/^(.+?)[-_]?/", "\\1", $row['version']) );

                        $one   = (!isset($no_array) || !isset($no_array[0])) ? 3  : $no_array[0];
                        $two   = (!isset($no_array[1]))                      ? 21 : $no_array[1];
                        $three = (!isset($no_array[2]))                      ? 0  : $no_array[2];

                        $this->mysql_version = (int)sprintf('%d%02d%02d', $one, $two, intval($three));
                   }
        }

        /*========================================================================*/
        // sql_can_fulltext
        // returns whether SQL engine has fulltext abilities
        // returns TRUE or FALSE
        /*========================================================================*/

        function sql_can_fulltext()
        {
                $this->sql_get_version();

                if ( $this->mysql_version >= 32323 )
                        return TRUE;
                else
                        return FALSE;
        }

        /*========================================================================*/
        // sql_can_fulltext_boolean
        // returns whether SQL engine has boolean fulltext abilities
        // (+word -word, etc)
        // returns TRUE or FALSE
        /*========================================================================*/

        function sql_can_fulltext_boolean()
        {
                $this->sql_get_version();

                if ( $this->mysql_version >= 40010 )
                        return TRUE;
                else
                        return FALSE;
        }


        /*========================================================================*/
        // Flushes the in memory query string
        /*========================================================================*/

        function flush_query()
        {
                $this->cur_query = "";
        }

        /*========================================================================*/
    // Fetch the number of rows affected by the last query
    /*========================================================================*/

    function get_affected_rows()
    {
        return mysql_affected_rows($this->connection_id);
    }

    /*========================================================================*/
    // Fetch the number of rows in a result set
    /*========================================================================*/

    function get_num_rows()
    {
        return mysql_num_rows($this->query_id);
    }

    /*========================================================================*/
    // Fetch the last insert id from an sql autoincrement
    /*========================================================================*/

    private function get_insert_id()
    {
            if (mysql_affected_rows($this->connection_id) >= 0)
                return mysql_insert_id($this->connection_id);
        else
                return false;
    }

    public function getLastInsertId()
    {
                return mysql_insert_id($this->connection_id);

            //return $this->get_insert_id();
    }

    /*========================================================================*/
    // Return the amount of queries used
    /*========================================================================*/

    function get_query_cnt()
    {
        return $this->query_count;
    }

    /*========================================================================*/
    // Free the result set from mySQLs memory
    /*========================================================================*/

    function free_result($query_id="")
    {

                   if ($query_id == "")
                    $query_id = $this->query_id;

            @mysql_free_result($query_id);
    }

    /*========================================================================*/
    // Shut down the database
    /*========================================================================*/

    function close_db()
    {
            if ( $this->connection_id )
                    return mysql_close( $this->connection_id );
    }

    /*========================================================================*/
    // Return an array of tables
    /*========================================================================*/

    function get_table_names()
    {

                $result     = mysql_list_tables($this->obj['sql_database']);
                $num_tables = mysql_numrows($result);
                for ($i = 0; $i < $num_tables; $i++)
                {
                        $tables[] = mysql_tablename($result, $i);
                }

                mysql_free_result($result);

                return $tables;
           }

           /*========================================================================*/
    // Return an array of fields
    /*========================================================================*/

	function get_result_fields($query_id="")
	{
		if ($query_id == "")
			$query_id = $this->query_id;

		while ($field = mysql_fetch_field($query_id))
			$Fields[] = $field;

		return $Fields;
	}

    /*========================================================================*/
    // Basic error handler
    /*========================================================================*/

    function fatal_error($the_error="")
    {
            // Are we simply returning the error?

            $this->error    = mysql_error();
            $this->error_no = mysql_errno();

            if ($this->return_die == 1)
            {
                    $this->failed = 1;
                    return;
            }

            $the_error .= "\n\nmySQL error: ".mysql_error()."\n";
            $the_error .= "mySQL error code: ".$this->error_no."\n";
            $the_error .= "Date: ".date("l dS of F Y h:i:s A");

            $out = "<html><head><title>Database Error</title>
                       <style>P,BODY{ font-family:arial,sans-serif; font-size:11px; }</style></head><body>
                       &nbsp;<br><br><blockquote><b>There appears to be an error with the database.</b><br>
                       You can try to refresh the page by clicking <a href=\"javascript:window.location=window.location;\">here</a>.
                       <br><br><b>Error Returned</b><br>
                       <pre style=\"border: 1px solid #FF0000; padding 4px;\">" . htmlspecialchars($the_error)."</pre><br>We apologise for any inconvenience</blockquote></body></html>";
    	die($out);
    }

    /*========================================================================*/
    // Create an array from a multidimensional array returning formatted
    // strings ready to use in an INSERT query, saves having to manually format
    // the (INSERT INTO table) ('field', 'field', 'field') VALUES ('val', 'val')
    /*========================================================================*/

    function compile_db_insert_string($data)
    {

            $field_names  = "";
                $field_values = "";

                if (empty($data) || !is_array($data))
                        return ;

                foreach ($data as $k => $v)
                {
                        if ( ! $this->manual_addslashes )
                                $v = $this->add_slashes($v);

                        $field_names  .= "$k,";

                        //-----------------------------------------
                        // Forcing data type?
                        //-----------------------------------------

                        if ( isset($this->force_data_type[ $k ]) and $this->force_data_type[ $k ] )
                        {
                                if ( $this->force_data_type[ $k ] == 'string' )
                                        $field_values .= "'$v',";
                                else if ( $this->force_data_type[ $k ] == 'int' )
                                        $field_values .= intval($v).",";
                                else if ( $this->force_data_type[ $k ] == 'float' )
                                        $field_values .= floatval($v).",";
                        }

                        //-----------------------------------------
                        // No? best guess it is then..
                        //-----------------------------------------

                        else
                        {
                                if ( is_numeric( $v ) and intval($v) == $v )
                                        $field_values .= $v.",";
                                else
                                        $field_values .= "'$v',";
                        }
                }

                $field_names  = preg_replace( "/,$/" , "" , $field_names  );
                $field_values = preg_replace( "/,$/" , "" , $field_values );

                return array( 'FIELD_NAMES'  => $field_names,
                                          'FIELD_VALUES' => $field_values,
                                        );
        }

        /*========================================================================*/
    // Create an array from a multidimensional array returning a formatted
    // string ready to use in an UPDATE query, saves having to manually format
    // the FIELD='val', FIELD='val', FIELD='val'
    /*========================================================================*/

    function compile_db_update_string($data)
    {

                $return_string = "";

                foreach ($data as $k => $v)
                {
                        if ( ! $this->manual_addslashes )
                                $v = $this->add_slashes($v);

                        //-----------------------------------------
                        // Forcing data type?
                        //-----------------------------------------
                        if ( isset($this->force_data_type[ $k ] ))
                        {
                                if ( $this->force_data_type[ $k ] == 'string' )
                                        $return_string .= $k . "='".$v."',";
                                else if ( $this->force_data_type[ $k ] == 'int' )
                                        $return_string .= $k . "=".intval($v).",";
                                else if ( $this->force_data_type[ $k ] == 'float' )
                                        $return_string .= $k . "=".floatval($v).",";
                        }

                        //-----------------------------------------
                        // No? best guess it is then..
                        //-----------------------------------------

                        else
                        {
                                if ( is_numeric( $v ) and intval($v) == $v )
                                        $return_string .= $k . "=".$v.",";
                                else
                                        $return_string .= $k . "='".$v."',";
                        }
                }

                $return_string = preg_replace( "/,$/" , "" , $return_string );

                return $return_string;
        }

        function compile_where_string($data)
        {

                $return_string = "";

                foreach ($data as $k => $v)
                {
                        if ( ! $this->manual_addslashes )
                                $v = $this->add_slashes($v);
                        else
                        {
                                if ( is_numeric( $v ) and intval($v) == $v )
                                        $return_string[] = $k . "=".$v;
                                else
                                        $return_string[] = $k . "='".$v."'";
                        }
                }

                $return_string = join(' AND ', $return_string);

                return $return_string;
        }

        /*========================================================================*/
        // Use different escape method for different SQL engines
        /*========================================================================*/

        function add_slashes( $t )
        {
                //return preg_replace( "/'/", "\\'", $t );
                //return addcslashes($t, '\'"\\');
                return mysql_real_escape_string($t, $this->connection_id);
        }

        /*========================================================================*/
        // Generates SystemID
        /*========================================================================*/

        public function getQueryId()
        {
                return $this->query_id;
        }

} // end class

?>
