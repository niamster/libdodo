<form enctype="multipart/form-data" method="POST">
	<input type="file" name="file">
	<input type="hidden" name="hidden" value="post">
	<input type="text" name="text" value="text">
	<input type="submit" value="push file">
</form>
<form method="POST">
	<input type="hidden" name="hidden" value="post">
	<input type="text" name="text" value="text">
	<input type="submit" value="push">
</form>

<pre>
	<(* test template *)>

	<(assign a = 1)>
	<(assign b = 2)>

	<(include menu.tpl)>

	<(assign b = $a)>

	<(include $main)>

	<(> <( not )> parsed area <)>

	<(print test=, $test)>

	<(if $test!=test)>
		$test != 'test'
	<(else)>
		$test == 'test'
	<(fi)>

	<(if $test)>
		$test
		<(if $test==test)>
			$test == 'test'
		<(fi)>
		<(if $test!=test)>
			$test != 'test'
		<(fi)>

		<(if $test)>
			test
			<(if $test==test)>
				$test == 'test'
			<(else)>
				$test != 'test'
			<(fi)>
		<(else)>
			<(if $test!=test)>
				$test != 'test'
			<(fi)>
			<(if false)>
				WTF?
			<(fi)>
		<(fi)>

		<(if 3>2)>
			3>2
		<(fi)>

		<(if 3<2)>
			WTF?
		<(fi)>

		<(if!false)>!false<(fi)>
	<(fi)>

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

	<(for $i in $strmaparr)>
		<(for $i in $i)> <(* HERE : '$i in $i' is OK because of the namespace scope *)>
			|<(print $i)>
			<(print $dodo.iterator)>
		<(rof)>
		|<(print $i.one)>
		<(print $dodo.iterator)>
	<(rof)>

	<(assign a = test1)>

	<(if true)>
		<(assign a = test2)>

		<(for $i in $strmaparr)>
			<(for $i => $j in $i)>
				<(if $i == three)>
					<(break 3)>
				<(fi)>
				<(print $i)> - <(print $j)>

				<(assign a = test3)>
				<(print a= ', $a,' (must be equal to 'test3'))>
			<(rof)>
			-------
		<(rof)>
		<(print a= ', $a,' (must be equal to 'test2'))>
	<(fi)>
	<(print a= ', $a,' (must be equal to 'test1'))>

	<(ns)>
			<(ns)>
				<(assign a = in namespace)>
				<(print a= ', $a,' (must be equal to 'in namespace'))>
			<(sn)>
		<(print a= ', $a,' (must be equal to 'test1'))>
	<(sn)>

	<(print string literal)>
	<(print {$strmaparr.{0}.{$one}})>
	<(print $strmaparr.1.{$strmap.{$one}})>
	<(print $strarr.0.0)>
	<(print $strmap.one)>
	<(print $strmaparr.0.one)>
	<(print $strmaparr.1.one)>
	<(print $strmaparr.1.{$strmap.{$one}}, " -- ", {$strmaparr.{0}.{$one}})>
	<(print $dodo.version, " : ", $dodo)>
</pre>
