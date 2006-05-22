<?
	
	require_once('db_mysql.class.php');
	
	set_time_limit(0);	

	$config = array(
					'sql_database' => 'test',
					'sql_user' => 'root',
					'sql_pass' => 'Dmitrik',
					'sql_tbl_prefix' => '',
					'debug' => 0,
					'hooks' => array()
					);

	$DB = new db_driver_mysql($config);
	$DB->connect();

	$arr = array(
					'date' => '2005-07-08',
					'operation' => 'mu'
				);

	$select = array(
					'date','operation'
					);
					
	for ($i=0;$i<100000;$i++)
	{
		$DB->select('log',$select,"`id`<20 or `operation`='mu'");
		$DB->queryCollect();
		//$DB->exec();	
		//$DB->getAssocArray();
		
		$DB->insert('log',$arr);
		$DB->queryCollect();
		//$DB->exec();
		
		$arr['operation'] = 'um';		
		$DB->update('log',$arr,'1');
		$arr['operation'] = 'mu';
		$DB->limit(50);
		$DB->queryCollect();
		//$DB->exec();	
	}
?>
