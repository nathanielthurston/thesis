LINE: while (<>) {
	chomp;
	@tt = split(/[gG]/, "$_$_");
	shift @tt;
	$tt = join(".", @tt);
	$gp = $_;
	$gp =~ s/[mn]//gi;
	print "$_ 4\n" if $gp =~ /^....$/;
	next unless $gp =~ /^.....$/;
	$gpp = $gp . $gp;
	foreach $i (0..4) {
		$gg1 = substr($gpp, $i, 2);
		$tt1 = $tt[$i];
		$gg2 = substr($gpp, $i+2, 2);
		$tt2 = $tt[$i+2];
		$gg2i = reverse $gg2;
		$gg2i =~ y/gG/Gg/;
		$tt2i = reverse $tt2;
		$tt2i =~ y/mMnN/MmNn/;
		if ($gg1 eq $gg2 && $tt1 eq $tt2) {
			print "$_ $tt $i $gg1 $tt1\n";
			next LINE;
		}
		if ($gg1 eq $gg2i && $tt1 eq $tt2i) {
			print "$_ $tt I$i $gg1 $tt1\n";
			next LINE;
		}
	}
}
