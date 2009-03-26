<?php
	$g_hostname = 'localhost';
	$g_db_type = 'mysql';
	$g_database_name = '<db>';
	$g_db_username = '<db user>';
	$g_db_password = '<db password>';
	
	$g_allow_anonymous_login = ON;
	$g_anonymous_account = 'anonymous';

	$g_allow_blank_email = ON;

	$g_show_version = OFF;

	$g_window_title = 'libdodo issue tracker';

	$g_project_override = 1;
	
	$g_administrator_email  = 'info@libdodo.org';
	$g_webmaster_email      = 'info@libdodo.org';
	$g_from_email           = 'noreply@libdodo.org';
	$g_return_path_email    = 'info@libdodo.org';

	$g_host = 'http://issues.libdodo.org/';
	$t_host = 'http://issues.libdodo.org/';

	$g_default_advanced_report = ON;
	$g_default_advanced_view   = ON;
	
	
	$g_source_control_account            = 'hg';
	$g_source_control_regexp             = '/(?:bug|issue)\s*[#]{0,1}(\d+)/im';
	$g_source_control_set_status_to      = RESOLVED;
	$g_source_control_set_resolution_to  = FIXED;
	$g_source_control_fixed_regexp       = '/(?:fix|resolv)(?:ed|es|e)\s*(?:bug|issue)?\s*[#]{0,1}(\d+)/im';
?>
