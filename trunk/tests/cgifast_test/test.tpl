
<form enctype="multipart/form-data" method="POST">
	<input type="file" name="file">
	<input type="submit" value="push file">
</form>

<pre>

	<(* test template *)>

	<(> <( not )> parsed area <)>

	**********

	simple 'if' statement

	<(>
		<(print $test)>
		<(if $test!=test )>
			equal
		<(else)>
			not equal
		<(fi)>
	<)>

	<(print $test)>
	<(if $test!=test )>
		equal
	<(else)>
		not equal
	<(fi)>

	**********

	nested 'if' statements

	<(>
		<(if $test )>
			$test
			<(if $test==test )>
				test
			<(fi)>
			<(if $test!=test )>
				!test
			<(fi)>	
			
			<(if $test )>
				test
				<(if $test==test )>
					<(print $test)>
				<(else)>
					NULL
				<(fi)>
			<(else)>	
				<(if $test!=test )>
					!test
				<(fi)>	
				<(if false )>
					WTF?
				<(fi)>
			<(fi)>	
			
			<(if 3>2 )>
				3>2
			<(fi)>

			<(if 3<2 )>
				WTF?
			<(fi)>
			
			<(
				if
				! `false`
			)>
				!false
			<(fi)>
			
			<(if!false)>!false<(fi)>
			
			<(if ! 'false' )>
				!'false'
			<(fi)>		
			<(if ! "false" )>
				! "false"
			<(fi)>	
		<(fi)>
	<)>

	<(if $test )>
		$test
		<(if $test==test )>
			test
		<(fi)>
		<(if $test!=test )>
			!test
		<(fi)>	
		
		<(if $test )>
			test
			<(if $test==test )>
				<(print $test)>
			<(else)>
				NULL
			<(fi)>
		<(else)>	
			<(if $test!=test )>
				!test
			<(fi)>	
			<(if false )>
				WTF?
			<(fi)>
		<(fi)>	
		
		<(if 3>2 )>
			3>2
		<(fi)>

		<(if 3<2 )>
			WTF?
		<(fi)>
		
		<(
			if
			! `false`
		)>
			!false
		<(fi)>
		
		<(if!false)>!false<(fi)>
		
		<(if ! 'false' )>
			!'false'
		<(fi)>		
		<(if ! "false" )>
			! "false"
		<(fi)>	
	<(fi)>

	**********

	simple 'for' statement
	
	<(>
		<(for $i in $strarr)>
			<(for $j in $i)><(print $j)><(rof)>
		<(rof)>

		<(for $b in $strmaparr)>
			<(for $c in $b)>
				|<(print $c)>|<(print $b.one)>
			<(rof)>
		<(rof)>

		<(for $c in $strmaparr.1)>
			|<(print $c)>
		<(rof)>
	<)>

	<(for $i in $strarr)>
		<(for $j in $i)><(print $j)><(rof)>
	<(rof)>

	<(for $b in $strmaparr)>
		<(for $c in $b)>
			|<(print $c)>|<(print $b.one)>
		<(rof)>
	<(rof)>

	<(for $c in $strmaparr.1)>
		|<(print $c)>
	<(rof)>

	**********

	namespace

	<(>
		<(for $i in $strmaparr)>
			<(for $i in $i)> <(* HERE : '$i in $i' is OK because of the namespace scope *)>
				|<(print $i)>
				<(print $dodo.iterator)>
			<(rof)>
			|<(print $i.one)>
			<(print $dodo.iterator)>
		<(rof)>
	
		<(assign a = test1)>

		<(if true )>	
			<(assign a = test2)>

			<(for $i in $strmaparr)>
				<(for $i => $j in $i)>
					<(if $i == three)>
						<(break 3)>
					<(fi)>
					<(print $i)> - <(print $j)>
					
					<(assign a = test3)>
					<(print $a," must be test3")>
				<(rof)>
				-------
			<(rof)>	
			<(print $a," must be test2")>
		<(fi)>
		<(print $a," must be test1")>
		
		<(ns)>
				<(ns)>
					<(assign a = in namespace)>
					<(print $a," must be `in namespace`")>
				<(sn)>
			<(print $a," must be test1")>
		<(sn)>
	<)>

	<(for $i in $strmaparr)>
		<(for $i in $i)> <(* HERE : '$i in $i' is OK because of the namespace scope *)>
			|<(print $i)>
			<(print $dodo.iterator)>
		<(rof)>
		|<(print $i.one)>
		<(print $dodo.iterator)>
	<(rof)>

	<(assign a = test1)>

	<(if true )>	
		<(assign a = test2)>

		<(for $i in $strmaparr)>
			<(for $i => $j in $i)>
				<(if $i == three)>
					<(break 3)>
				<(fi)>
				<(print $i)> - <(print $j)>
				
				<(assign a = test3)>
				<(print $a," must be test3")>
			<(rof)>
			-------
		<(rof)>	
		<(print $a," must be test2")>
	<(fi)>
	<(print $a," must be test1")>
		
	<(ns)>
			<(ns)>
				<(assign a = in namespace)>
				<(print $a," must be `in namespace`")>
			<(sn)>
		<(print $a," must be test1")>
	<(sn)>

	**********

	print statement and variable resolution

	<(>
		<(print string literal)>
		<(print {$strmaparr.{0}.{$one}} )>
		<(print $strmaparr.1.{$strmap.{$one}} )>
		<(print $strarr.0.0 )>
		<(print $strmap.one )>
		<(print $strmaparr.0.one )>
		<(print $strmaparr.1.one )>
		<(print $strmaparr.1.{$strmap.{$one}} , -- , {$strmaparr.{0}.{$one}})>
		<(print $dodo.version, " ", $dodo)>
	<)>

	<(print string literal)>
	<(print {$strmaparr.{0}.{$one}} )>
	<(print $strmaparr.1.{$strmap.{$one}} )>
	<(print $strarr.0.0 )>
	<(print $strmap.one )>
	<(print $strmaparr.0.one )>
	<(print $strmaparr.1.one )>
	<(print $strmaparr.1.{$strmap.{$one}} , -- , {$strmaparr.{0}.{$one}})>
	<(print $dodo.version, " ", $dodo)>

</pre>
