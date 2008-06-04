
<(assign a = 1)>
<(assign b = 2)>

included menu.tpl: <(include "menu.tpl")>

<(assign b = $a)>

included $main <(include $main)>



<(> not )> parsed  <)>

<(* if statement *)>

<(if $test!=test )>
	<(print $test)>
<(else)>
	NULL
<(fi)>

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

<(if $test )>
	$test
	<(if $test==test )>
		$test==test
	<(fi)>
	<(if $test==test1 )>
		<(print $test,"==test1")>
	<(fi)>	
	
	<(if $test )>
		<(print "is ", $test )>
		<(if $test==test )>
			$test==test
		<(else)>
			NULL
		<(fi)>
	<(else)>	
		<(if $test!=test )>
			test
		<(fi)>	
		<(if false )>
			WTF?
		<(fi)>
	<(fi)>
	<(if !$test )>
		<(print "not ", $test )>
	<(else)>
		<(print "is ", $test )>
	<(fi)>
	
	<(if 3>2 )>
		3>2
	<(fi)>

	<(if 3<2 )>
		3<2
	<(fi)>
	
	<(if 3!=2 )>
		3!=2
	<(fi)>
	
	<(if 
		! `false`)>
		not false
	<(fi)>
	
	<(if!false)>not false<(fi)>
	
	<(if ! 'false' )>
		not false
	<(fi)>		
	<(if ! "false" )>
		not false
	<(fi)>	
<(fi)>

<( print bla-bla )>
<( print $show )>

<(for $i in $arr)>
	<(for $b in $i)>
		<div class="test"><(print $b)></div>
	<(rof)>
<(rof)>


<(print $test )>
<(print $arr.0.0 )>
<(print $arr1.one )>

<(print $arr2.0.one )>
<(print $arr2.1.one )>


<(for $b in $arr2)>
	<(for $c in $b)>
		|<(print $c)>|<(print $b.one)>
	<(rof)>
<(rof)>

<(for $c in $arr2.1)>
	|<(print $c)>
<(rof)>

<(print {$arr2.{0}.{$one}} )>
<(print $arr2.1.{$arr1.{$one}} )>

<(print namespace: )>

<(for $b in $arr2)>
	<(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
		|<(print $b)>
		<(print $dodo.iterator)>
	<(rof)>
	|<(print $b.one)>
	<(print $dodo.iterator)>
<(rof)>

<(assign a = test1)>
<(print $a," must be test1")>

<(if true )>	

	<(assign a = test2)>
	<(print $a," must be test2")>

	<(for $b in $arr2)>
		<(for $b => $c in $b)>
			<(if $b == three)>
				<(break 3)>
			<(fi)>
			<(print $b)> -> <(print $c)>
			
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

<(print $arr2.1.{$arr1.{$one}} , !!!! , {$arr2.{0}.{$one}})>

--
<(for $i in $arr)>
	<(print $i)>
<(rof)>
--

<(>
<(print $dodo.version, " ", $dodo)>
<)>

<(print $dodo.version, " ", $dodo)>
