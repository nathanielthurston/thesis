while (<>) {
	if (/adding (\d+)=([gGnNmM]+)/) {
		$n = $1 + 7;
		$w = $2;
		$word[$n] = $w;
	}
	if (/ELIMINATED (\d+)/) {
		++$useCount{$word[$1]};
	}
}

foreach (sort keys %useCount) {
	next if $_ eq "";
	print "$useCount{$_} $_\n";
}
